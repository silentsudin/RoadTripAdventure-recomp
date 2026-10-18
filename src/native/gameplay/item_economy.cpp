#include "gameplay/item_economy.h"

#include <algorithm>

namespace rt::native::gameplay::item_economy {

namespace {

// Getter bases are the addresses the price getters index (DAT_<base> + slot *
// stride), i.e. record start + price offset. Values are the ELF-read prices
// (test pins them against the ELF); terminator words are the raw words one
// slot past the last record.
const std::vector<PriceTable> kTables = {
    {1, 0x002ECA78, 0x1C, 13, 0x6E696761},
    {2, 0x002ECED8, 0x18, 12, 0x30303033},
    {3, 0x002ED200, 0x10, 5, 0x70732D70},
    {4, 0x002ED330, 0x1C, 6, 0x2C656E69},
    {5, 0x002ED4F0, 0x10, 4, 0x6C6B6369},
    {6, 0x002ED5D8, 0x2C, 4, 0x64206120},
    {13, 0x002F00A8, 0x10, 14, 0x000003E8},
};

const std::vector<std::uint8_t> kUnbounded = {7, 8, 9, 10, 11, 12, 14};

std::vector<CatalogEntry> buildCatalog() {
    std::vector<CatalogEntry> out;
    for (const auto& t : kTables) {
        for (std::uint8_t slot = 0; slot < t.count; ++slot) {
            out.push_back({ItemRef{t.category, slot}, 0});
        }
    }
    static constexpr std::uint32_t kPrices[] = {
        // category 1 tire (13)
        200, 1000, 2000, 5000, 10000, 2000, 3000, 500, 3000, 1000, 3000, 5000, 200000,
        // category 2 gearbox (12)
        200, 500, 1000, 1500, 2000, 4000, 8000, 12000, 16000, 20000, 80000, 160000,
        // category 3 chassis (5)
        200, 500, 1000, 2000, 4000,
        // category 4 engine (6)
        200, 1000, 2000, 4000, 7000, 10000,
        // category 5 steering (4)
        200, 500, 1000, 2000,
        // category 6 brakes (4): getter slot values
        500, 1000, 1500, 2000,
        // category 13 horn (14)
        0, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000,
    };
    for (std::size_t i = 0; i < out.size(); ++i) out[i].price = kPrices[i];
    return out;
}

}  // namespace

const std::vector<PriceTable>& priceTables() { return kTables; }

const std::vector<std::uint8_t>& unboundedCategories() { return kUnbounded; }

const std::vector<CatalogEntry>& catalog() {
    static const std::vector<CatalogEntry> kCatalog = buildCatalog();
    return kCatalog;
}

std::size_t catalogIndex(ItemRef item) {
    const auto& cat = catalog();
    for (std::size_t i = 0; i < cat.size(); ++i) {
        if (cat[i].item == item) return i;
    }
    return cat.size();
}

std::optional<std::uint32_t> priceOf(ItemRef item) {
    const std::size_t idx = catalogIndex(item);
    if (idx >= catalog().size()) return std::nullopt;
    return catalog()[idx].price;
}

SpendOutcome spendTimerBudget(Purse& purse, std::int32_t amount) {
    SpendOutcome out;
    if (amount > purse.money) return out;
    std::int64_t next = static_cast<std::int64_t>(purse.money) - amount;
    if (next > kMoneyCap) {
        purse.clamped += next - kMoneyCap;
        next = kMoneyCap;
    }
    if (amount >= 0) {
        purse.spent += amount;
    } else {
        purse.credited += -static_cast<std::int64_t>(amount);
    }
    purse.money = static_cast<std::int32_t>(next);
    out.status = SpendStatus::Accepted;
    out.sfx = amount != 0;
    return out;
}

bool Inventory::owned(ItemRef item) const {
    const std::size_t idx = catalogIndex(item);
    return idx < owned_.size() && owned_[idx] != 0;
}

void Inventory::setOwned(ItemRef item, bool value) {
    const std::size_t idx = catalogIndex(item);
    if (idx < owned_.size()) owned_[idx] = value ? 1 : 0;
}

std::size_t Inventory::ownedCount() const {
    return static_cast<std::size_t>(std::count_if(owned_.begin(), owned_.end(),
                                                  [](std::uint8_t v) { return v != 0; }));
}

ShopResult buyItem(Purse& purse, Inventory& inventory, ItemRef item) {
    ShopResult out;
    const auto price = priceOf(item);
    if (!price) {
        out.status = ShopStatus::UnknownItem;
        return out;
    }
    if (static_cast<std::int64_t>(purse.money) < static_cast<std::int64_t>(*price)) {
        out.status = ShopStatus::Insufficient;
        return out;
    }
    if (inventory.owned(item)) {
        out.status = ShopStatus::AlreadyOwned;
        return out;
    }
    const SpendOutcome spend = spendTimerBudget(purse, static_cast<std::int32_t>(*price));
    if (spend.status != SpendStatus::Accepted) {
        out.status = ShopStatus::Insufficient;
        return out;
    }
    inventory.setOwned(item, true);
    out.status = ShopStatus::Bought;
    out.sfx = spend.sfx;
    return out;
}

ShopResult sellItem(Purse& purse, Inventory& inventory, ItemRef item) {
    ShopResult out;
    const auto price = priceOf(item);
    if (!price) {
        out.status = ShopStatus::UnknownItem;
        return out;
    }
    if (!inventory.owned(item)) {
        out.status = ShopStatus::NotOwned;
        return out;
    }
    const auto refund = static_cast<std::int32_t>(sellRefund(*price));
    const SpendOutcome spend = spendTimerBudget(purse, -refund);
    inventory.setOwned(item, false);
    out.status = ShopStatus::Sold;
    out.sfx = spend.sfx;
    return out;
}

bool purseBalanced(const Purse& purse) {
    const std::int64_t expected = static_cast<std::int64_t>(purse.startMoney) + purse.credited -
                                  purse.spent - purse.clamped;
    return expected == purse.money && purse.money >= 0 && purse.money <= kMoneyCap;
}

}  // namespace rt::native::gameplay::item_economy
