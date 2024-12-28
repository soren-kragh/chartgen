std::cout << R"EOF(
ChartArea: 800 800

Title: Normal Distributed Samples

Footnote: https://github.com/soren-kragh/chartgen
FootnotePos: Right

Axis.X.Style: None
Axis.Y.Style: None

Axis.X.NumberFormat: None
Axis.Y.NumberFormat: None

Axis.X.Range: -1.0 +1.0 -1.0
Axis.Y.Range: -1.0 +1.0 -1.0

# An anonymous scatter plot.
Series.Type: Scatter
Series.New:

# Use a very dark but also very transparent fill color so we
# get a nice density effect for the markers. To disable the
# yellow marker outline effect, you may either set LineWidth
# to 0, or set LineColor to None.
Series.MarkerSize: 12
Series.LineWidth: 2
Series.LineColor: yellow 0 0.8
Series.FillColor: red -0.5 0.9

Series.Data:
)EOF";
