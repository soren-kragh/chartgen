std::cout << R"EOF(
# This file documents the input format for the chartgen program. This file is
# not intended to be used directly; use the simple template as a starting point
# and pick relevant specifies from this file as needed. It is recommended to let
# chartgen determine things automatically when possible.
#
# The UTF-8 character encoding standard is used.
#
# The input file consists of a number of specifiers; any line starting with a
# '#' at column 0 is ignored; empty lines are also ignored. The simplest input
# file consists only of data values, the Series.Data key is implicit in this
# special case.
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
# they should be uniformly indented. Please observe the LetterSpacing specifier
# in case wide letters are used.
#
# In the following all supported specifiers will be documented with examples.
# Many of the specifies that you should normally not include, at least not until
# you have judged the initial defaults, have been commented out below.
#
# The nature of the X-axis affects many of the properties of the chart. The
# X-axis can either be numerical or textual. A numerical X-axis has real number
# values just like the Y-axis and is thus more mathematical in nature. On the
# other hand, a textual X-axis has text strings as "values". The default is a
# numerical X-axis, but if just one series is defined which uses textual
# X-values (e.g. a Bar plot), then the X-axis as a whole becomes textual. See
# Series.Type for more information.
#
# A color can be any of the 147 named SVG colors (google "svg colors") or a
# hexadecimal RGB value of the form #rrggbb; None means no color. The optional
# 2nd value (-1.0 to 1.0) specifies by how much to lighten the color (or darken
# if negative). The third optional value specifies the transparency of the color
# (0.0 to 1.0).
#

# Margin in points; default is 0.
#Margin: 0

# Border in points; default is no border (0).
#BorderWidth: 5
#BorderColor: maroon

# Padding around everything in points; default is 8.
#Padding: 8

# The foreground color is primarily used as the default color for texts. The
# background color also determines the chart area color, unless the chart area
# color is also given (see ChartAreaColor).
#ForegroundColor: brown
#BackgroundColor: darkslategray

# See Title, which is normally what should be used for single charts. Global
# titles are placed at the top and are typically used for when multiple charts
# are organized in a grid (see New).
#GlobalTitle: Title
#GlobalSubTitle: Smaller Title
#GlobalSubSubTitle: Even Smaller Title
#GlobalTitlePos: Left
#GlobalTitleSize: 1.0
#GlobalTitleLine: On

# An optional heading for the global legends (see Series.GlobalLegend).
#GlobalLegendHeading: Metals

# Draw a frame around global legends; may be On or Off; default is Auto.
#GlobalLegendFrame: On

# Specify the position of global series legends. If Auto (the default), the
# global legends will be placed in a free chart grid location if possible, or
# otherwise below all the charts. Alternatively a grid location (with optional
# alignment) can be specified explicitly using same syntax as for New (see New).
#GlobalLegendPos: Bottom

# Set the relative size of global legend box texts.
#GlobalLegendSize: 1.0

# The background color of the global legend box.
#GlobalLegendColor: darkseagreen

# The spacing and positioning of letters are based on mono-spaced ASCII letters.
# If different letters are used, the LetterSpacing may be used to adjust the
# spacing by the given factor. The first number is the width adjustment factor,
# the second optional number is the height adjustment factor, and the third
# optional number is the baseline adjustment factor.
#LetterSpacing: 1.8 1.1 0.8

# Padding (in points) around individual charts in grid; default is 4. This
# setting only has any effect when having multiple charts. A negative value
# means that only the core chart areas are considered when placing the charts in
# a grid. An optional second number specify the padding around the core chart
# areas.
#GridPadding: 4

