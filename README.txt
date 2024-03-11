========================================
How to build
========================================

Download the follow repositories side by
side and checkout the relevant version
tag (see CHANGELOG.txt), which is set
across all repositories.

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
format. Focus has been on simplicity and
ease of use in a fully scripted work
flow. While limited tweaking options are
provided, the automatic defaults should
normally be fine.

The generated SVG files are compatible
with a wide variety of software. You may
however also use the svg2png script to
convert to bitmap (relies on inkscape),
which is often preferred for e-mails or
if the data set is very large.

========================================
Status
========================================

Only X/Y charts are currently supported.

To come:
* Line charts.
* Bar charts.
* Stacked bar charts.
