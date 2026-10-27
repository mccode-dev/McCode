# The `Text2OFF` Instrument

*McXtrace: Render a text string as 3D geometry, e.g. for export to an OFF file*

## Identification

- **Site:** Templates
- **Author:** Peter Willendrup (pkwi@fysik.dtu.dk)
- **Origin:** DTU Physics
- **Date:** October 2026

## Description

```text
Draws the string given in "letters" with the Text component and nothing else -
no particles are traced. Use it with mxdisplay-cad to turn a text into CAD
geometry, for instance an OFF file that can be fed back into components that
read OFF geometry (labels, logos, test shapes):

mxdisplay-cad Text2OFF.instr -t 2 -f off letters="McXtrace" size=0.5

Notes:
- Use --trace 2 (-t 2): with the default trace level 1 Text draws thin lines,
which carry no surfaces and give an empty OFF/STL file.
- Each stroke is a cylinder and each stroke end/joint a sphere, so the
resulting surfaces overlap - fine for display, not a closed single solid.
- Letters are drawn upper-case; characters without a glyph are left blank.

Example: letters="McXtrace" size=0.5 plane="XY"
```

## Examples



## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| letters | str | Text to draw | "McXtrace" |
| size | m | Letter height | 0.5 |
| plane | str | Plane of the text: "XY", "YZ" or "XZ" | "XY" |
| thickness | 1 | Stroke diameter as fraction of size | 0.1 |
| rx | deg | Rotation of the text around the x axis | 0 |
| ry | deg | Rotation of the text around the y axis | 0 |
| rz | deg | Rotation of the text around the z axis | 0 |

## Links

- [Source code](Text2OFF.instr) for `Text2OFF.instr`.
- Text.comp, mxdisplay-cad --help

---