# Start creation of of new chart (optional). When having multiple charts, these
# are organized in a grid as specified. The first two numbers specify the row
# and column of the upper left corner, and the second optional two numbers
# specify the row and column of the lower right corner; the new chart may span
# multiple grid cells in either direction. If no grid location is given, the new
# chart is just added below any existing charts in the grid. After the optional
# grid location follows the optional horizontal/vertical alignment within the
# allocated grid cell(s); the default alignment will push charts located at a
# grid edge towards that edge.
# The chart edges of the same row/column will be aligned if possible, empty
# rows/columns can however be inserted to avoid this, for example:
# New: 0 0
# ChartArea: 200 200
# New: 0 1
# ChartArea: 200 200
# New: 1 0 1 1
# ChartArea: 600 200
# Will give:
# +----+      +----+
# |    |      |    |
# |    |      |    |
# +----+      +----+
# +----------------+
# |                |
# |                |
# +----------------+
# Whereas:
# New: 0 1
# ChartArea: 200 200
# New: 0 2
# ChartArea: 200 200
# New: 1 0 1 3
# ChartArea: 600 200
# Will give:
#   +----+  +----+
#   |    |  |    |
#   |    |  |    |
#   +----+  +----+
# +----------------+
# |                |
# |                |
# +----------------+
# When using New, most state is reset as if a new blank file is started; use
# macros to easily repeat specifiers shared among multiple charts in the grid.
#New: 0 0 1 1 Right Top

# Specifies the dimensions of the core chart area where the data is graphed.
# The values are in points and should typically be around 1000. Since SVG is
# scalable these dimensions primarily determine the relative size of text
# annotations and line thicknesses; texts will appear relatively larger if the
# core chart area is small and vice versa.
#ChartArea: 1000 600

# Draw a box around chart area; may be On or Off.
#ChartBox: On

# Change color of various chart elements.
#ChartAreaColor: dimgray
#AxisColor: white
#GridColor: green
#TextColor: lightyellow

# The background color of legend box and title box (when these are shown).
#FrameColor: aqua 0 0.5

# Titles are placed at the top of the chart.
Title: This is the title of the chart

SubTitle:
  An additional smaller title

SubSubTitle:
  An extra even smaller title

# Normally it is determined automatically whether a frame should be drawn around
# the titles; specifying TitleFrame forces the frame to be shown or not; may
# be On or Off.
#TitleFrame: On

# May be Left, Right, or Center; default is Center. The optional second argument
# may be Top or Bottom and sets the vertical alignment when inside the chart
# area (TitleInside: On); default is Top.
#TitlePos: Center Top

# Specifies if the title(s) should be placed inside the chart area; may be On or
# Off (default). Caution is advised when doing this as it may block out other
# chart elements, but for relatively small titles it can make sense.
#TitleInside: On

# Set the relative size of titles.
#TitleSize: 1.0

# Adds a footnote. Footnotes are placed below everything.
Footnote:
  https://github.com/soren-kragh/chartgen

# May be Left, Right, or Center; default is Left. Applies to the most recently
# added footnote and any additional footnotes that may follow.
FootnotePos: Right

# Enables a line above the footnotes.
#FootnoteLine: On

# Set the relative size of footnote texts.
#FootnoteSize: 1.0

# In the following Axis.Y, Axis.Y1, and Axis.PriY refers to the primary Y-axis.
# A secondary Y-axis is also possible and is identified as Axis.Y1 or Axis.SecY.
# All specifies for the primary Y-axis also apply to the secondary Y-axis.

# The orientation of the X-axis may be Horizontal or Vertical, the Y-axis
# orientation cannot be specified directly as it is always orthogonal to the
# X-axis.
Axis.X.Orientation: Horizontal

# Reverse axis direction; may be On or Off.
#Axis.X.Reverse: On
#Axis.Y.Reverse: On

# May be Auto, None, Line, Arrow, or Edge.
Axis.X.Style: Auto
Axis.Y.Style: Auto

Axis.X.Label: X-Axis
Axis.Y.Label: Y-Axis
Axis.SecY.Label: Secondary Y-Axis

Axis.X.SubLabel: smaller label
Axis.Y.SubLabel: smaller label

# Set the relative size of axis labels.
#Axis.X.LabelSize: 1.0
#Axis.Y.LabelSize: 1.0

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
Axis.Y2.LogScale: On

# Min, max, and optionally where the other orthogonal axis crosses this axis.
# Auto ranging is applied if no Range specifier is given (recommended). If the
# Axis Style is Edge, then the orthogonal axis crossing determines at which edge
# the axis is placed. If both a primary and a secondary Y-axis is used, then the
# primary Y-axis is always to the left, and the secondary Y-axis is always to
# the right. The X-axis range is ignored if the chart has textual X-values.
#Axis.X.Range: 0 100 90
#Axis.Y.Range: -5 25

