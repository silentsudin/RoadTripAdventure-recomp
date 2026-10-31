#include "gameplay/save_slots.h"

#include <cstdlib>
#include <fstream>
#include <system_error>

namespace rt::native::gameplay {
namespace {
std::filesystem::path defaultDirectory() {
    if (const char* overrideDir = std::getenv("RT_SAVE_DIR")) return overrideDir;
    if (const char* home = std::getenv("HOME")) return std::filesystem::path(home) / "Library/Application Support/RoadTripNative";
    return std::filesystem::temp_directory_path() / "RoadTripNative";
}
}

SlotStore::SlotStore(std::filesystem::path directory)
    : directory_(directory.empty() ? defaultDirectory() : std::move(directory)) {}

std::filesystem::path SlotStore::path(int slot) const {
    if (slot < 0 || slot > 1) return {};
    return directory_ / ("slot" + std::to_string(slot) + ".sav");
}

bool SlotStore::save(int slot, const SaveData& state, std::string* error) const {
    const auto target = path(slot);
    if (target.empty()) { if (error) *error = "invalid slot"; return false; }
    std::error_code ec;
    std::filesystem::create_directories(directory_, ec);
    if (ec) { if (error) *error = ec.message(); return false; }
    save_v2::Container container;
    const auto image = serializeSave(state);
    std::copy(image.begin(), image.end(), container.image.begin());
    const auto bytes = save_v2::serializeV2(container);
    const auto tmp = target.string() + ".tmp";
    { std::ofstream out(tmp, std::ios::binary | std::ios::trunc);
      if (!out || !out.write(reinterpret_cast<const char*>(bytes.data()), static_cast<std::streamsize>(bytes.size())) || !out.flush()) {
          if (error) *error = "write failed"; std::filesystem::remove(tmp, ec); return false;
      }
    }
    std::filesystem::rename(tmp, target, ec);
    if (ec) { std::filesystem::remove(target, ec); ec.clear(); std::filesystem::rename(tmp, target, ec); }
    if (ec) { if (error) *error = ec.message(); std::filesystem::remove(tmp, ec); return false; }
    return true;
}

bool SlotStore::load(int slot, SaveData& state, std::string* error) const {
    const auto source = path(slot);
    if (source.empty()) { if (error) *error = "invalid slot"; return false; }
    std::ifstream in(source, std::ios::binary);
    if (!in) { if (error) *error = "slot unavailable"; return false; }
    std::vector<std::uint8_t> bytes((std::istreambuf_iterator<char>(in)), {});
    if (in.bad()) { if (error) *error = "read failed"; return false; }
    save_v2::LoadReport report;
    auto container = save_v2::loadAny(bytes, report); // raw v1 migration; v2 record recovery
    if (report.status == save_v2::Status::BadMagic || report.status == save_v2::Status::BadVersion ||
        report.status == save_v2::Status::BadLayout || report.status == save_v2::Status::Truncated) {
        if (error) *error = "invalid or truncated save";
        return false;
    }
    state = save_v2::toSaveData(container);
    return true;
}

std::array<bool, 2> SlotStore::list() const {
    return {std::filesystem::is_regular_file(path(0)), std::filesystem::is_regular_file(path(1))};
}

SaveData SlotStore::fresh() { return save_v2::toSaveData(save_v2::Container{}); }

bool runSaveAction(std::string_view action, int slot, SlotStore& store, SaveData& progress,
                   std::string* error) {
    if (action == "save.save_state") return store.save(slot, progress, error);
    if (action == "save.load_state") return store.load(slot, progress, error);
    if (error) *error = "unknown save action";
    return false;
}

} // namespace rt::native::gameplay
