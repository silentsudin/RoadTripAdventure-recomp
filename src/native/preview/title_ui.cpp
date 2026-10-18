#include "preview/title_ui.h"

#include "preview/race_hud.h"

#include <algorithm>

namespace rt::native::preview {
namespace {
constexpr std::array<TitleRow, 2> kRows{TitleRow::NewGame, TitleRow::Continue};
constexpr std::array<float, 4> kPanel{0.03f, 0.06f, 0.16f, 0.86f};
constexpr std::array<float, 4> kSelected{0.15f, 0.48f, 0.94f, 0.90f};
constexpr std::array<float, 4> kEnabled{1.f, 0.94f, 0.66f, 1.f};
constexpr std::array<float, 4> kDisabled{0.45f, 0.47f, 0.52f, 0.82f};

float frameScale(int width, int height) {
    return std::min(static_cast<float>(std::max(width, 1)) / 640.f,
                    static_cast<float>(std::max(height, 1)) / 480.f);
}

ScreenQuad designRect(float x, float y, float width, float height,
                      const std::array<float, 4>& color, int viewportWidth, int viewportHeight) {
    const float scale = frameScale(viewportWidth, viewportHeight);
    const float offsetX = (static_cast<float>(viewportWidth) - 640.f * scale) * 0.5f;
    const float offsetY = (static_cast<float>(viewportHeight) - 480.f * scale) * 0.5f;
    return {offsetX + x * scale, offsetY + (y + 16.f) * scale, width * scale, height * scale, color};
}

const char* label(TitleRow row) {
    return row == TitleRow::NewGame ? "NEW GAME" : "CONTINUE";
}
}  // namespace

const SaveSlotInfo& FakeCardBackend::slot(std::size_t index) const {
    static const SaveSlotInfo kMissing{};
    return index < slots_.size() ? slots_[index] : kMissing;
}

void FakeCardBackend::setSlot(std::size_t index, SaveSlotInfo info) {
    if (index < slots_.size()) slots_[index] = std::move(info);
}

TitleUiState::TitleUiState(const FakeCardBackend& card) : save_(card.slot(0)) {}

const SaveSlotInfo& TitleUiState::save() const { return save_; }
std::size_t TitleUiState::rowCount() const { return kRows.size(); }
TitleRow TitleUiState::row(std::size_t index) const { return kRows.at(index); }
std::size_t TitleUiState::selection() const { return selection_; }
bool TitleUiState::enabled(std::size_t index) const {
    return index < kRows.size() && (kRows[index] == TitleRow::NewGame || save_.exists);
}
TitleTransition TitleUiState::transition() const { return transition_; }
float TitleUiState::fadeAlpha() const {
    return transition_ == TitleTransition::Complete ? 1.f
           : static_cast<float>(fadeFrames_) / static_cast<float>(kFadeFrames);
}

void TitleUiState::up() {
    while (selection_ > 0) {
        --selection_;
        if (enabled(selection_)) return;
    }
}

void TitleUiState::down() {
    for (std::size_t candidate = selection_ + 1; candidate < kRows.size(); ++candidate) {
        if (enabled(candidate)) {
            selection_ = candidate;
            return;
        }
    }
}

TitleAction TitleUiState::confirm() {
    if (transition_ != TitleTransition::Idle) return TitleAction::None;
    if (row(selection_) == TitleRow::Continue) {
        if (!save_.exists) return TitleAction::None;
        if (!save_.valid || save_.corrupt) return TitleAction::SaveCorrupt;
        transition_ = TitleTransition::FadingOut;
        return TitleAction::LoadContinue;
    }
    transition_ = TitleTransition::FadingOut;
    return TitleAction::StartNewGame;
}

TitleAction TitleUiState::back() { return transition_ == TitleTransition::Idle ? TitleAction::Back : TitleAction::None; }

void TitleUiState::tick(std::uint32_t frames) {
    if (transition_ != TitleTransition::FadingOut) return;
    fadeFrames_ = std::min(kFadeFrames, fadeFrames_ + frames);
    if (fadeFrames_ == kFadeFrames) transition_ = TitleTransition::Complete;
}

TitleUiDrawData TitleUiState::emit(int viewportWidth, int viewportHeight) const {
    TitleUiDrawData draw;
    // Authored preview placement: 640x448 UI design coordinates are placed in
    // the centred 640x480 output frame (+16 Y), matching fonteTextQuads.
    draw.quads.push_back(designRect(144.f, 136.f, 352.f, 152.f, kPanel, viewportWidth, viewportHeight));
    for (std::size_t i = 0; i < kRows.size(); ++i) {
        const float y = 160.f + static_cast<float>(i) * 56.f;
        if (i == selection_) draw.quads.push_back(designRect(168.f, y, 304.f, 40.f, kSelected, viewportWidth, viewportHeight));
        appendFonteText(draw.text, label(kRows[i]), 192.f, y + 8.f, enabled(i) ? kEnabled : kDisabled,
                        viewportWidth, viewportHeight);
    }
    if (fadeAlpha() > 0.f)
        draw.quads.push_back({0.f, 0.f, static_cast<float>(viewportWidth), static_cast<float>(viewportHeight),
                              {0.f, 0.f, 0.f, fadeAlpha()}});
    return draw;
}

}  // namespace rt::native::preview