# Position of axis; default is Auto. Overrides any orthogonal axis cross defined
# by the Range specifier. If both a primary and a secondary Y-axis is used, then
# the primary Y-axis is always to the left/bottom, and the secondary Y-axis is
# always to the right/top. For the X-axis, the special positions BasePri/BaseY1
# and BaseSec/BaseY2 may be used to indicate the Bar/Area base (usually zero,
# see Series.Base).
#Axis.X.Pos: Top
#Axis.Y.Pos: Right

# Define axis ticks.
# Linear scale:
#   First number is the major tick interval and the second number is an integer
#   specifying the number of minor sub-intervals per major tick (ignored for
#   textual X-axis).
# Logarithmic scale:
#   First number is the base, normally 10, and second is an integer specifying
#   the number of minor sub-intervals per division, also usually 10. The base
#   can only be a power of 10, while the minor sub-intervals can only be
#   factors of 100.
# Tick intervals are determined automatically if no Tick specifier is given
# (recommended).
#Axis.X.Tick: 10.0 4
#Axis.Y.Tick: 1.0 0

# When the X-axis is textual, often it will not be possible to show the text for
# all data points. An attempt will be made to fit the text, but at some point
# texts will have to be dropped. The TickSpacing allows user control over what
# is shown, for example, if the texts represent years, showing only every 10th
# year can be appealing. The first number is the starting position to show, and
# the second optional number is the stride after that (default stride is 1).
# When stacking charts (see New) sharing the same textual X-axis, a trick to
# not show the categories for the stacked charts, is to set the TickSpacing
# start position really high for all but the bottom chart.
#Axis.X.TickSpacing: 0 10

# Turn grid lines on/off for major and minor ticks; may be On or Off. Unless
# explicitly enabled for both Y-axes, only the grid for one of the Y-axes will
# be shown, typically the primary Y-axis. Enabling the grid for both Y-axes is
# probably a bad idea in most cases; a better approach is to adjust the Range
# for the two Y-axes such the the primary grid aligns with the secondary Y-axis.
# The minor grid is always off for textual X-axis.
#Axis.X.Grid: On On
#Axis.Y.Grid: On Off

# Grid style may be Auto, Dash, or Solid; default is Auto.
#Axis.X.GridStyle: Auto
#Axis.Y.GridStyle: Auto

# Color of the grid.
#Axis.X.GridColor: blue
#Axis.Y.GridColor: blue

# Number format may be None, Fixed, Scientific, or Magnitude. Default is Fixed
# for linear scale and Magnitude for logarithmic scale. Magnitude means showing
# e.g. "10k" instead of "10000" etc.
#Axis.X.NumberFormat: Fixed
#Axis.Y.NumberFormat: Fixed
Axis.Y2.NumberFormat: Magnitude

# Forces the sign to be shown also for positive numbers; may be On or Off.
#Axis.X.NumberSign: On
#Axis.Y.NumberSign: On

# A number unit at a short unit indication placed after each axis number. This
# can be an alternative to the Unit specifier above. Leading _ is replaced by
# space.
#Axis.X.NumberUnit: s
#Axis.Y.NumberUnit: _Volts
Axis.Y2.NumberUnit: Ω

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

# Set the relative size of axis numbers and axis unit.
#Axis.X.NumberSize: 1.0
#Axis.Y.NumberSize: 1.0

# An optional legend heading.
#LegendHeading: Types of Food

# Normally it is determined automatically whether a frame should be drawn around
# the legends; specifying LegendFrame forces the frame to be shown or not; may
# be On or Off.
#LegendFrame: On

# Legend position may be Auto, Left, Right, Top, or Bottom. Legends for the
# series are normally (Auto) placed somewhere inside the core chart area, but if
# this gets too cluttered you may place the series legends outside the core
# chart area.
#LegendPos: Bottom

# Set the relative size of legend box texts.
#LegendSize: 1.0

# Specify the relative width of bars (0.0 to 1.0). The optional second value
# (0.0 to 1.0) determines the total relative width allocated to all bars at the
# same X-value before individual widths are applied.
# For Lollipop plot, a zero for the second value can make sense, as it will
# cause all lollipops to be shown on top of each other.
# Defaults shown below.
#BarWidth: 1.0 0.85

# Defines the relative width (0.0 to 1.0) of the topmost layered bar (default is
# 0.5); applies to LayeredBar plots only. LayeredBarWidth makes the layered bars
# progressively narrower thereby making the underlying bars partially visible.
# Another strategy is to set LayeredBarWidth to 1.0 and instead make the bars
# semi transparent (see Series.FillTransparency), or a combination of
# LayeredBarWidth and transparency.
#LayeredBarWidth: 0.5

