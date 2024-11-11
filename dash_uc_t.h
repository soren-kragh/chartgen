std::cout << R"EOF(
# This file documents the input format for the chartgen program. This file is
# not intended to be used directly; use the simple template as a starting point
# and pick relevant specifies from this file as needed. It is strongly
# recommended to let chartgen determine things automatically when possible.
#
# The input file consists of a number of specifiers; any line starting with a
# '#' at column 0 is ignored; empty lines are also ignored. The simplest input
# file consists only of the Series.Data specifier.
#
# A specifier takes the form:
# KEY: VALUE(s)
# or
# KEY:
#   VALUE(s)
#   VALUE(s)
#   ...
#
# The KEY must be unindented and any VALUE(s) on the following lines must be
# indented, preferably by the same amount (important for text VALUEs). Multiple
# VALUEs on the same line must be whitespace separated.
#
# Text VALUEs such as titles and labels may span multiple lines, in which case
# they must be uniformly indented.
#
# In the following all supported specifiers will be documented with examples.
# Many of the specifies you should normally not include, at least not until you
# have judged the initial defaults, have been commented out below.
#

# Specifies the dimensions of the core chart area where the data is graphed.
# The values are in points and should typically be around 1000. Since SVG is
# scalable these dimensions primarily determine the relative size of text
# annotations and line thicknesses; texts will appear relatively larger if the
# core chart area is small and vice versa.
#ChartArea: 1000 700

# Margin around chart in points; default is 5.
#Margin: 5

# Titles are placed at the top of the chart.
Title: This is the title of the chart

SubTitle:
  An additional smaller title

SubSubTitle:
  An extra even smaller title

# Footnotes are placed at the bottom of the chart.
Footnote:
  https://github.com/soren-kragh/chartgen

# May be Left, Right, or center; default is Left.
FootnotePos: Right

# In the following Axis.Y is an alias for the primary Y-axis, which is also
# identified as Axis.PriY. A secondary Y-axis is also possible and is identified
# as Axis.SecY. All specifies for the primary Y-axis also apply to the secondary
# Y-axis.

# The orientation of the X-axis may be Horizontal or Vertical, the Y-axis
# orientation cannot be specified directly as it is always orthogonal to the
# X-axis.
Axis.X.Orientation: Horizontal

# Reverse axis direction; may be On or Off.
#Axis.X.Reverse: On
#Axis.Y.Reverse: On

# May be Auto, None, Arrow, or Edge.
Axis.X.Style: Auto
Axis.Y.Style: Auto

Axis.X.Label: X-Axis
Axis.Y.Label: Y-Axis
Axis.SecY.Label: Secondary Y-Axis

Axis.X.SubLabel: smaller label
Axis.Y.SubLabel: smaller label

# Especially for linear scale, it is often a good idea to select units and scale
# the data accordingly to avoid very small or very large numbers in the graph.
# It is a cardinal sin to not have units on your axes, but the Axis Label or
# Axis NumberUnit can also serve that purpose (especially if the Axis Style is
# not Arrow).
Axis.X.Unit: Mb/s
Axis.Y.Unit:
  micro
  seconds

# Position of unit text; may be Auto, Left, Right, Top/Above, or
# Bottom/Below. Not all may apply; default is Auto (recommended).
#Axis.X.UnitPos: Auto
#Axis.Y.UnitPos: Auto

# Turn logarithmic scale on/off; may be On or Off.
#Axis.X.LogScale: On
#Axis.Y.LogScale: On
Axis.SecY.LogScale: On

# Min, max, and optionally where the other orthogonal axis crosses this axis.
# Auto ranging is applied if no Range specifier is given (recommended). If the
# Axis Style is Edge, then the orthogonal axis crossing determines at which edge
# the axis is placed. If both a primary and a secondary Y-axis is used, then the
# primary Y-axis is always to the left, and the secondary Y-axis is always to
# the right. The X-axis range is ignored if the chart has text based X-values.
#Axis.X.Range: 0 100 90
#Axis.Y.Range: -5 25

# May be Auto/Top/Bottom for the X-axis, and Auto/Left/Right for the Y-axis;
# default is Auto. Overrides any orthogonal axis cross defined by the Range
# specifier. If both a primary and a secondary Y-axis is used, then the primary
# Y-axis is always to the left, and the secondary Y-axis is always to the right.
#Axis.X.Pos: Top
#Axis.Y.Pos: Right

