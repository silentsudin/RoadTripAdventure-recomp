"""The race against President Forest and the ending: from a save where the World Grand Prix is
won, go to Cloud Hill, get the challenge (through a patched NPC, see game_state.toml), race
Forest on Endurance Run with the bot, then the ending, credits and the "Became the President!"
stamp.

The bot cannot beat Forest yet, so the test turns the game's "lost" into "won" before the
post-race script reads it (coverage-first: the race and the ending both run for real)."""

from __future__ import annotations

import math
import struct

from rtharness import seconds
from rtharness.adventure import FIELDS, GAME_MAP, PROGRESS, SCENE_RACING, continue_to_factory, drive_race, scene
from rtharness.driver import CAR_STRIDE, CARS_BASE, DriverConfig
from rtharness.town import stamps_earned

PRES = GAME_MAP["president"]
WGP = GAME_MAP["world_grand_prix"]
SCENE_TOWN = 2
LOST, WON = 1, 2


def president_edits() -> dict[int, bytes]:
    edits = {FIELDS["licence"]["offset"]: bytes([3]), FIELDS["location"]["offset"]: bytes([PRES["town"]])}
    for i, offset in enumerate(WGP["teammate_offsets"]):
        edits[offset] = bytes([i + 1])
    flags = bytearray(16)
    for bit in PRES["wgp_won_flags"]:
        flags[bit // 8] |= 1 << (bit % 8)
    for i, value in enumerate(flags):
        if value:
            edits[WGP["flags_offset"] + i] = bytes([value])
    return edits


def car(game, slot):
    raw = game.read(CARS_BASE + slot * CAR_STRIDE, 0xE0)
    x, _, z = struct.unpack_from("<3f", raw, 0)
    fx, _, fz = struct.unpack_from("<3f", raw, 0xB0)
    rx, _, rz = struct.unpack_from("<3f", raw, 0xD0)
    return x, z, math.atan2(fz - rz, fx - rx)


def chase(game, slot: int, vblanks: int):
    """Drives at town car `slot` (bumping into a car starts its conversation)."""
    end = game.vblank + vblanks
    while game.vblank < end:
        x, z, heading = car(game, 0)
        tx, tz, _ = car(game, slot)
        err = (math.atan2(tz - z, tx - x) - heading + math.pi) % (2 * math.pi) - math.pi
        lx = int(max(0, min(255, 128 - 127 * max(-1.0, min(1.0, err * 2)))))
        game.step(10, ["cross"] if math.dist((x, z), (tx, tz)) > 12 else [], lx)
    game.release()


def test_president_race_and_ending(game_factory, golden, golden_audio):
    game = game_factory(checkpoint="adventure_first_save", progress_edits=president_edits())
    continue_to_factory(game)
    for _ in range(4):  # Change parts / Race / Save data / Quit game / Drive around town
        game.press("down")
    game.press("cross")
    game.run(seconds(2))
    game.press("cross")  # "Come again!"
    game.run(seconds(12))
    assert scene(game) == SCENE_TOWN
    entry = PRES["npc_table"] + 4 * PRES["dust_entry"]
    game.write(entry, PRES["secretary"].to_bytes(4, "little"))
    chase(game, 2, seconds(70))

    # The secretary, the mansion, Forest: "Do you want to challenge me?" Yes.
    for _ in range(40):
        if scene(game) == SCENE_RACING:
            break
        game.press("cross")
        game.run(seconds(3))
    assert scene(game) == SCENE_RACING, "the race against the President did not start"
    for _ in range(4):  # the countdown waits for a press
        game.press("cross")
        game.run(seconds(2))

    result = PRES["race_result_address"]

    def as_won():
        if game.u32(result) == LOST:
            game.write(result, WON.to_bytes(4, "little"))

    def check_start():
        golden("president_race_start", game.frame())
        golden_audio("president_race_start", game.audio())
        as_won()

    at = {t: as_won for t in range(1, 900)}
    at[15] = check_start
    drive_race(game, config=DriverConfig(wait_for_line=False, moving_leader=True), at=at, ends_in_town=True)
    assert game.u32(result) == WON

    whole = game.read(PROGRESS, 13384)
    shots = {1: "president_won", 8: "president_mansion", 30: "ending_credits", 66: "ending_late_credits",
             69: "ending_thanks", 72: "ending_stamp"}
    for i in range(75):  # Forest's speech, the new body, the mansion, credits, the stamp
        game.run(seconds(3))
        if i in shots:
            golden(shots[i], game.frame())
        game.press("cross")
    game.run(seconds(10))
    assert scene(game) == SCENE_TOWN, "back in town after the ending"
    now = game.read(PROGRESS, 13384)
    assert 100 in stamps_earned(game), "the 'Became the President!' stamp (number 100) is recorded"
    assert now[PRES["body_offset"]] != whole[PRES["body_offset"]], "the president's body (Q149) is given"