# The bar margin specifies the extra margin for the X-axis of bar graphs, and
# all textual X-axis graphs such as Line or Area. For example, if a bar graph
# has three bars and BarMargin is 1.0, then the X-axis will be laid out as if it
# has five buckets (1.0+3+1.0). The effect is that extra margin is created at
# the start/end of the bar graph (or any textual X-axis graph).
#BarMargin: 0

# Series type may be:
#   Type        X-value     Description
#-------------------------------------------------------------------------------
#   XY          Number      X/Y plot (default). Regard X-values as numbers and
#                           draw lines between data points, possibly with point
#                           markers.
#   Scatter     Number      Scatter plot. Same as XY but with no lines and
#                           always with point markers. Using a highly
#                           transparent LineColor and no FillColor can achieve a
#                           density effect.
#   Line        Text        Line plot. Regard X-values as text and draw lines
#                           between data points, possibly with point markers.
#                           Recommended for very large data sets.
#   Point       Text        Like Scatter, but regard X-values as text.
#   Lollipop    Text        Lollipop plot. Regard X-values as text and draw
#                           lines from data points to Base; default with point
#                           markers. Do not use for very large data sets.
#   Bar         Text        Bar plot. Regard X-values as text and draw bars
#                           from data points to Base (usually zero). Do not use
#                           for very large data sets.
#   StackedBar  Text        Like Bar, but stack on top of (or below if negative
#                           relative to Base) the previous bar. Do not use for
#                           very large data sets.
#   LayeredBar  Text        Like Bar, but place bars from different series on
#                           top of each other in layers; see LayeredBarWidth.
#                           Do not use for very large data sets.
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
# should normally not (*) be shown on the same chart as any other types, where
# the X-value is interpreted as a text string. This attribute applies to all
# subsequent series, or until it is redefined.
#

# (*) If you do mix, the underlying X-value on a textual X-axis is just the
# position starting from 0, so for a Bar plot with 10 bars the X-values will go
# from 0 to 9. This knowledge can be used to show XY or Scatter plots on top of
# e.g. Bar plots, but often Line or Point plots will be a better choice in this
# situation.
#
# The Series-Type specifier must be given BEFORE the Series.New to which it
# associate.
Series.Type: XY

# Each new series must start with this specifier giving the name of the
# series (may be multi-line); the type of the newly created series will be
# defined by the previously given Series.Type.
Series.New: Name of series

# The following Series specifiers associate to the above newly created series.

# Set the prune distance in points. For very large data sets, graphical details
# are removed if it is judged that they do not contribute significantly to the
# final render. While chartgen itself can handle large data sets, the subsequent
# rendering of the SVG by 3rd party tools may struggle or even crash if the SVG
# gets too big. Default is 0.3, meaning that if the pruning causes a render
# inaccuracy of less than 0.3 points, pruning can happen. This attribute applies
# to the current series and all subsequent series, or until it is redefined.
# Bar, StackedBar, and Lollipop plots cannot be pruned, but you should not use
# these types for large data sets anyway.
# The pruning algorithm is NOT a smoothing operation. Thin spikes are preserved
# and the overall shape of the series is generally preserved, while at the same
# time drastically reducing the number of SVG elements in e.g. noisy sensor data
# etc.
#Series.Prune: 0.3

# Set the series legend to be global; may be On or Off, default is Off. Global
# legends are relevant when multiple charts are organized in a grid, in which
# case the legends are shown outside the charts. Global legends may be shared,
# i.e. shown only once despite several series having that legend. For sharing to
# occur, the name and style of the series must be the same. This attribute
# applies to the current series and all subsequent series, or until it is
# redefined.
#Series.GlobalLegend: Off

# Specify if line style legends are shown with an outline around the legend text
# (On), or with a small line segment in front of the legend text (Off); default
# is On. Note that the outline is suppressed for thick lines no matter what.
# This attribute applies to the current series and all subsequent series, or
# until it is redefined.
#Series.LegendOutline: Off

# The Y-axis to use for the data values, may be Primary/Y1 or Secondary/Y2; the
# default is Primary. This attribute applies to the current series and all
# subsequent series, or until it is redefined.
Series.Axis: Primary

