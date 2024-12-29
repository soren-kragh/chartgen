std::cout << R"EOF(
Footnote: https://github.com/soren-kragh/chartgen
FootnotePos: Right

ChartArea: 700 700

Title:
  Normal Distributed Samples
SubTitle:
  2D polar plot
SubSubTitle:
  This plot has many data points, so it is a very good idea to convert it
  to bitmap before sharing or using the picture in documents etc., e.g.:

  chartgen -e3 | chartgen | svg2png >example3.png

Axis.X.Style: None
Axis.Y.Style: None

Axis.X.Pos: Bottom
Axis.Y.Pos: Left

# Show axis numbers inside the chart area.
Axis.X.NumberPos: Above
Axis.Y.NumberPos: Right

Axis.X.Range: -1.1 +1.1
Axis.Y.Range: -1.1 +1.1

# You must select the type before defining the series.
Series.Type: Scatter
Series.New:
  Scatter plot width
  transparency effect

# Use a very transparent color so we get a nice density effect for the markers.
# By having both marker outline and marker fill, we get a cool effect. You may
# disable this by setting ether line- or fill-color to None.
# Google "svg colors" for a list of the standard SVG color names.
Series.MarkerSize: 16
Series.LineWidth: 4
Series.LineColor: blue -0.5 0.9
Series.FillColor: yellow 0 0.9

Series.Data:
)EOF";
