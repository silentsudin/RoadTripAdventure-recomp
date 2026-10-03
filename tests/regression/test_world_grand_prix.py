"""World Grand Prix: each of the 7 stages, started from a save edited to that point (Super A
licence, two teammates, the earlier stages done, at the stage's town). The bot races the stage;
the game must mark it done."""

from __future__ import annotations

import pytest

from rtharness import seconds
from rtharness.adventure import (FIELDS, GAME_MAP, PROGRESS, SCENE_RACING, continue_to_factory, drive_race,
                                  scene)
from rtharness.driver import DriverConfig

WGP = GAME_MAP["world_grand_prix"]
STAGES = WGP["stages"]
SUPER_A = 3


def flag_set(flags: bytes, bit: int) -> bool:
    return bool(flags[bit // 8] >> (bit % 8) & 1)


def stage_edits(stage: int) -> dict[int, bytes]:
    """Progress edits for a save that is about to run `stage` (1-based)."""
    edits = {FIELDS["licence"]["offset"]: bytes([SUPER_A]),
             FIELDS["location"]["offset"]: bytes([STAGES[stage - 1][0]])}
    for i, offset in enumerate(WGP["teammate_offsets"]):
        edits[offset] = bytes([i + 1])
    flags = bytearray(16)
    for done in range(1, stage):
        bit = WGP["stage_flag_base"] + done - 1
        flags[bit // 8] |= 1 << (bit % 8)
    for i, value in enumerate(flags):
        if value:
            edits[WGP["flags_offset"] + i] = bytes([value])
    # The earlier stages' results: our team (team 0) took 1st to 3rd each time, the other teams
    # the remaining places, so the final stage ends in a win.
    record = bytes(range(0x1E, 0x1B, -1)) + bytes(range(0x1B, 0x06, -1))
    edits[WGP["results_offset"]] = (stage - 1).to_bytes(4, "little") + record * (stage - 1)
    return edits


def enter_world_grand_prix(game, limit_seconds: float = 120):
    """Factory menu -> World Grand Prix -> through the briefing until the race starts."""
    game.press("down")
    game.run(seconds(1))
    start = game.vblank
    while scene(game) != SCENE_RACING:
        assert game.vblank - start < seconds(limit_seconds), "the stage's race did not start"
        game.press("cross")
        game.run(seconds(3))
    for _ in range(4):  # Adventure races wait for a press before the countdown
        game.press("cross")
        game.run(seconds(2))


@pytest.mark.parametrize("stage", range(1, len(STAGES) + 1),
                         ids=[f"{i + 1}_{c.replace(' ', '_')}" for i, (_, c) in enumerate(STAGES)])
def test_wgp_stage(game_factory, golden, golden_audio, stage):
    game = game_factory(checkpoint="adventure_first_save", progress_edits=stage_edits(stage))
    continue_to_factory(game)
    golden(f"wgp_{stage}_factory", game.frame())
    enter_world_grand_prix(game)
    key = f"wgp_{stage}"

    def check_start():
        golden(key + "_start", game.frame())
        golden_audio(key + "_start", game.audio())

    course = STAGES[stage - 1][1]
    config = DriverConfig(backtrack=course in GAME_MAP["bot"]["backtrack"],
                          follow="far" if course in GAME_MAP["bot"]["wgp_follow_far"] else "near")
    place = drive_race(game, config=config, at={15: check_start})
    assert 1 <= place <= 24
    if stage == len(STAGES):
        # After the final stage the game returns to the factory and (as yet) starts the WGP over
        # rather than playing the ending: what decides the ending is not mapped yet (COVERAGE.md).
        game.run(seconds(5))
        golden(key + "_after", game.frame())
        return
    bit = WGP["stage_flag_base"] + stage - 1
    flags_at = PROGRESS + WGP["flags_offset"]
    for _ in range(20):  # results, team standings, the stage's closing dialogue
        if flag_set(game.read(flags_at, 16), bit):
            break
        game.press("cross")
        game.run(seconds(3))
    assert flag_set(game.read(flags_at, 16), bit), f"stage {stage} was not marked done"
    golden(key + "_after", game.frame())