# Select the base for bar and area type plots; default is 0. This attribute
# applies to the current series and all subsequent series, or until it is
# redefined.
Series.Base: 0

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
# Specifying a Series.Style clears any persistent style modifiers except
# Series.MarkerShape and Series.Lighten.
#Series.Style: 4

# The point marker shape may be Circle, Square, Triangle, InvTriangle, Diamond,
# Cross, LineX, or LineY; default is Circle. LineX/LineY is a small line
# parallel with the X- or Y-axis. This persistent attribute applies to the
# current series and all subsequent series, or until it is redefined.
#Series.MarkerShape: Circle

# Alter size (diameter) of point markers. This persistent attribute applies to
# the current series and all subsequent series, or until it is redefined.
#Series.MarkerSize: 0

# This specifies the line width; it acts as a persistent modifier to the current
# Style. This attribute applies to the current series and all subsequent series,
# or until it is redefined.
#Series.LineWidth: 1

# This specifies the line dash; it acts as a persistent modifier to the current
# Style. The first number is the length of the dash and the second optional
# number of the length of the hole. This attribute applies to the current series
# and all subsequent series, or until it is redefined.
#Series.LineDash: 3 1

# This value (-1.0 to 1.0) specifies by how much the series colors should
# lightened (or darkened if negative); it acts as a persistent additive modifier
# to the current Style. This attribute applies to the current series and all
# subsequent series, or until it is redefined.
#Series.Lighten: 0

# This value (0.0 to 1.0) specifies the transparency to be used for Bar and Area
# fill color; it acts as a persistent override modifier to the current Style.
# This attribute applies to the current series and all subsequent series, or
# until it is redefined.
#Series.FillTransparency: 0

# This specifies the line color; it acts as a one-time modifier to the current
# Style. The LineColor attribute applies to the current series only.
#Series.LineColor: darkorange -0.2 0.7

# This specifies the fill color used for bars, areas, marker interiors, etc.; it
# acts as a one-time modifier to the current Style. The FillColor attribute
# applies to the current series only.
#Series.FillColor: None

# Enable tagging of data points, that is, showing the data values near the data
# points. This will quickly become cluttered, so should only be used for
# relatively simple charts. For XY and Scatter plot, both the X- and the Y-value
# are shown.
#Series.Tag: On

# Position of tag; may be Auto, Left, Right, Above, Below, or Center. Default is
# Auto. For Bar and Lollipop type plots the position may be Auto, Base, End, and
# Beyond.
#Series.TagPos: Below

# Tag size scaling factor.
#Series.TagSize: 0.8

# Show the tag in a small box; may be On or Off.
#Series.TagBox: On

# Tag text color, the fill color of the tag box (if any), and the color of the
# line around the tag box (if any); they act as persistent modifiers to the
# current Style. These attributes apply to the current series and all subsequent
# series, or until they are redefined.
#Series.TagTextColor: black
#Series.TagFillColor: lightyellow 0 0.3
#Series.TagLineColor: black

# These are the X- and Y-values for the series. If no new series was created
# beforehand, an anonymous one will be automatically created. For XY and Scatter
# type plots the X-value is a number, otherwise the X-value is regarded as a
# text string, which must be double quoted if it contains spaces. Series.Data is
# the last specifier for a given series; any Series specifies after the
# Series.Data specifier apply to the next series.
# If a dash (-) is given for a value, it means that the data point is skipped,
# if an exclamation mark (!) is given for a value, it means that the data point
# is undefined. The exclamation mark causes a break in a line plot, while a dash
# does not. This feature can be used in case a series has incomplete or missing
# data compared to the other series.
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
Series.Axis : Y1
Series.New  : Series 2
Series.Axis : Y2
Series.Style: 50
Series.LineColor: black
Series.FillColor: black 0.5
Series.MarkerSize: 10
Series.Data:
#       X-value         Series 1        Series 2
        8               22              5e3
        30              3               18e3
        80              14              2e3

# A macro is defined with MacroDef and must end with MacroEnd; the macro name
# must match. The macro is called with Macro; a macro can call other macros but
# cannot itself define a macro.
#MacroDef: MyMacro
# Lines in macro
#MacroEnd: MyMacro
#Macro: MyMacro
)EOF";
