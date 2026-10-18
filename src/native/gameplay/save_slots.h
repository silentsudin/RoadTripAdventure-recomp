#pragma once

#include "gameplay/save_v2.h"

#include <array>
#include <filesystem>
#include <optional>
#include <string>
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

} // namespace rt::native::gameplay
