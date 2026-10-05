"""HD texture packs: other palettes of a replaced texture, and the pack memory budget.

The packs are made from the run's own dumps in a temp dir (nothing derived from the game is
committed).
"""

import re

import numpy as np
from PIL import Image

from test_texture_pack import blueness, race, race_frame


def tinted(src, dst, scale=2):
    img = Image.open(src).convert("RGBA")
    a = np.asarray(img.resize((img.width * scale, img.height * scale), Image.BILINEAR)).copy()
    a[:, :, 0] //= 3
    a[:, :, 1] //= 3
    Image.fromarray(a, "RGBA").save(dst)


def index_pattern(path):
    """Group key for palettes of one texture, as the runtime matches them: size and the image of
    colour classes numbered by first appearance."""
    a = np.asarray(Image.open(path).convert("RGBA")).reshape(-1, 4).astype(np.int64)
    packed = a[:, 0] | (a[:, 1] << 8) | (a[:, 2] << 16) | (a[:, 3] << 24)
    _, first, inverse = np.unique(packed, return_index=True, return_inverse=True)
    order = np.argsort(np.argsort(first))  # class number by first appearance
    return Image.open(path).size, order[inverse].tobytes()


def test_recolor(game_factory, tmp_path):
    """Other palettes of a replaced texture (fades, flashes) are drawn with its image, recoloured by
    an affine map fitted between the two palettes."""
    dumps = tmp_path / "dumps"
    plain = race_frame(game_factory, "hd_dump", {"RT_TEXTURE_DUMP": str(dumps)})

    # One palette of each paletted texture (the first drawn), and every other texture: the other
    # palettes must be fitted.
    seen, chosen = set(), []
    for p in sorted(dumps.glob("*.png"), key=lambda p: p.stat().st_mtime):
        if "_psm13" in p.name or "_psm14" in p.name:
            key = index_pattern(p)
            if key in seen:
                continue
            seen.add(key)
        chosen.append(p)

    def pack_of(name, transform):
        pack = tmp_path / name
        pack.mkdir()
        for p in chosen:
            img = Image.open(p).convert("RGBA")
            transform(img.resize((img.width * 2, img.height * 2), Image.BILINEAR)).save(pack / p.name)
        return str(pack)

    # A plain upscale keeps the game's colours, recoloured palettes included; the race has
    # palette variants that are fitted and bound.
    game = race(game_factory, "hd_upscaled", {"RT_TEXTURE_PACK": pack_of("upscaled", lambda im: im)})
    upscaled = game.frame()
    err = float(np.mean(np.abs(upscaled.array().astype(int) - plain.array().astype(int))))
    assert err < 4.0, f"the upscaled pack should keep the game's colours (mean error {err:.2f})"
    log = (game.work_dir / "app.log").read_text(errors="replace")
    fitted = [(int(a), int(b)) for a, b in re.findall(r"(\d+)/(\d+) fitted", log)]
    assert fitted and fitted[-1][0] > 0, f"palette variants should be fitted ({fitted[-1:] or 'none'})"

    # The recolour reaches the shader: with the test hook every match is drawn red/blue swapped.
    swapped = race_frame(game_factory, "hd_swap", {"RT_TEXTURE_PACK": pack_of("tinted", lambda im: im),
                                                   "RT_TEXTURE_RECOLOR_TEST": "swap"})
    assert blueness(swapped) < blueness(plain) - 10, \
        f"swapping red and blue should show ({blueness(plain):.1f} -> {blueness(swapped):.1f})"


def test_pack_memory_budget(game_factory, tmp_path):
    """Over a tiny budget, images are evicted and reloaded; the pack still shows."""
    dumps = tmp_path / "dumps"
    original = race_frame(game_factory, "budget_dump", {"RT_TEXTURE_DUMP": str(dumps)})
    pack = tmp_path / "pack"
    pack.mkdir()
    for p in dumps.glob("*.png"):
        if min(Image.open(p).size) >= 32:
            tinted(p, pack / p.name, scale=4)

    game = race(game_factory, "budget_pack", {"RT_TEXTURE_PACK": str(pack), "RT_TEXTURE_PACK_BUDGET_MB": "16"})
    frame = game.frame()
    assert blueness(frame) > blueness(original) + 20, \
        f"the pack should show under a small budget ({blueness(original):.1f} -> {blueness(frame):.1f})"
    log = (game.work_dir / "app.log").read_text(errors="replace")
    evicted = [int(m) for m in re.findall(r"\[textures\] pack: .*?, (\d+) evicted", log)]
    assert evicted and max(evicted) > 0, "a 16 MB budget should evict pack images"
