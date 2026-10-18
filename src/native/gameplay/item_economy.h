// Item economy: the single purse funnel (SpendTimerBudget 0x23DC58) and the
// shop buy/sell flow over ELF-read per-category price tables.
// See docs/native-reference/item-economy.md for the evidence and pins.
#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

namespace rt::native::gameplay::item_economy {

// SpendTimerBudget 0x23DC58: cap 999999 (0xF423F), cash SFX 0x305 iff amount != 0.
inline constexpr std::int32_t kMoneyCap = 999999;
inline constexpr std::int32_t kStartingMoney = 1000;  // seed @0x229FFC/0x22A004
inline constexpr std::uint16_t kCashSfx = 0x305;

// `category` is the price-getter class: selector word 0x2DC8A8[row] = row + 1,
// dispatched through the 15-entry getter table 0x2F1280 (class 0 = flat 500).
struct ItemRef {
    std::uint8_t category = 0;
    std::uint8_t slot = 0;
    friend bool operator==(const ItemRef& a, const ItemRef& b) {
        return a.category == b.category && a.slot == b.slot;
    }
};

struct CatalogEntry {
    ItemRef item;
    std::uint32_t price;
};

// One ELF price getter that indexes a fixed-stride record table:
// price(slot) = u32 @ (base + stride * slot), slot < count.
struct PriceTable {
    std::uint8_t category;
    std::uint32_t getterBase;
    std::uint32_t stride;
    std::uint8_t count;
    std::uint32_t terminatorWord;  // raw word at slot == count (ELF boundary pin)
};

// Price getters with a decoded, bounded record table (see the doc, §2).
const std::vector<PriceTable>& priceTables();
// Getter classes present in the selector but not bounded (GAP): 7..12, 14.
const std::vector<std::uint8_t>& unboundedCategories();

const std::vector<CatalogEntry>& catalog();
std::optional<std::uint32_t> priceOf(ItemRef item);
std::size_t catalogIndex(ItemRef item);  // npos-like: catalog().size() when absent

// Sell refund: credit price / 2 (truncating), 0x269284 via 0x268E40.
inline std::uint32_t sellRefund(std::uint32_t price) { return price / 2; }

struct Purse {
    std::int32_t startMoney = kStartingMoney;
    std::int32_t money = kStartingMoney;
    std::int64_t spent = 0;     // sum of accepted positive amounts
    std::int64_t credited = 0;  // sum of accepted credits (-amount)
    std::int64_t clamped = 0;   // amount discarded by the 999999 cap
};

enum class SpendStatus : std::uint8_t { Accepted, Insufficient };

struct SpendOutcome {
    SpendStatus status = SpendStatus::Insufficient;
    bool sfx = false;
};

// Mirror of SpendTimerBudget: refuse when amount > money, else money -= amount,
// clamp to the cap, play the cash SFX iff amount != 0. Negative amount = credit.
SpendOutcome spendTimerBudget(Purse& purse, std::int32_t amount);

// Owned flags for one player: SetFlaggedSlotBit / TestFlaggedSlotBit (one bit
// per catalog entry).
class Inventory {
public:
    Inventory() = default;
    explicit Inventory(std::size_t catalogSize) : owned_(catalogSize, 0) {}
    bool owned(ItemRef item) const;
    void setOwned(ItemRef item, bool value);
    std::size_t ownedCount() const;

private:
    std::vector<std::uint8_t> owned_;
};

enum class ShopStatus : std::uint8_t {
    Bought,
    Sold,
    Insufficient,   // 0x200 message: money < price
    AlreadyOwned,   // 0x300 message: TestFlaggedSlotBit set
    NotOwned,       // sell of an item not held
    UnknownItem,    // not in the catalog
};

struct ShopResult {
    ShopStatus status = ShopStatus::UnknownItem;
    bool sfx = false;
};

// Shop state-3 purchase: pre-check money < price, then owned, then pay + set bit.
ShopResult buyItem(Purse& purse, Inventory& inventory, ItemRef item);
// Sell: clear owned, credit price / 2 through the funnel.
ShopResult sellItem(Purse& purse, Inventory& inventory, ItemRef item);

// Balance invariant. After any sequence of accepted and refused operations:
//   money == startMoney + credited - spent - clamped, 0 <= money <= kMoneyCap.
bool purseBalanced(const Purse& purse);

}  // namespace rt::native::gameplay::item_economy
