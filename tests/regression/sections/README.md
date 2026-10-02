# Play-through sections

A section is recorded input (`<name>.movie`) replayed from a starting point, plus checks. Together the
sections play through the game; each one usually ends with an in-game save, which becomes the
checkpoint (memory card) the next one starts from.

`<name>.toml`:

```toml
title = "What this section covers"
movie = "<name>.movie"
start = "boot"                  # or a checkpoint another test/section produces
produces = "<checkpoint name>"  # optional: the memory card after the section
end_vblank = 12345              # optional; defaults to the movie's "# end" line
[expect]                        # optional: progress fields (config/game_state.toml) at the end
money = 1000
```

Checks: a golden frame at every `golden` marker (F6 while recording), the exact sound of the whole
section, and the `[expect]` fields.

Record a new section with `python3 scripts/record_section.py <name> --start <checkpoint> --produces
<checkpoint>`: it opens the game at normal speed in deterministic time, starting from the checkpoint
(the game boots; Continue from slot 1 yourself). Play, press F6 where a frame should be checked, end
with an in-game save, then quit. Afterwards run `python3 scripts/regress.py -k <name> --update-goldens`
once to record its goldens. Names sort in play order (`010_...`, `020_...`).
