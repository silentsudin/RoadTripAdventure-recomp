"""Save states (runtime/ps2_save_state.h; socket commands save_state / load_state / state_info /
state_hash): a state loads back to the same machine, in the same process and in a fresh one, and
the game then plays exactly as it did after the save (every save-state chunk's hash per vblank,
the sound and the last picture). Damaged files, states of another game and states made by a newer
version are refused without touching the running game; a load that fails half-way puts the
machine back. States are derived data: they are written to a temp dir, never kept.
"""

from __future__ import annotations

import hashlib
import json
import math
import struct

import pytest

from rtharness import BUTTONS, seconds
from rtharness.adventure import FIELDS, continue_to_factory, scene
from rtharness.quick_race import open_course, start_race

VBLANKS = 300          # replayed and compared after each save / load
SCENE_TOWN = 2
HOST_TIMING = "EETM"   # wall-clock deadlines: the only chunk that changes when saved again later
# paraLLEl-GS's supersamples are not in a state (they are 5x local memory and follow a setting):
# a load leaves them invalid, so where the screen has been scanned out (a picture taken) the next
# frame blends against single-sampled pixels and local memory differs slightly for that frame (the
# guest's own state does not). Tests that load in the process that took pictures run at 1x.
ONE_X = {"RT_GS_SSAA": "1"}


# ---------------------------------------------------------------------------- helpers

def raw_call(game, cmd: str, **args) -> dict:
    """A socket command whose failure is an answer (the harness's _call raises on ok: false)."""
    game._file.write(json.dumps({"cmd": cmd, **args}) + "\n")
    game._file.flush()
    return json.loads(game._file.readline())