# Define axis ticks.
# Linear scale:
#   First number is the major tick interval and the second number is an integer
#   specifying the number of minor sub-intervals per major tick.
# Logarithmic scale:
#   First number is the base, normally 10, and second is an integer specifying
#   the number of minor sub-intervals per division, also usually 10. The base
#   can only be a power of 10, while the minor sub-intervals can only be
#   factors of 100.
# Tick intervals are determined automatically if no Tick specifier is given
# (recommended).
#Axis.X.Tick: 10.0 4
#Axis.Y.Tick: 1.0 0

# Turn grid lines on/off for major and minor ticks; may be On or Off. Unless
# explicitly enabled for both Y-axes, only the grid for one of the Y-axes will
# be shown, typically the primary Y-axis. Enabling the grid for both Y-axes is
# probably a bad idea in most cases; a better approach is to adjust the Range
# for the two Y-axes such the the primary grid aligns with the secondary Y-axis.
#Axis.X.Grid: On On
#Axis.Y.Grid: On Off

# Grid style may be Auto, Dash, or Solid; default is Auto.
#Axis.X.GridStyle: Auto
#Axis.Y.GridStyle: Auto

# Number format may be None, Fixed, Scientific, or Magnitude. Default is Fixed
# for linear scale and Magnitude for logarithmic scale. Magnitude means showing
# e.g. "10k" instead of "10000" etc.
#Axis.X.NumberFormat: Fixed
#Axis.Y.NumberFormat: Fixed
Axis.SecY.NumberFormat: Magnitude

# A number unit at a short unit indication placed after each axis number. This
# can be an alternative to the Unit specifier above. Leading _ is replaced by
# space.
#Axis.X.NumberUnit: s
#Axis.Y.NumberUnit: _Volts

# Specify if numbers are shown for minor ticks, default for linear scale is
# Off, while default for logarithmic scale is On; may be On or Off.
#Axis.X.MinorNumber: On
#Axis.Y.MinorNumber: Off

# Position of axis numbers; may be Auto, Left, Right, Top/Above, or
# Bottom/Below. Not all may apply; default is Auto (recommended). Very large or
# very small numbers will be hard to read, so it is a very good idea to scale
# the data and use suitable units.
#Axis.X.NumberPos: Auto
#Axis.Y.NumberPos: Auto

# Legend position may be Auto, Left, Right, or Below. Legends for the series
# are normally (Auto) placed somewhere inside the core chart area, but if this
# gets too cluttered you may place the series legends outside the core chart
# area.
#LegendPos: Below

# Series type may be:
#   Type        X-value     Description
#-------------------------------------------------------------------------------
#   XY          Number      X/Y plot (default). Regard X-values as numbers and
#                           draw lines between data points, possibly with point
#                           markers.
#   Scatter     Number      Scatter plot. Same as XY but with no lines and
#                           always with point markers. Using a highly
#                           transparent LineColor and no FillColor can achieve a
#                           density effect for large data sets.
#   Line        Text        Line plot. Regard X-values as text and draw lines
#                           between data points, possibly with point markers.
#   Point       Text        Like Scatter, but regard X-values as text.
#   Lollipop    Text        Lollipop plot. Regard X-values as text and draw
#                           lines from data points to Base; default with point
#                           markers.
#   Bar         Text        Bar plot. Regard X-values as text and draw bars
#                           from data points to Base (usually zero).
#   StackedBar  Text        Like Bar, but stack on top of (or below if negative
#                           relative to Base) the previous bar.
#   Area        Text        Area plot. Regard X-values as text and draw an area
#                           polygon between data points and the Base line.
#                           Optionally also draw a line between data points,
#                           possibly with point markers.
#   StackedArea Text        Like Area, but stack on top of (or below if negative
#                           relative to Base) the previous StackedArea. Note
#                           that negative values (relative to Base) are stacked
#                           separately, so mixing negative with positive in the
#                           same series will likely look weird.
#-------------------------------------------------------------------------------
#
# Since the X-values are true numbers for XY and Scatter types, these types
# should not (*) be shown on the same chart as any other types, where the
# X-value is interpreted as a text string. This attribute applies to all
# subsequent series, or until it is redefined.
#
# (*) If you absolutely must mix, the underlying X-value on a text based X-axis
# is just the position starting from 0, so for a Bar plot with 10 bars the
# X-values will go from 0 to 9. This knowledge can be used to show XY or Scatter
# plots on top of e.g. Bar plots, but often Line or Point plots will be a better
# choice in this situation.
Series.Type: XY

