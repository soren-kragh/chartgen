std::cout << R"EOF(

# Use a macro for the side panels.
MacroDef: SidePanel
ChartBox: On
Series.Type: Bar
Series.New: Samples
Series.SharedLegend: On
Series.FillColor: green
Series.LineWidth: 0
Axis.Y.Range: 0 1000
Axis.Y.NumberFormat: None
Axis.Y.Grid: Off
TitleSize: 1.2
TitleInside: On
TitleFrame: Off
TitlePos: Right Bottom
MacroEnd: SidePanel
)EOF";