def pad_at(i: int) -> tuple[int, int]:
    """Fixed input: accelerate, steer in a slow sine, tap square now and then."""
    mask = 0xFFFF & ~BUTTONS["cross"]
    if (i // 90) % 4 == 3:
        mask &= ~BUTTONS["square"]
    return mask, int(128 + 100 * math.sin(i / 37.0))


def replay(game, n: int = VBLANKS, start: int = 0) -> list:
    """Runs n vblanks with the fixed input (from its `start`th vblank); per vblank the chunk
    hashes, then sound and picture."""
    game.audio()  # reset the sound hash
    seq = []
    for i in range(start, start + n):
        mask, lx = pad_at(i)
        game._call("pad", buttons=mask, lx=lx, ly=128, rx=128, ry=128)
        game.run(1)
        h = game._call("state_hash")
        seq.append((h["vblank"], h["hashes"]))
    sound = game.audio()
    picture = hashlib.sha1(game.frame().rgba).hexdigest()
    seq.append(("end", {"AUDIO": f'{sound["hash"]}/{sound["frames"]}', "FRAME": picture}))
    return seq


def first_divergence(a: list, b: list):
    assert len(a) == len(b)
    for (va, ha), (vb, hb) in zip(a, b):
        if va != vb:
            return f"vblank {va} vs {vb}"
        diff = sorted(k for k in ha if ha[k] != hb.get(k))
        if diff:
            return f"at vblank {va}: {', '.join(diff)}"
    return None


def save(game, path, **args) -> dict:
    r = game._call("save_state", path=str(path), max_vblanks=600, **args)
    assert r["written"], r
    return r


def load(game, path, **args) -> dict:
    r = raw_call(game, "load_state", path=str(path), max_vblanks=600, **args)
    assert r.get("ok"), r
    assert r["reserialized_equal"], f"the machine after loading differs from the state: {r}"
    return r


def race(game_factory, name: str, env: dict | None = None):
    game = game_factory(name=name, render=True, env=env)
    open_course(game, 0)
    start_race(game)
    game.pad("cross")
    game.run(seconds(8))
    return game


TOWN = 1  # Peach Town


def town_edits():
    return {FIELDS["location"]["offset"]: bytes([TOWN]), FIELDS["licence"]["offset"]: bytes([2])}


def town(game_factory, name: str):
    game = game_factory(checkpoint="adventure_first_save", name=name, render=True, progress_edits=town_edits())
    continue_to_factory(game)
    for _ in range(4):  # Change parts / Race / Save data / Quit game / Drive around town
        game.press("down")
    game.press("cross")
    game.run(seconds(2))
    game.press("cross")  # dismiss "Come again!"
    game.run(seconds(10))
    assert scene(game) == SCENE_TOWN, "driving in town"
    game.pad("cross")
    game.run(seconds(5))
    return game


# The file format (runtime/ps2_save_state.h), enough to damage or edit a header.
MAGIC = b"RTSTATE\x1a"


def state_hash(data: bytes, seed: int = 0x9E3779B97F4A7C15) -> int:
    """ps2x::stateHash (state_archive.h), for headers and small chunks."""
    m64 = (1 << 64) - 1
    mul = 0x9FB21C651E98DF25
    lanes = [seed, seed ^ 0xC2B2AE3D27D4EB4F, seed ^ 0x165667B19E3779F9, seed ^ 0x85EBCA77C2B2AE63]
    i = 0
    while i + 32 <= len(data):
        for lane in range(4):
            w = int.from_bytes(data[i + 8 * lane:i + 8 * lane + 8], "little")
            x = ((lanes[lane] ^ w) * mul) & m64
            lanes[lane] = x ^ (x >> 29)
        i += 32
    h = lanes[0] ^ ((lanes[1] * 3) & m64) ^ ((lanes[2] * 5) & m64) ^ ((lanes[3] * 7) & m64) ^ len(data)
    for b in data[i:]:
        h = ((h ^ b) * 0x100000001B3) & m64
    h ^= h >> 31
    h = (h * mul) & m64
    h ^= h >> 29
    return h


def parse_header(blob: bytes) -> dict:
    assert blob[:8] == MAGIC
    fmt, size, crc, comp = struct.unpack_from("<IIII", blob, 8)
    pos = 8 + 16 + 4 * 8  # vblank, saved, raw size, payload size
    (nmeta,) = struct.unpack_from("<I", blob, pos)
    pos += 4
    for _ in range(nmeta * 2):
        (n,) = struct.unpack_from("<I", blob, pos)
        pos += 4 + n
    (nchunks,) = struct.unpack_from("<I", blob, pos)
    pos += 4
    chunks = []
    for _ in range(nchunks):
        cid, ver = struct.unpack_from("<4sH", blob, pos)
        chunks.append({"id": cid.decode(), "version_at": pos + 4, "version": ver})
        pos += 30
    assert pos + 8 == size
    return {"format": fmt, "size": size, "crc": crc, "compression": comp, "chunks": chunks}


def rehash_header(blob: bytearray) -> bytearray:
    size = struct.unpack_from("<I", blob, 12)[0]
    struct.pack_into("<Q", blob, size - 8, state_hash(bytes(blob[:size - 8])))
    return blob


# ---------------------------------------------------------------------------- tests

def test_same_process_race(game_factory, tmp_path):
    """Save in a race, play on, load, play the same input again: the same machine every vblank."""
    game = race(game_factory, "save_states_same_process", env=ONE_X)
    path = tmp_path / "slot1.rtstate"
    saved = save(game, path)
    info = game._call("state_info", path=str(path))
    assert info["vblank"] == saved["state_vblank"] and info["meta"].get("mode") == "race", info
    assert info["file_bytes"] < info["raw_bytes"] / 3, "the state is compressed"
    after_save = replay(game)
    game.step(120, buttons=["circle"], lx=30)  # somewhere else first
    loaded = load(game, path)
    assert loaded["state_vblank"] == saved["state_vblank"]
    assert first_divergence(after_save, replay(game)) is None


@pytest.mark.parametrize("where", ["race", "town"])
def test_cross_process(game_factory, tmp_path, where):
    """Save in one process, load in a fresh one (at boot): it plays on exactly as the first did."""
    path = tmp_path / "slot1.rtstate"
    start = race if where == "race" else town
    first = start(game_factory, f"save_states_{where}_a")
    save(first, path)
    expected = replay(first)
    first.close()
    kwargs = dict(checkpoint="adventure_first_save", progress_edits=town_edits()) if where == "town" else {}
    second = game_factory(name=f"save_states_{where}_b", render=True, **kwargs)
    load(second, path)
    assert first_divergence(expected, replay(second)) is None


def test_save_load_save_is_identical(game_factory, tmp_path):
    """A state loaded and saved again at once is the same bytes, chunk by chunk (but the
    wall-clock deadlines, EETM, which count from the moment of saving)."""
    game = race(game_factory, "save_states_resave")
    first, again = tmp_path / "first.rtstate", tmp_path / "again.rtstate"
    save(game, first)
    game.run(60)
    load(game, first, resave_path=str(again))
    a = game._call("state_info", path=str(first))
    b = game._call("state_info", path=str(again))
    assert a["vblank"] == b["vblank"]
    ca = {c["name"]: c for c in a["chunks"]}
    cb = {c["name"]: c for c in b["chunks"]}
    assert ca.keys() == cb.keys()
    differ = [n for n in ca if n != HOST_TIMING and (ca[n]["hash"], ca[n]["size"]) != (cb[n]["hash"], cb[n]["size"])]
    assert not differ, f"chunks differ after save -> load -> save: {differ}"


def test_refuses_bad_files(game_factory, tmp_path):
    """Damaged files, another game's states and newer versions' are refused with a reason, and the
    running game is left alone."""
    game = race(game_factory, "save_states_refuse")
    good = tmp_path / "good.rtstate"
    save(game, good)
    stored = tmp_path / "stored.rtstate"
    save(game, stored, raw=True)
    blob = good.read_bytes()
    header = parse_header(blob)
    assert header["compression"] == 1 and header["crc"] == 0x5A49851D

    def variant(name: str, data: bytes):
        p = tmp_path / f"{name}.rtstate"
        p.write_bytes(data)
        return p

    cases = {}
    cases["garbage"] = (variant("garbage", b"not a state at all" * 100), "not a save state")
    cases["empty"] = (variant("empty", b""), "not a save state")
    cases["truncated"] = (variant("truncated", blob[:len(blob) // 2]), "cut short")
    flipped = bytearray(blob)
    flipped[header["size"] + (len(blob) - header["size"]) // 2] ^= 0x55
    cases["payload_flipped"] = (variant("payload_flipped", bytes(flipped)), "damaged")
    bad_header = bytearray(blob)
    bad_header[header["size"] - 20] ^= 1  # inside the chunk table, header hash not updated
    cases["header_flipped"] = (variant("header_flipped", bytes(bad_header)), "damaged (header)")
    other = bytearray(blob)
    struct.pack_into("<I", other, 16, 0x12345678)
    cases["other_game"] = (variant("other_game", bytes(rehash_header(other))), "another game")
    newer = bytearray(blob)
    struct.pack_into("<I", newer, 8, 99)
    cases["newer_format"] = (variant("newer_format", bytes(rehash_header(newer))), "newer version")
    # A chunk version from the future: stored payload, the version in the table and the chunk.
    sblob = bytearray(stored.read_bytes())
    sheader = parse_header(sblob)
    assert sheader["compression"] == 0
    target = next(c for c in sheader["chunks"] if c["id"] == "SPU2")
    struct.pack_into("<H", sblob, target["version_at"], 200)
    at = sheader["size"]
    while sblob[at:at + 4] != b"SPU2":  # chunks: {fourcc, u16 version, u64 size, data}
        at += 14 + struct.unpack_from("<Q", sblob, at + 6)[0]
    struct.pack_into("<H", sblob, at + 4, 200)
    cases["newer_chunk"] = (variant("newer_chunk", bytes(rehash_header(sblob))), "newer version")

    for name, (path, why) in cases.items():
        before = game.run(1)
        r = raw_call(game, "load_state", path=str(path))
        assert not r.get("ok") and r.get("refused"), f"{name}: {r}"
        assert why in r["error"], f"{name}: {r['error']!r} should say {why!r}"
        assert r["vblank"] == before, f"{name}: a refused load ran the game"
    # The stored (uncompressed) state itself is fine.
    load(game, stored)
    assert game.run(30) > 0


def test_failed_load_rolls_back(game_factory, tmp_path):
    """A load that fails after overwriting the machine puts it back: the game plays on exactly as
    if no load had been tried."""
    game = race(game_factory, "save_states_rollback", env=ONE_X)
    here, elsewhere = tmp_path / "here.rtstate", tmp_path / "elsewhere.rtstate"
    save(game, elsewhere)
    game.step(90, buttons=["circle"], lx=40)
    save(game, here)
    expected = replay(game, 120)
    load(game, here)
    # The failed load stands for the replay's first vblank (same input), the rest follows.
    mask, lx = pad_at(0)
    game._call("pad", buttons=mask, lx=lx, ly=128, rx=128, ry=128)
    r = raw_call(game, "load_state", path=str(elsewhere), test_fail=True, max_vblanks=600)
    assert not r.get("ok") and r.get("rolled_back"), r
    h = game._call("state_hash")
    assert (h["vblank"], h["hashes"]) == tuple(expected[0]), "the machine after the failed load"
    got = replay(game, 119, start=1)
    assert first_divergence(expected[1:-1], got[:-1]) is None
