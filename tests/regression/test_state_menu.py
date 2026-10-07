"""The in-game menu's save states (src/states/StateSlots, src/ui/PauseMenu): four slots in
<data>/states, each with a picture of the moment, where and when; a save and a load through the
menu's own code path (socket commands state_slots / state_slot / state_slot_status, which the
menu's rows call too); the rows only in a game of the player's; refused loads say why in the
player's words. States and their pictures are derived data: they stay in the run's directory.
"""

from __future__ import annotations

import hashlib
import struct
import time

from rtharness import seconds
from test_save_states import ONE_X, first_divergence, parse_header, race, rehash_header, replay

MAX_VBLANKS = 200  # the menu lets at most 120 through while it holds the game


def slots(game) -> dict:
    return game._call("state_slots")


def status(game) -> dict:
    return game._call("state_slot_status")


def settle(game, deadline: float = 30.0) -> dict:
    """Polls a menu save/load to its end: waits (no vblanks) while the picture is taken, the file
    checked or written, and runs one vblank at a time while it waits for a savable point."""
    end = time.time() + deadline
    ran = 0
    while time.time() < end:
        s = status(game)
        if s["phase"] in ("done", "failed"):
            return s
        if s["phase"] == "waiting":
            assert ran < MAX_VBLANKS, s
            game.run(1)
            ran += 1
        else:
            time.sleep(0.02)
    raise AssertionError(f"menu state operation stuck: {status(game)}")


def replay_and_picture(game) -> list:
    """replay(), with the picture compared by its newest field (the even lines). Headless, the GS
    scans out only when a picture is grabbed, and the interlaced scanout weaves in a field from
    host-side scanout history that a state doesn't hold (here the thumbnail's grab at the save):
    after the load the odd lines repeat the newest field. Shown live, every vblank is scanned out
    and the history is right again one frame after a load."""
    seq = replay(game)
    seq[-1][1].pop("FRAME")
    frame = game.frame()
    rows = frame.array()[0::2]
    seq.append(("picture", {"FIELD": hashlib.sha1(rows.tobytes()).hexdigest()}))
    return seq


def menu(game, op: str, slot: int) -> dict:
    assert game._call("state_slot", op=op, slot=slot)["ok"]
    return settle(game)


def test_slots_save_and_load(game_factory):
    """Title: no rows. In a race: save into slot 1 (picture, place, mode, time), play on, load it
    back through the menu path: the game plays on exactly as it did after the save."""
    game = game_factory(name="state_menu_round_trip", render=True, env=ONE_X)
    game.run(seconds(10))  # boot to the title / attract
    title = slots(game)
    assert not title["available"], title
    assert all(s["empty"] for s in title["slots"]) and len(title["slots"]) == 4, title

    game.close()
    game = race(game_factory, "state_menu_round_trip_race", env=ONE_X)
    listed = slots(game)
    assert listed["available"], listed
    saved = menu(game, "save", 1)
    assert saved["phase"] == "done" and saved["saving"], saved
    assert saved["message"] == "Saved in slot 1.", saved
    after_save = replay_and_picture(game)

    listed = slots(game)["slots"]
    one = listed[0]
    assert not one["empty"] and one["readable"], one
    assert one["detail"] == "Quick Race" and one["title"] and one["title"] != "Empty", one
    assert one["when"].startswith(("Today at ", "Yesterday at ")), one  # saved moments ago (past midnight: yesterday)
    assert one["thumb_h"] == 180 and 180 <= one["thumb_w"] <= 720, one
    assert all(s["empty"] for s in listed[1:]), listed
    info = game._call("state_info", path=str(game.data_dir / "states" / "slot1.rtstate"))
    assert info["meta"]["thumbnail"].startswith("hex:89504e47"), "the picture is a PNG"
    assert info["meta"]["mode"] == "race" and info["meta"]["race_mode"] == "quick", info["meta"]

    game.step(120, buttons=["circle"], lx=30)  # somewhere else first
    loaded = menu(game, "load", 1)
    assert loaded["phase"] == "done" and loaded["message"] == "Loaded slot 1.", loaded
    assert first_divergence(after_save, replay_and_picture(game)) is None

    # Saving over a slot replaces it (the menu asked first; the operation is the same).
    again = menu(game, "save", 1)
    assert again["phase"] == "done", again
    assert len(slots(game)["slots"]) == 4


