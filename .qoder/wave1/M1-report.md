# M1 main-menu calibration

Implemented `docs/native-reference/menu-calibration.md` §4 in
`src/native/preview/ui_menu.{h,cpp}`:

- added the calibrated backdrop bands, road tone, grass field, and reordered
  backdrop composition (sky, bands, road, grass, plate, panel);
- calibrated main-panel geometry, row metrics, independently scaled text,
  selection bar, horn, plate strip, publisher, and copyright;
- added the optional `button Enter` prompt box with the documented dark icon
  stand-in.

Validation:

```text
c++ -fsyntax-only -std=c++20 -I src/native src/native/preview/ui_menu.cpp
```

passed with no output. The requested C++17 variant cannot parse existing
project declarations using `std::span`; no source error from this change was
reported before that baseline language-standard failure.
