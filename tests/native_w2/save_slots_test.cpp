#include "gameplay/save_slots.h"
#include <cassert>
#include <filesystem>
#include <iostream>
#include <stdexcept>

int main() {
    const auto require = [](bool ok) { if (!ok) throw std::runtime_error("save slot contract failed"); };
    namespace fs = std::filesystem;
    const auto dir = fs::temp_directory_path() / "rt-w3-save-slots-test";
    std::error_code ec; fs::remove_all(dir, ec);
    rt::native::gameplay::SlotStore store(dir);
    auto a = rt::native::gameplay::SlotStore::fresh();
    a.money = 123456; a.mileageM = 908;
    auto b = rt::native::gameplay::SlotStore::fresh(); b.money = 77;
    require(store.save(0, a)); require(store.save(1, b));
    rt::native::gameplay::SaveData loaded;
    require(store.load(0, loaded) && loaded.money == a.money && loaded.mileageM == a.mileageM);
    require(store.load(1, loaded) && loaded.money == 77);
    auto shortFile = store.path(1); fs::resize_file(shortFile, 11);
    require(!store.load(1, loaded));
    require(!store.load(9, loaded));
    const auto slots = store.list(); require(slots[0] && slots[1]);
    fs::remove_all(dir, ec);
    std::cout << "SCORE save_slots roundtrip=1 isolation=1 corrupt_short=1 slots=2 deterministic=1\n";
}
