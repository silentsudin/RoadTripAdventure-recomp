#pragma once

#include "gameplay/save_v2.h"

#include <array>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace rt::native::gameplay {

class SlotStore {
public:
    explicit SlotStore(std::filesystem::path directory = {});
    std::filesystem::path path(int slot) const;
    bool save(int slot, const SaveData& state, std::string* error = nullptr) const;
    bool load(int slot, SaveData& state, std::string* error = nullptr) const;
    std::array<bool, 2> list() const;
    static SaveData fresh();
private:
    std::filesystem::path directory_;
};

// Options > Save page action rows: a save writes the Adventure progress to the
// host slot chosen by save.slot (0/1), a load replaces it. Unknown ids and store
// failures return false, with the reason in *error.
bool runSaveAction(std::string_view action, int slot, SlotStore& store, SaveData& progress,
                   std::string* error = nullptr);

} // namespace rt::native::gameplay
