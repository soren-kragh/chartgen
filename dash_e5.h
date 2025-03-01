std::cout << R"EOF(
Footnote: https://github.com/soren-kragh/chartgen
FootnotePos: Right

Title: StackedBar and Lollipop
TitleFrame: On

LegendPos: Above

Axis.X.Orientation: Vertical
Axis.X.Pos: Base
Axis.X.Style: Line

Axis.Y.NumberFormat: Scientific
Axis.Y.Pos: Top
Axis.Y.Style: Edge
Axis.Y.GridStyle: Solid

Series.Type: StackedBar

Series.LineWidth: 0.5

Series.New: Positive A
Series.LineColor: white
Series.FillColor: blue 0.0

Series.New: Positive B
Series.LineColor: white
Series.FillColor: blue 0.3

Series.New: Positive C
Series.LineColor: white
Series.FillColor: blue 0.6

Series.New: Negative A
Series.LineColor: white
Series.FillColor: red 0.0

Series.New: Negative B
Series.LineColor: white
Series.FillColor: red 0.3

Series.New: Negative C
Series.LineColor: white
Series.FillColor: red 0.6

Series.Type: Lollipop
Series.New: Sum
Series.LineColor: black
Series.LineWidth: 5
Series.MarkerShape: LineX
Series.MarkerSize: 15

Series.Tag: On
Series.TagBox: On
Series.TagSize: 1
Series.TagPos: Beyond
Series.TagFillColor: black
Series.TagLineColor: white
Series.TagTextColor: white

Series.Data:
)EOF";