# Each new series should start with this specifier giving the name of the
# series (may be multi-line). The following Series specifiers associate to
# this.
Series.New: Name of series

# Associated Y-axis may be Primary or Secondary; the default is Primary. This
# attribute applies to the current series and all subsequent series, or until it
# is redefined.
Series.AxisY: Primary

# Select the base for bar and area type plots; default is 0. This attribute
# applies to the current series and all subsequent series, or until it is
# redefined.
Series.Base: 0

# Alter size (diameter) of point markers. This attribute applies to the current
# series and all subsequent series, or until it is redefined.
#Series.MarkerSize: 0

# The point marker shape may be Circle, Square, Triangle, or Diamond; default is
# Circle. This attribute applies to the current series and all subsequent
# series, or until it is redefined.
#Series.MarkerShape: Circle

# The style of the graph. The style is a number in the range from 0 to 79; if no
# Style specifier is given, it is assigned an incrementing number based on the
# last given Series.Style.
#  0 to  9: Solid line using 10 different colors
# 10 to 19: Same as 0 to 9 but with short dashed line
# 20 to 29: Same as 0 to 9 but with medium dashed line
# 30 to 39: Same as 0 to 9 but with long dashed line
# 40 to 79: Same as 0 to 39, but with thinner line
# Note that the 10 default colors have been chosen to be easier to discern for
# people with the most common form of color blindness (red-green).
# Specifying a Series.Style clears any persistent style modifiers.
#Series.Style: 4

# This specifies the line width; it acts as a persistent modifier to the current
# Style. This attribute applies to the current series and all subsequent series,
# or until it is redefined.
#Series.LineWidth: 1

# This specifies the line dash; it acts as a persistent modifier to the current
# Style. The first number is the length of the dash and the second optional
# number of the length of the hole. This attribute applies to the current series
# and all subsequent series, or until it is redefined.
#Series.LineDash: 3 1

# This specifies the line color; it acts as a one-time modifier to the current
# Style. A color can be any of the 147 named SVG color codes or an hexadecimal
# RGB value of the form #rrggbb; None means no color. The optional 2nd value
# (range [-1.0;1.0]) specifies by how much to lighten the color (or darken if
# negative). The third optional value specifies the transparency of the color
# (0.0 to 1.0). Note however that transparency may not be supported by all SVG
# image viewers and manipulation tools. The LineColor attribute applies to the
# current series only.
#Series.LineColor: darkorange -0.2 0.7

# This specifies the fill color used for e.g. bars, areas, marker interiors,
# etc.; it acts as a one-time modifier to the current Style. Same format as
# LineColor. The FillColor attribute applies to the current series only.
#Series.FillColor: None

# These are the X- and Y-values for the series. If no new series was created
# beforehand, an anonymous one will be automatically created. For XY and Scatter
# type plots the X-value is a number, otherwise the X-value is regarded as a
# text string, which must be double quoted if it contains spaces. Series.Data is
# the last specifier for a given series; any Series specifies after the
# Series.Data specifier apply to the next series.
Series.Data:
        0       23.7
        7.0     2.3
        23      7e-2
        47      10.0
        71      4.3
        97      14

# Several series sharing the same X-values can be specified in one go; series
# using a text string as the X-value (everything but XY and Scatter types)
# should use this way of specifying the series data. Use double-quotes if the
# text string contains spaces. If not enough new series were created beforehand,
# anonymous ones will be automatically created as needed.
Series.New  : Series 1
Series.AxisY: Primary
Series.New  : Series 2
Series.AxisY: Secondary
Series.Style: 54
Series.MarkerSize: 10
Series.Data:
#       X-value         Series 1        Series 2
        8               22              5e3
        30              3               18e3
        80              14              2e3
)EOF";