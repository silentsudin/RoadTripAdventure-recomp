#pragma once

#include "preview/preview.h"

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace rt::native::preview {

enum class TitleRow : std::uint8_t { NewGame, Continue };
enum class TitleAction : std::uint8_t { None, StartNewGame, LoadContinue, SaveCorrupt, Back };
enum class TitleTransition : std::uint8_t { Idle, FadingOut, Complete };

struct SaveSlotInfo {
    bool exists = false;
    bool valid = false;
    bool corrupt = false;
    std::string playerName;
    std::uint32_t course = 0;
    std::uint32_t elapsedFrames = 0;
};

class FakeCardBackend {
public:
    const SaveSlotInfo& slot(std::size_t index) const;
    void setSlot(std::size_t index, SaveSlotInfo info);

private:
    std::array<SaveSlotInfo, 2> slots_{};
};

struct TitleUiDrawData {
    std::vector<ScreenQuad> quads;
    std::vector<HudQuad> text;
};

class TitleUiState {
public:
    static constexpr std::uint32_t kFadeFrames = 32;

    explicit TitleUiState(const FakeCardBackend& card);
    const SaveSlotInfo& save() const;
    std::size_t rowCount() const;
    TitleRow row(std::size_t index) const;
    std::size_t selection() const;
    bool enabled(std::size_t index) const;
    TitleTransition transition() const;
    float fadeAlpha() const;
    void up();
    void down();
    TitleAction confirm();
    TitleAction back();
    void tick(std::uint32_t frames = 1);
    TitleUiDrawData emit(int viewportWidth, int viewportHeight) const;

private:
    SaveSlotInfo save_{};
    std::size_t selection_ = 0;
    TitleTransition transition_ = TitleTransition::Idle;
    std::uint32_t fadeFrames_ = 0;
};

}  // namespace rt::native::preview
