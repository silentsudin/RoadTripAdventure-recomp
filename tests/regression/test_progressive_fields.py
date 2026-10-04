"""Progressive fields (Options -> Interlacing: Off): the game hook drops the half-line offset the
game puts on every other 224-line field, and the scanout shows each field as a whole picture.

The rest of the suite runs the original interlaced fields (the harness sets
RT_PROGRESSIVE_FIELDS=0); this test checks both against each other on the still title screen.
"""

from rtharness import seconds


def changed_fraction(a, b) -> float:
    assert (a.width, a.height) == (b.width, b.height)
    px = len(a.rgba) // 4
    diff = sum(1 for i in range(0, len(a.rgba), 4) if a.rgba[i:i + 3] != b.rgba[i:i + 3])
    return diff / px


def consecutive_title_frames(game_factory, name: str, progressive: bool):
    game = game_factory(name=name, env={"RT_PROGRESSIVE_FIELDS": "1" if progressive else "0"})
    game.run(seconds(12))
    game.render(True)
    game.run(8)
    first = game.frame()
    game.run(1)
    second = game.frame()
    return first, second


def test_progressive_fields_are_steady(game_factory, golden):
    a, b = consecutive_title_frames(game_factory, "fields_interlaced", progressive=False)
    interlaced = changed_fraction(a, b)
    a, b = consecutive_title_frames(game_factory, "fields_progressive", progressive=True)
    progressive = changed_fraction(a, b)
    assert interlaced > 0.05, f"the original fields should alternate ({interlaced:.3f} of pixels changed)"
    assert progressive < 0.01, f"progressive fields should not ({progressive:.3f} of pixels changed)"
    golden("title_progressive", b)


PROGRESSIVE_8X = {"RT_PROGRESSIVE_FIELDS": "1", "RT_GS_SSAA": "8"}


def test_progressive_quick_race(game_factory, golden):
    """The default picture (progressive fields at 8x: 1280x896) in a race."""
    game = game_factory(env=PROGRESSIVE_8X)
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
    frame = game.frame()
    assert (frame.width, frame.height) == (1280, 896), "8x progressive scans out 1280x896"
    golden("progressive_quick_race_10s", frame)


def test_progressive_town(game_factory, golden):
    """The default picture in Peach Town's open world (streaming, 2D/3D mix)."""
    from rtharness.adventure import FIELDS, continue_to_factory

    edits = {FIELDS["location"]["offset"]: bytes([1]), FIELDS["licence"]["offset"]: bytes([2])}
    game = game_factory(checkpoint="adventure_first_save", progress_edits=edits, env=PROGRESSIVE_8X)
    continue_to_factory(game)
    golden("progressive_factory", game.frame())
    for _ in range(4):  # Drive around town
        game.press("down")
    game.press("cross")
    game.run(seconds(2))
    game.press("cross")
    game.run(seconds(10))
    game.pad("cross")
    game.run(seconds(6))
    game.release()
    game.run(seconds(1))
    frame = game.frame()
    assert (frame.width, frame.height) == (1280, 896)
    golden("progressive_town_driven", frame)
