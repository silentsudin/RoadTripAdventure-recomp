#!/usr/bin/env python3
"""Draws the app icon (our own art: a chubby toy car on a road, in the UI's palette) and writes the
Android adaptive icon layers and legacy icons into platform/android's res/, and the macOS icon
(platform/macos/RoadTrip.icns, the same picture as the legacy icon; needs macOS's iconutil).

    build/regress-venv/bin/python scripts/make_android_icon.py   (needs Pillow)

Adaptive icons are 108 dp; launchers mask them to a shape inside the middle 72 dp, and the
inner 66 dp (radius 313 on the 1024 canvas) is always shown, so the car stays inside it.
"""
from pathlib import Path

from PIL import Image, ImageDraw, ImageFilter

ROOT = Path(__file__).resolve().parent.parent
RES = ROOT / "platform/android/app/src/main/res"
S = 1024  # drawing size of the 108 dp canvas

OUTLINE = (0x1A, 0x1A, 0x30, 255)
BODY = (0xF8, 0x60, 0x18, 255)       # Theme Frame
BODY_LIGHT = (0xFF, 0x9A, 0x50, 255)
GLASS = (0x8E, 0xD4, 0xF0, 255)      # Theme ListBevelLight
GOLD = (0xF8, 0xC8, 0x38, 255)       # Theme BarCapGold
SILVER = (0xE8, 0xE8, 0xE8, 255)
SKY_TOP = (0x2C, 0x6C, 0xE8)         # Theme BarTop
SKY_LOW = (0x8E, 0xD4, 0xF0)
HILL = (0x48, 0xA8, 0x38)
ROAD = (0x5C, 0x64, 0x80)
CLEAR = (0, 0, 0, 0)


def background() -> Image.Image:
    img = Image.new("RGBA", (S, S))
    d = ImageDraw.Draw(img)
    for y in range(S):  # sky
        t = min(1.0, y / (S * 0.62))
        d.line([(0, y), (S, y)], fill=tuple(int(a + (b - a) * t) for a, b in zip(SKY_TOP, SKY_LOW)) + (255,))
    d.ellipse([-S * 0.3, S * 0.52, S * 1.3, S * 1.1], fill=HILL + (255,))
    d.rectangle([0, S * 0.72, S, S], fill=ROAD + (255,))  # road
    for x in range(-40, S, 200):
        d.rounded_rectangle([x, S * 0.80, x + 110, S * 0.80 + 26], radius=12, fill=GOLD)
    return img


def car(silhouette: bool = False) -> Image.Image:
    """The car, facing right: super-deformed (a big bubble cabin, about as tall as it is long)."""
    img = Image.new("RGBA", (S, S), CLEAR)
    d = ImageDraw.Draw(img)
    ink = (255, 255, 255, 255)
    o = 26  # outline width

    def shape(fn, box, fill, **kw):
        fn([box[0] - o, box[1] - o, box[2] + o, box[3] + o], fill=ink if silhouette else OUTLINE, **kw)
        if not silhouette:
            fn(box, fill=fill, **kw)

    cabin = [320, 270, 700, 560]
    body = [250, 460, 780, 690]
    wheels, r = (360, 670), 118
    windows = ([395, 317, 495, 458], [525, 317, 635, 458])
    if silhouette:
        for cx in wheels:  # a gap between the tyres and the body
            g = r + o + 14
            d.ellipse([cx - g, 690 - g, cx + g, 690 + g], fill=CLEAR)
    shape(d.rounded_rectangle, cabin, BODY, radius=170)
    shape(d.rounded_rectangle, body, BODY, radius=110)
    if silhouette:
        for w in windows:
            d.rounded_rectangle(w, radius=40, fill=CLEAR)
        for cx in wheels:
            g = r + o + 14
            d.ellipse([cx - g, 690 - g, cx + g, 690 + g], fill=CLEAR)
    else:
        d.rectangle([cabin[0], 520, cabin[2], 560], fill=BODY)  # the seam between cabin and body
        d.rounded_rectangle([body[0] + 40, body[1] + 18, body[2] - 40, body[1] + 48], radius=15, fill=BODY_LIGHT)
        for w in windows:
            d.rounded_rectangle([w[0] - 12, w[1] - 12, w[2] + 12, w[3] + 12], radius=48, fill=OUTLINE)
            d.rounded_rectangle(w, radius=36, fill=GLASS)
        d.rounded_rectangle([418, 335, 440, 395], radius=11, fill=(255, 255, 255, 230))  # glare
        d.ellipse([720, 500, 800, 580], fill=OUTLINE)  # headlight
        d.ellipse([732, 512, 788, 568], fill=GOLD)
    for cx in wheels:
        shape(d.ellipse, [cx - r, 690 - r, cx + r, 690 + r], OUTLINE)
        if silhouette:
            d.ellipse([cx - 52, 690 - 52, cx + 52, 690 + 52], fill=CLEAR)
            d.ellipse([cx - 20, 690 - 20, cx + 20, 690 + 20], fill=ink)
        else:
            d.ellipse([cx - 52, 690 - 52, cx + 52, 690 + 52], fill=SILVER)
            d.ellipse([cx - 20, 690 - 20, cx + 20, 690 + 20], fill=OUTLINE)
    if not silhouette:
        shadow = Image.new("RGBA", (S, S), CLEAR)  # where it stands on the road
        ImageDraw.Draw(shadow).ellipse([230, 780, 800, 840], fill=(0, 0, 0, 120))
        img = Image.alpha_composite(shadow.filter(ImageFilter.GaussianBlur(12)), img)
    # Inside the always-shown circle: 85% about the car's middle.
    k = 0.85
    small = img.resize((int(S * k), int(S * k)), Image.Resampling.LANCZOS)
    out = Image.new("RGBA", (S, S), CLEAR)
    cx, cy = 512, 530
    out.paste(small, (int(cx - cx * k), int(cy - cy * k)))
    return out


