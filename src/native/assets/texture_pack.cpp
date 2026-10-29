// src/native/assets/texture_pack.cpp — see texture_pack.h.
#include "assets/texture_pack.h"

#include "assets/hd_pack.h"
#include "assets/setup_wizard.h"

#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <set>

namespace rt::native::assets {

struct TexturePack::PackIndexHolder {
    PackIndex index;
};

namespace {
std::filesystem::path& dumpDirRef() {
    static std::filesystem::path dir;
    return dir;
}
std::set<std::uint64_t>& dumpedKeys() {
    static std::set<std::uint64_t> keys;
    return keys;
}
void note(const std::string& message) { std::cout << "[texturepack] " << message << '\n'; }
}  // namespace

TexturePack::TexturePack() = default;
TexturePack::~TexturePack() = default;

TexturePack& activeTexturePack() {
    static TexturePack pack;
    return pack;
}

std::filesystem::path packsRoot() { return dataRoot() / "textures" / "packs"; }

std::filesystem::path resolvePackPath(const std::string& value) {
    if (value.empty()) return {};
    std::error_code ec;
    if (std::filesystem::is_directory(value, ec)) return std::filesystem::path(value);
    return packsRoot() / value;
}

bool TexturePack::load(const std::filesystem::path& folder, std::string& error) {
    cache_.clear();
    served_ = 0;
    index_.reset();
    active_ = false;
    imageCount_ = 0;
    ++generation_;  // a renderer's texture cache must not survive a switch
    if (folder.empty()) {
        folder_.clear();
        error.clear();
        return false;
    }
    auto holder = std::make_unique<PackIndexHolder>();
    holder->index = scanPackFolder(folder);
    if (!holder->index.valid()) {
        error = "pack folder is not readable: " + folder.string();
        note(error);
        return false;
    }
    folder_ = folder;
    imageCount_ = holder->index.size();
    index_ = std::move(holder);
    active_ = true;
    note(folder_.string() + ": " + std::to_string(imageCount_) + " images, " +
         std::to_string(index_->index.warnings.collisions) + " key collisions");
    return true;
}

void TexturePack::clear() {
    std::string ignored;
    (void)load({}, ignored);
}

const DecodedImage* TexturePack::replacement(std::uint32_t width, std::uint32_t height,
                                             std::span<const std::uint8_t> rgba) {
    if (!active_ || !index_) return nullptr;
    const StableContentKey key = stableContentKey(width, height, rgba);
    if (key.isNull()) return nullptr;
    auto cached = cache_.find(key.value);
    if (cached != cache_.end()) return cached->second ? &*cached->second : nullptr;

    const ReplacementRequest request = requestReplacement(index_->index, key, width, height);
    if (request.verdict != ReplacementVerdict::Replace) {
        if (request.verdict == ReplacementVerdict::StaleMismatch)
            note("stale pack image skipped: " + request.note);
        cache_[key.value] = std::nullopt;
        return nullptr;
    }
    // A pack image is untrusted: never read a file past the size a legal pack
    // image can have, whatever the directory listing says.
    std::error_code ec;
    const auto size = std::filesystem::file_size(request.path, ec);
    if (ec || size == 0 || size > kMaxPackFileBytes) {
        note("pack image skipped (" + request.path.filename().string() + "): " +
             (ec ? "unreadable" : "not a pack-image size"));
        cache_[key.value] = std::nullopt;
        return nullptr;
    }
    std::vector<std::uint8_t> bytes(static_cast<std::size_t>(size));
    std::ifstream file(request.path, std::ios::binary);
    file.read(reinterpret_cast<char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
    if (!file || file.gcount() != static_cast<std::streamsize>(bytes.size())) {
        note("pack image unreadable: " + request.path.string());
        cache_[key.value] = std::nullopt;
        return nullptr;
    }
    if (bytes.empty()) {
        note("pack image unreadable: " + request.path.string());
        cache_[key.value] = std::nullopt;
        return nullptr;
    }
    DecodedImage image = decodePackImage(bytes);
    if (!image.ok) {
        note("pack image refused (" + request.path.filename().string() + "): " + image.why);
        cache_[key.value] = std::nullopt;
        return nullptr;
    }
    ++served_;
    return &cache_.emplace(key.value, std::move(image)).first->second.value();
}

void setDumpDirectory(const std::filesystem::path& dir) {
    dumpDirRef() = dir;
    dumpedKeys().clear();
}
const std::filesystem::path& dumpDirectory() { return dumpDirRef(); }

std::filesystem::path dumpTexture(std::uint32_t width, std::uint32_t height,
                                  std::span<const std::uint8_t> rgba) {
    if (dumpDirRef().empty()) return {};
    const StableContentKey key = stableContentKey(width, height, rgba);
    if (key.isNull()) return {};
    if (!dumpedKeys().insert(key.value).second) return {};  // same texture again: already written
    const DumpWriteError err = writeDumpFile(dumpDirRef(), key, width, height, rgba, DumpFormat::PamRgba);
    if (err != DumpWriteError::Ok) {
        std::cout << "[texturepack] dump failed for " << stableKeyToString(key) << '\n';
        return {};
    }
    return planDump(dumpDirRef(), key, width, height, DumpFormat::PamRgba).path;
}

}  // namespace rt::native::assets