def test_refused_loads_say_why(game_factory):
    """Damaged files, another game's states and states made by an older version are refused before
    anything changes, with the loader's reason in the player's words; the game runs on."""
    game = race(game_factory, "state_menu_refuse")
    assert menu(game, "save", 1)["phase"] == "done"
    states = game.data_dir / "states"
    good = (states / "slot1.rtstate").read_bytes()
    header = parse_header(good)

    # Slot 2: damaged (a byte of the compressed payload flipped). Slot 3: another game's.
    damaged = bytearray(good)
    damaged[header["size"] + (len(good) - header["size"]) // 2] ^= 0x55
    (states / "slot2.rtstate").write_bytes(bytes(damaged))
    other = bytearray(good)
    struct.pack_into("<I", other, 16, 0x12345678)
    (states / "slot3.rtstate").write_bytes(bytes(rehash_header(other)))
    # Slot 4: made by an "older version": a stored state without the app's GAME section.
    raw = game._call("save_state", path=str(states / "raw.tmp"), raw=True, max_vblanks=600)
    assert raw["written"], raw
    blob = bytearray((states / "raw.tmp").read_bytes())
    (states / "raw.tmp").unlink()
    h = parse_header(blob)
    assert h["compression"] == 0
    at = h["size"]
    while blob[at:at + 4] != b"GAME":
        at += 14 + struct.unpack_from("<Q", blob, at + 6)[0]
    cut = 14 + struct.unpack_from("<Q", blob, at + 6)[0]
    del blob[at:at + cut]
    entry = next(c for c in h["chunks"] if c["id"] == "GAME")["version_at"] - 4
    del blob[entry:entry + 30]
    count_at = h["chunks"][0]["version_at"] - 4 - 4
    struct.pack_into("<I", blob, count_at, len(h["chunks"]) - 1)
    struct.pack_into("<I", blob, 12, h["size"] - 30)
    raw_size, payload = struct.unpack_from("<QQ", blob, 40)
    struct.pack_into("<QQ", blob, 40, raw_size - cut, payload - cut)
    (states / "slot4.rtstate").write_bytes(bytes(rehash_header(blob)))

    listed = slots(game)["slots"]
    assert all(s["readable"] for s in listed), listed  # their headers are fine
    expect = {
        2: "damaged",
        3: "another game",
        4: "older version of the app",
    }
    for slot, words in expect.items():
        before = game.run(1)
        r = menu(game, "load", slot)
        assert r["phase"] == "failed" and r["refused"], (slot, r)
        assert words in r["message"], (slot, r["message"])
        assert r["vblanks"] == 0 and game.run(1) == before + 1, f"slot {slot}: a refused load ran the game"

    # A file that isn't a state at all shows as unreadable in the list.
    (states / "slot2.rtstate").write_bytes(b"not a state" * 10)
    two = slots(game)["slots"][1]
    assert not two["empty"] and not two["readable"] and "damaged" in two["detail"], two
    # The good state still loads.
    assert menu(game, "load", 1)["phase"] == "done"
    assert game.run(30) > 0


def test_card_save_after_menu_load(game_factory):
    """A menu load marks the memory cards changed (the game reads them again at its next card
    check): saving to the card afterwards still works and holds the live state."""
    from rtharness.adventure import continue_to_factory, save_game
    from test_memcard import MONEY

    game = game_factory(checkpoint="adventure_first_save", name="state_menu_card", render=True, env={"RT_MC_TRACE": "1"})
    continue_to_factory(game)
    assert slots(game)["available"]
    assert menu(game, "save", 1)["phase"] == "done"
    game.run(seconds(1))
    assert menu(game, "load", 1)["phase"] == "done"
    game.write(MONEY, (4321).to_bytes(4, "little"))
    save_game(game, slot=1)
    assert game.saved_progress()["money"] == 4321, "the card save after a menu load holds the live state"
    game.close()  # (the log is flushed at exit)
    log = (game.work_dir / "app.log").read_text(errors="replace")
    assert "result=-1" in log, "the game saw the changed card"
