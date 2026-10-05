"""Texture dumps and packs (Options -> Texture pack; [textures] dump and pack_anisotropy).

The pack is made from the run's own dumps (nothing derived from the game is committed): every
dumped texture of 64x64 or more, at twice the size and tinted. A race with that pack must look
tinted where the original was not, and 16x anisotropic filtering must change the replaced
textures' look against trilinear.
"""

import time

import numpy as np
from PIL import Image

from rtharness import seconds


def race(game_factory, name: str, env: dict):
    game = game_factory(name=name, env={"RT_PROGRESSIVE_FIELDS": "1", **env})
    game.render(True)  # textures are read back and replaced as they are decoded
    game.run(seconds(10))
    game.press("start")
    game.run(seconds(3))
    game.press("down")
    game.run(seconds(2))
    for _ in range(8):
        game.press("cross")
        game.run(seconds(4))
    game.pad("cross")
    game.run(seconds(10))
    return game


def race_frame(game_factory, name: str, env: dict):
    game = race(game_factory, name, env)
    frame = game.frame()
    frame.image().save(game.work_dir / "race.png")  # for triage
    return frame


def blueness(frame) -> float:
    rgb = frame.array().astype(np.int16)
    return float(np.mean(rgb[:, :, 2] - (rgb[:, :, 0] + rgb[:, :, 1]) / 2))


def test_texture_pack(game_factory, tmp_path):
    dumps = tmp_path / "dumps"
    original = race_frame(game_factory, "texture_dump", {"RT_TEXTURE_DUMP": str(dumps)})
    deadline = time.time() + 10  # dumps are written by a worker thread
    while time.time() < deadline and not any(dumps.glob("*.png")):
        time.sleep(0.2)
    names = sorted(dumps.glob("*.png"))
    assert len(names) > 100, f"a race uses hundreds of textures ({len(names)} dumped)"
    # <16 hex hash>_<W>x<H>_psm<NN>.png
    for p in names:
        hash_, size, psm = p.stem.split("_")
        int(hash_, 16)
        assert len(hash_) == 16 and psm.startswith("psm")

    pack = tmp_path / "pack"
    pack.mkdir()
    made = 0
    for p in names:
        img = Image.open(p).convert("RGBA")
        if min(img.size) < 64:
            continue
        a = np.asarray(img.resize((img.width * 2, img.height * 2), Image.BILINEAR)).copy()
        a[:, :, 0] //= 3
        a[:, :, 1] //= 3
        Image.fromarray(a, "RGBA").save(pack / p.name)
        made += 1
    assert made > 10

    game = race(game_factory, "texture_pack", {"RT_TEXTURE_PACK": str(pack), "RT_ANISOTROPY": "1"})
    trilinear = game.frame()
    trilinear.image().save(game.work_dir / "race.png")
    assert blueness(trilinear) > blueness(original) + 20, \
        f"the tinted pack should show ({blueness(original):.1f} -> {blueness(trilinear):.1f})"
    stats = game._call("texture_pack")
    assert stats["pack_images"] == made and 0 < stats["replaced"] <= made, stats

    # Switched off and on again while running: the originals come back, then the pack.
    game._call("texture_pack", path="")
    game.run(seconds(1))
    off = game.frame()
    off.image().save(game.work_dir / "race_off.png")
    assert blueness(off) < blueness(original) + 5, f"the pack should be gone ({blueness(off):.1f})"
    game._call("texture_pack", path=str(pack))
    game.run(seconds(1))
    on = game.frame()
    assert blueness(on) > blueness(original) + 20, f"the pack should be back ({blueness(on):.1f})"

    aniso = race_frame(game_factory, "texture_pack_aniso", {"RT_TEXTURE_PACK": str(pack), "RT_ANISOTROPY": "16"})
    diff = np.mean(np.any(aniso.array() != trilinear.array(), axis=2))
    assert diff > 0.01, f"16x anisotropic should change glancing replaced textures ({diff:.4f} of pixels)"
