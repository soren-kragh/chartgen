========================================
How to build
========================================

Download these repositories side by side:
https://github.com/soren-kragh/svg.git
https://github.com/soren-kragh/chart.git
https://github.com/soren-kragh/chartgen.git

> cd chartgen
> make

On linux the g++ compiler must be
installed, on Windows who knows.

========================================
About
========================================

chartgen is a simple command line tool
to generate very basic charts in SVG
format. Use the svg2png script to
convert to bitmap (relies on inkscape).
Focus has been on simplicity and ease of
use in a fully scripted work flow. While
limited tweaking options are provided,
the automatic defaults should normally
be fine.

========================================
Status
========================================

Only basic X/Y charts are currently
supported.

To come:
* Dual Y-axis support.
* Line charts.
* Bar charts.
* Stacked bar charts.
