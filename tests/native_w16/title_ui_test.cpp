#include "preview/title_ui.h"

#include <cstdio>
#include <string_view>

namespace {
int failures = 0;
int checks = 0;
void check(bool ok, const char* what) {
    ++checks;
    if (!ok) { ++failures; std::printf("FAIL %s\n", what); }
}
}  // namespace

int main() {
    using namespace rt::native::preview;
    FakeCardBackend card;
    TitleUiState ui(card);
    check(ui.rowCount() == 2, "ROM-grounded two-row title menu");
    check(ui.row(0) == TitleRow::NewGame && ui.row(1) == TitleRow::Continue, "New Game then Continue");
    check(ui.selection() == 0, "initial selection is New Game");
    check(ui.enabled(0) && !ui.enabled(1), "Continue disabled without save");
    ui.down();
    check(ui.selection() == 0, "clamp and skip unavailable Continue");

    card.setSlot(0, {.exists = true, .valid = true, .corrupt = false, .playerName = "RACER"});
    TitleUiState loaded(card);
    loaded.down();
    check(loaded.selection() == 1, "Continue selected when a save exists");
    loaded.down();
    check(loaded.selection() == 1, "navigation clamps at final row");
    check(loaded.confirm() == TitleAction::LoadContinue, "valid Continue requests load");
    check(loaded.transition() == TitleTransition::FadingOut, "confirm begins deterministic fade");
    loaded.tick(31);
    check(loaded.transition() == TitleTransition::FadingOut && loaded.fadeAlpha() < 1.f, "fade remains active before frame 32");
    loaded.tick();
    check(loaded.transition() == TitleTransition::Complete && loaded.fadeAlpha() == 1.f, "fade completes at frame 32");

    card.setSlot(0, {.exists = true, .valid = false, .corrupt = true});
    TitleUiState corrupt(card);
    corrupt.down();
    check(corrupt.confirm() == TitleAction::SaveCorrupt && corrupt.transition() == TitleTransition::Idle,
          "corrupt existing save is reported without transition");

    auto mutated = card.slot(0);
    mutated.corrupt = false;
    check(mutated.corrupt != card.slot(0).corrupt, "negative control catches corrupt flag mutation");
    std::printf("checks=%d failed=%d negative_control=%s\n", checks, failures,
                mutated.corrupt == card.slot(0).corrupt ? "MISSED" : "CAUGHT");
    return failures == 0 ? 0 : 1;
}
