#pragma once

// src/native/assets/texture_pack.h
//
// The app-side glue over assets/hd_pack: one active HD pack, decoded images,
// and per-content lookups. hd_pack owns the naming/index/replacement contract
// (see hd_pack.h and docs/native-reference/hd-texture-packs.md); this module
// only turns a pack folder into pixels the renderers can bind, with the
// per-content verdict cached so a frame never re-reads a file.
//
// Contract kept from hd_pack (and the sibling recomp): a pack problem never
// changes a drawn pixel — a miss, an unreadable file, a stale `_src` guard or
// a refused image all fall back to the native texture, and the reason is
// logged once.

#include "assets/png_decode.h"

#include <cstdint>
#include <filesystem>
#include <map>
#include <memory>
#include <optional>
#include <span>
#include <string>

namespace rt::native::assets {

class TexturePack {
  public:
    TexturePack();
    ~TexturePack();
    TexturePack(const TexturePack&) = delete;
    TexturePack& operator=(const TexturePack&) = delete;
    // Loads `folder` (a bare name is resolved under packsRoot() by the caller)
    // and drops everything decoded before. An empty path deactivates the pack.
    // False means no pack is active; `error` explains a requested one.
    bool load(const std::filesystem::path& folder, std::string& error);
    void clear();

    bool active() const { return active_; }
    // Bumped on every load/clear: renderers must key their texture caches by
    // it so a switch cannot keep serving the previous pack's image.
    std::uint32_t generation() const { return generation_; }
    const std::filesystem::path& folder() const { return folder_; }
    std::size_t imageCount() const { return imageCount_; }
    std::size_t replacementsServed() const { return served_; }

    // The pack image for one decoded texture, or nullptr when the pack has
    // none (miss, unreadable, refused). Cached per content key.
    const DecodedImage* replacement(std::uint32_t width, std::uint32_t height,
                                    std::span<const std::uint8_t> rgba);

  private:
    bool active_ = false;
    std::uint32_t generation_ = 0;
    std::filesystem::path folder_;
    struct PackIndexHolder;  // hides hd_pack.h from callers
    std::unique_ptr<PackIndexHolder> index_;
    std::map<std::uint64_t, std::optional<DecodedImage>> cache_;
    std::size_t served_ = 0;
    std::size_t imageCount_ = 0;
};

// The process-wide pack (renderers read it; main() loads it once).
TexturePack& activeTexturePack();

// <data>/textures/packs — where packs live for the app (RT_DATA_DIR aware).
std::filesystem::path packsRoot();

// Resolves an RT_TEXTURE_PACK value: an existing directory is used as-is,
// otherwise it is a pack folder name under packsRoot().
std::filesystem::path resolvePackPath(const std::string& value);

// ── Dump mode (the other half of the same seam) ─────────────────────────────
// RT_TEXTURE_DUMP=<dir> writes every texture the renderer first uploads as
// "<key>_<W>x<H>.pam" (PAM keeps alpha), the naming a pack folder accepts.
// Dumping changes no pixel: the writer runs beside the upload, never instead
// of it.
void setDumpDirectory(const std::filesystem::path& dir);
const std::filesystem::path& dumpDirectory();
// Writes one texture if dumping is on; returns the path written ("" otherwise).
std::filesystem::path dumpTexture(std::uint32_t width, std::uint32_t height,
                                 std::span<const std::uint8_t> rgba);

}  // namespace rt::native::assets
