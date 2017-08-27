# Software blitter

Entry of [giuliom95](https://github.com/giuliom95) for the [GameLoop.it](gameloop.it) contest [Software blitter con alpha blending](https://forum.gameloop.it/d/355-contest-software-blitter-con-alpha-blending) (in Italian).

Three version are proposed:
* **Old** (inside `blit.old.c`). The simplest but slowest
* **New** (inside `blit.new.c`). The fastest. It is an improvement of **old**: the main improvement is to bound efficiently the two for cycles.
* **LUT** (inside `blit.lut.c`). It is an experiment and uses Look-Up Tables for conversions from RGB565 to floatRGB.

`blit_utils.c` contains tests, an exporter and importer from and to ppm, and simple benchmarking utilities. `buffer.ppm` and `sprite.ppm` are two ppm images useful for testing.