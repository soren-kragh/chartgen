std::cout << R"EOF(
Footnote: https://github.com/soren-kragh/chartgen
FootnotePos: Right
FootnoteLine: On

# Use negative grid padding to force abutted chart areas.
GridPadding: -1

# Place the shared legend for the side panels to the right.
SharedLegendPos: Right

ChartArea: 600 600
ChartBox: On

Heading:
  2D Sample Distribution
SubSubHeading:
  This plot has many data points, so it may be a good idea to convert it to
  bitmap before sharing or using in documents etc., e.g.:

  ⟫ chartgen -e3 | chartgen | svg2png >example3.png

HeadingPos: Left
HeadingLine: On

Axis.X.Range: -1.25 +1.25
Axis.Y.Range: -1.25 +1.25

Axis.X.Style: None
Axis.Y.Style: None

Axis.X.Pos: Bottom
Axis.Y.Pos: Left

# Show axis numbers inside the chart area.
Axis.X.NumberPos: Above
Axis.Y.NumberPos: Right

# Show plus sign for positive axis numbers.
Axis.X.NumberSign: On
Axis.Y.NumberSign: On

# Make axis numbers a little bigger.
Axis.X.NumberSize: 1.4
Axis.Y.NumberSize: 1.4

# You must select the type before defining the series.
Series.Type: Scatter
Series.New:
  Scatter plot with
  density effect

# Use a very transparent color so we get a nice density effect for the markers.
# By having both marker outline and marker fill, we get a cool effect. You may
# disable this by setting ether line- or fill-color to None.
# Google "svg colors" for a list of the standard SVG color names.
Series.MarkerSize: 15
Series.LineWidth: 5
Series.LineColor: blue -0.5 0.9
Series.FillColor: yellow 0 0.9

Series.Data:
)EOF";
