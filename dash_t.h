std::cout << R"EOF(
# Simple template

Title       : Chart Title
Axis.X.Label: X-Axis Label
Axis.Y.Label: Y-Axis Label

Series.Type : XY
Series.New  : Name of series
Series.Data :
        0       23.7
        7.0     2.3
        40      20
        47      10.0
        71      4.3
        97      14

# Summary of all available specifiers (see -T template for details):
# FrameWidth: 5
# FrameColor: black
# Margin: 5
# ChartArea: 1000 600
# ChartBox: On
# BackgroundColor: darkslategray
# ChartAreaColor: dimgray
# AxisColor: white
# GridColor: green
# TextColor: lightyellow
# LetterSpacing: 1.8 1.1 0.8
# Title:
# SubTitle:
# SubSubTitle:
# TitlePos: Left
# TitleInside: On
# TitleSize: 1.0
# Footnote:
# FootnotePos: Center
# FootnoteLine: On
# FootnoteSize: 1.0
# Axis.X.Orientation: Vertical
# Axis.*.Reverse: Off
# Axis.*.Style: Auto
# Axis.*.Label:
# Axis.*.SubLabel:
# Axis.*.LabelSize: 1.0
# Axis.*.Unit:
# Axis.*.UnitPos: Above
# Axis.*.LogScale: On
# Axis.*.Range: 0 100 90
# Axis.*.Pos: Top
# Axis.*.Tick: 10.0 4
# Axis.*.Grid: Off On
# Axis.*.GridStyle: Auto
# Axis.*.GridColor: blue
# Axis.*.NumberFormat: Magnitude
# Axis.*.NumberSign: On
# Axis.*.NumberUnit: s
# Axis.*.MinorNumber: On
# Axis.*.NumberPos: Auto
# Axis.*.NumberSize: 1.0
# LegendHeading: Countries
# LegendFrame: On
# LegendPos: Below
# LegendColor: aqua 0 0.5
# LegendOutline: Off
# LegendSize: 1.0
# BarWidth: 0.8 0.7
# BarMargin: 0
# Series.Type: XY
# Series.New: Name of series
# Series.AxisY: Secondary
# Series.Base: 0
# Series.Style: 32
# Series.MarkerShape: Circle
# Series.MarkerSize: 8
# Series.LineWidth: 1
# Series.LineDash: 3 1
# Series.Lighten: -0.3
# Series.FillTransparency: 0.8
# Series.LineColor: black
# Series.FillColor: None
# Series.Tag: On
# Series.TagPos: Below
# Series.TagSize: 0.8
# Series.TagBox: On
# Series.TagTextColor: black
# Series.TagFillColor: lightyellow 0 0.3
# Series.TagLineColor: black
# Series.Data:
)EOF";