def save(img: Image.Image, folder: str, name: str, px: int):
    out = RES / folder
    out.mkdir(parents=True, exist_ok=True)
    img.resize((px, px), Image.Resampling.LANCZOS).save(out / name)


def mac_icon(full: Image.Image):
    """macOS: the legacy icon's picture (the visible 72 dp) as an 824 px rounded square on a
    1024 canvas with a soft shadow (Apple's icon grid), at every .iconset size."""
    import shutil
    import subprocess
    import tempfile
    crop = full.crop((int(S * 18 / 108), int(S * 18 / 108), int(S * 90 / 108), int(S * 90 / 108)))
    side, inset = 824, 100
    tile = crop.resize((side, side), Image.Resampling.LANCZOS)
    mask = Image.new("L", (side, side), 0)
    ImageDraw.Draw(mask).rounded_rectangle([0, 0, side - 1, side - 1], radius=185, fill=255)
    tile.putalpha(mask)
    canvas = Image.new("RGBA", (1024, 1024), CLEAR)
    shadow = Image.new("RGBA", (1024, 1024), CLEAR)
    ImageDraw.Draw(shadow).rounded_rectangle([inset, inset + 12, inset + side, inset + side + 12], radius=185,
                                             fill=(0, 0, 0, 90))
    canvas = Image.alpha_composite(canvas, shadow.filter(ImageFilter.GaussianBlur(14)))
    canvas.alpha_composite(tile, (inset, inset))
    out = ROOT / "platform/macos"
    canvas.save(out / "icon_1024.png")
    if not shutil.which("iconutil"):
        print("iconutil not found: wrote", out / "icon_1024.png", "only")
        return
    with tempfile.TemporaryDirectory() as tmp:
        iconset = Path(tmp) / "RoadTrip.iconset"
        iconset.mkdir()
        for px in (16, 32, 128, 256, 512):
            canvas.resize((px, px), Image.Resampling.LANCZOS).save(iconset / f"icon_{px}x{px}.png")
            canvas.resize((px * 2, px * 2), Image.Resampling.LANCZOS).save(iconset / f"icon_{px}x{px}@2x.png")
        subprocess.run(["iconutil", "-c", "icns", str(iconset), "-o", str(out / "RoadTrip.icns")], check=True)
    print("wrote", out / "RoadTrip.icns")


def main():
    bg, fg, mono = background(), car(), car(silhouette=True)
    # Adaptive layers: 108 dp per density.
    for folder, px in (("mipmap-mdpi", 108), ("mipmap-hdpi", 162), ("mipmap-xhdpi", 216), ("mipmap-xxhdpi", 324),
                       ("mipmap-xxxhdpi", 432)):
        save(bg, folder, "ic_launcher_background.png", px)
        save(fg, folder, "ic_launcher_foreground.png", px)
        save(mono, folder, "ic_launcher_monochrome.png", px)
    # Legacy icons (48 dp): the visible 72 dp of the 108 dp canvas, in a rounded square.
    full = Image.alpha_composite(bg, fg)
    crop = full.crop((int(S * 18 / 108), int(S * 18 / 108), int(S * 90 / 108), int(S * 90 / 108)))
    mask = Image.new("L", crop.size, 0)
    ImageDraw.Draw(mask).rounded_rectangle([0, 0, crop.size[0] - 1, crop.size[1] - 1], radius=crop.size[0] // 5, fill=255)
    crop.putalpha(mask)
    for folder, px in (("mipmap-mdpi", 48), ("mipmap-hdpi", 72), ("mipmap-xhdpi", 96), ("mipmap-xxhdpi", 144),
                       ("mipmap-xxxhdpi", 192)):
        save(crop, folder, "ic_launcher.png", px)
    mac_icon(full)
    anydpi = RES / "mipmap-anydpi-v26"
    anydpi.mkdir(parents=True, exist_ok=True)
    (anydpi / "ic_launcher.xml").write_text(
        '<?xml version="1.0" encoding="utf-8"?>\n'
        '<!-- Drawn by scripts/make_android_icon.py. -->\n'
        '<adaptive-icon xmlns:android="http://schemas.android.com/apk/res/android">\n'
        '    <background android:drawable="@mipmap/ic_launcher_background" />\n'
        '    <foreground android:drawable="@mipmap/ic_launcher_foreground" />\n'
        '    <monochrome android:drawable="@mipmap/ic_launcher_monochrome" />\n'
        '</adaptive-icon>\n')
    # Previews: as a circle launcher shows it (radius 341 of 1024), and the themed silhouette.
    preview = ROOT / "build/scratch"
    preview.mkdir(parents=True, exist_ok=True)
    circle = full.copy()
    m = Image.new("L", (S, S), 0)
    ImageDraw.Draw(m).ellipse([512 - 341, 512 - 341, 512 + 341, 512 + 341], fill=255)
    circle.putalpha(m)
    circle.crop((512 - 341, 512 - 341, 512 + 341, 512 + 341)).resize((256, 256), Image.Resampling.LANCZOS).save(preview / "icon_circle.png")
    themed = Image.new("RGBA", (S, S), (0x30, 0x40, 0x60, 255))
    themed.alpha_composite(mono)
    themed.putalpha(m)
    themed.crop((512 - 341, 512 - 341, 512 + 341, 512 + 341)).resize((256, 256), Image.Resampling.LANCZOS).save(preview / "icon_themed.png")
    print("wrote", RES)


if __name__ == "__main__":
    main()
