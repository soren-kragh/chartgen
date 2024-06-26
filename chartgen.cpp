//
//  MIT No Attribution License
//
//  Copyright 2024, Soren Kragh
//
//  Permission is hereby granted, free of charge, to any person obtaining a
//  copy of this software and associated documentation files (the
//  “Software”), to deal in the Software without restriction, including
//  without limitation the rights to use, copy, modify, merge, publish,
//  distribute, sublicense, and/or sell copies of the Software, and to
//  permit persons to whom the Software is furnished to do so.
//

#include <csignal>
#include <csetjmp>
#include <cfenv>
#include <fstream>
#include <unordered_map>
#include <functional>
#include <chart_main.h>

////////////////////////////////////////////////////////////////////////////////

#define ERR( MSG_ ) \
  do \
  { \
    std::cerr << "*** ERROR: " << MSG_ << std::endl; \
    exit( 1 ); \
  } while ( 0 )

////////////////////////////////////////////////////////////////////////////////

void show_version( void )
{
  std::cout << R"EOF(chartgen v0.10.0
This is free software: you are free to change and redistribute it.

Written by Soren Kragh
)EOF";
}

////////////////////////////////////////////////////////////////////////////////

void show_help( void )
{
  std::cout << R"EOF(Usage: chartgen [OPTION]... [FILE]...
Generate a chart in SVG format from FILE(s) to standard output.

With no FILE, or when FILE is -, read standard input.

  -t                Output a simple template file; a good starting point.
  -T                Output a full documentation file.
  -h, --help        Display this help and exit.
  -v, --version     Display version.

Examples:
  chartgen f - g    Process f's contents, then standard input, then g's
                    contents; output the resulting SVG to standard output.
  chartgen          Process standard input and output the resulting SVG
                    to standard output.
)EOF";
}

////////////////////////////////////////////////////////////////////////////////

void gen_template( bool full )
{
  if ( full ) {
    std::cout << R"EOF(#
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
#ChartArea: 1200 800

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

# Each new series should start with this specifier giving the name of the
# series (may be multi-line). The following Series specifiers associate to
# this.
Series.New: Name of series

# Series type may be:
#   Type        X-value     Description
#-------------------------------------------------------------------------------
#   XY          Number      X/Y plot (default). Regard X values as numbers and
#                           draw lines between data points, possibly with point
#                           markers.
#   Scatter     Number      Scatter plot. Same as XY but with no lines and
#                           always with point markers.
#   Line        Text        Line plot. Regard X values as text and draw lines
#                           between data points, possibly with point markers.
#   Lollipop    Text        Lollipop plot. Regard X values as text and draw
#                           lines from data points to zero; always with point
#                           markers.
#   Bar         Text        Bar plot. Regard X values as text and draw bars
#                           from data points to zero.
#-------------------------------------------------------------------------------
# Since the X values are true numbers for XY and Scatter types, these types
# cannot be shown on the same chart as any other types, where the X value is
# interpreted as a text string. This attribute applies to the current series and
# all subsequent series, or until it is redefined.
Series.Type: XY

# Associated Y-axis may be Primary or Secondary; the default is Primary. This
# attribute applies to the current series and all subsequent series, or until it
# is redefined.
Series.AxisY: Primary

# Set size (diameter) of point markers; for XY plot the default is zero (no
# point markers). This attribute applies to the current series and all
# subsequent series, or until it is redefined.
#Series.MarkerSize: 0

# The point marker shape may be Circle, Square, Triangle, or Diamond; default is
# Circle. This attribute applies to the current series and all subsequent
# series, or until it is redefined.
#Series.MarkerShape: Circle

# The style of the graph. The style is a number in the range from 0 to 71;
# if no Style specifier is given (recommended) it is assigned an incrementing
# number based on the last given Series.Style.
#  0 to  7: Solid line using 8 different colors
#  8 to 15: Same as 0 to 7 but with short dashed line
# 16 to 23: Same as 0 to 7 but with medium dashed line
# 24 to 31: Same as 0 to 7 but with long dashed line
# 32 to 63: Same as 0 to 31, but with thinner line
# 64 to 67: Black styles (solid, short/medium/long dash)
# 68 to 71: Same as 64 to 67, but with thinner line
#Series.Style: 4

# These are the X and Y values for the series. If no new series was created
# beforehand, an anonymous one will be automatically created. For XY and Scatter
# type plots the X value is a number, otherwise the X value is regarded as a
# text string, which must be single quoted if it contains spaces. Series.Data is
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
# using a text string as the X value (everything but XY and Scatter types)
# should use this way of specifying the series data. Use double-quotes if the
# text string contains spaces. If not enough new series were created beforehand,
# anonymous ones will be automatically created as needed.
Series.New  : Series 1
Series.AxisY: Primary
Series.New  : Series 2
Series.AxisY: Secondary
Series.Style: 70
Series.MarkerSize: 10
Series.Data:
#       X-value         Series 1        Series 2
        8               22              5e3
        30              3               18e3
        80              14              2e3

# EOF
)EOF";
  } else {
    std::cout << R"EOF(# Simple template
Title       : Chart Title
Axis.X.Label: X-Axis Label
Axis.Y.Label: Y-Axis Label
Series.New  : Name of series
Series.Data :
        0       23.7
        7.0     2.3
        40      20
        47      10.0
        71      4.3
        97      14

# Summary of all available specifiers (see -T template for help):
# ChartArea: 1200 800
# Margin: 5
# Title:
# SubTitle:
# SubSubTitle:
# Footnote:
# FootnotePos: Auto
# Axis.X.Orientation: Horizontal
# Axis.X.Reverse: Off
# Axis.Y.Reverse: Off
# Axis.X.Style: Auto
# Axis.Y.Style: Auto
# Axis.X.Label:
# Axis.Y.Label:
# Axis.X.SubLabel:
# Axis.Y.SubLabel:
# Axis.X.Unit:
# Axis.Y.Unit:
# Axis.X.UnitPos: Above
# Axis.Y.UnitPos: Right
# Axis.X.LogScale: On
# Axis.Y.LogScale: On
# Axis.X.Range: 0 100 90
# Axis.Y.Range: -5 25
# Axis.X.Pos: Top
# Axis.Y.Pos: Right
# Axis.X.Tick: 10.0 4
# Axis.Y.Tick: 1.0 0
# Axis.X.Grid: Off On
# Axis.Y.Grid: On Off
# Axis.X.GridStyle: Auto
# Axis.Y.GridStyle: Auto
# Axis.X.NumberFormat: Auto
# Axis.Y.NumberFormat: Auto
# Axis.X.NumberUnit: s
# Axis.Y.NumberUnit: _Volts
# Axis.X.MinorNumber: On
# Axis.Y.MinorNumber: Off
# Axis.X.NumberPos: Auto
# Axis.Y.NumberPos: Auto
# LegendPos: Below
# Series.New: Name of series
# Series.Type: XY
# Series.AxisY: Secondary
# Series.MarkerSize: 8
# Series.MarkerShape: Circle
# Series.Style: 32
# Series.Data:

)EOF";
  }
}

////////////////////////////////////////////////////////////////////////////////

bool is_ws( char c )
{
  return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

void trunc_ws( std::string& s )
{
  while ( s.length() > 0 && is_ws( s.back() ) ) {
    s.pop_back();
  }
}

void trunc_nl( std::string& s )
{
  while ( s.length() > 0 ) {
    if ( s.back() != '\r' && s.back() != '\n' ) break;
    s.pop_back();
  }
}

////////////////////////////////////////////////////////////////////////////////

Chart::Main chart;

std::vector< Chart::Series* > series_list;

struct LineRec {
  std::string line;
  std::string file_name;
  size_t      line_number;
};

std::vector< LineRec >           lines;
std::vector< LineRec >::iterator cur_line;
size_t                           cur_col;

void next_line( void )
{
  cur_line++;
  cur_col = 0;
}

// Start column of last parsed identifier.
size_t id_col;

////////////////////////////////////////////////////////////////////////////////

bool at_eof( void )
{
  return cur_line == lines.end();
}

bool at_eol( void )
{
  if ( at_eof() ) return true;
  if ( cur_col >= cur_line->line.length() ) return true;
  if ( cur_col == 0 && cur_line->line[ 0 ] == '#' ) return true;
  return false;
}

bool at_ws( void )
{
  return !at_eol() && is_ws( cur_line->line[ cur_col ] );
}

void skip_ws( bool multi_line = false )
{
  while ( !at_eof() ) {
    while ( !at_eol() ) {
      if ( !is_ws( cur_line->line[ cur_col ] ) ) return;
      cur_col++;
    }
    if ( !multi_line ) break;
    next_line();
  }
}

void parse_err( const std::string msg, bool revert_col = false )
{
  if ( revert_col ) cur_col = id_col;
  std::cerr << "*** PARSE ERROR: " << msg << "\n";
  if ( at_eof() ) {
    std::cerr << "at EOF";
  } else {
    if ( cur_col > cur_line->line.length() ) cur_col = cur_line->line.length();
    std::cerr
      << cur_line->file_name << " ("
      << cur_line->line_number << ','
      << cur_col << "):\n";
    std::cerr << cur_line->line << '\n';
    for ( size_t i = 0; i < cur_col; i++ ) {
      std::cerr << ' ';
    }
    std::cerr << '^';
  }
  std::cerr << '\n';
  exit( 1 );
}

////////////////////////////////////////////////////////////////////////////////

char get_char( bool adv = true )
{
  if ( at_eol() ) return '#';
  char c = cur_line->line[ cur_col ];
  if ( adv ) cur_col++;
  return c;
}

std::string get_identifier( bool all_non_ws = false )
{
  id_col = cur_col;
  std::string id = "";
  while ( !at_eol() ) {
    char c = cur_line->line[ cur_col ];
    if (
      (all_non_ws && !is_ws( c ) ) ||
      (c >= 'a' && c <= 'z') ||
      (c >= 'A' && c <= 'Z') ||
      (c >= '0' && c <= '9') ||
      (c == '.' || c == '-' || c == '+' || c == '_')
    ) {
      id.push_back( c );
      cur_col++;
    } else {
      break;
    }
  }
  return id;
}

// Attempts to parse an int64_t or double from the current position. If
// successful the function returns true and the current position is advanced,
// otherwise the function returns false and the current position is left
// unchanged.
bool get_int64( int64_t& i )
{
  id_col = cur_col;
  try {
    std::string str = cur_line->line.substr( cur_col );
    str.push_back( ' ' );
    std::istringstream iss( str );
    if ( !(iss >> i).fail() ) {
      cur_col += iss.tellg();
      return true;
    }
    return false;
  } catch ( const std::exception& e ) {
    return false;
  }
}
bool get_double( double& d )
{
  id_col = cur_col;
  try {
    std::string str = cur_line->line.substr( cur_col );
    str.push_back( ' ' );
    std::istringstream iss( str );
    if ( !(iss >> d).fail() ) {
      cur_col += iss.tellg();
      return true;
    }
    return false;
  } catch ( const std::exception& e ) {
    return false;
  }
}

// Read in a text based X-value which defines a category for the series.
bool get_category( std::string& t )
{
  t = "";
  auto fst_col = cur_col;
  bool in_quote = false;
  while ( !at_eol() ) {
    char c = cur_line->line[ cur_col ];
    if ( c == '"' ) {
      if ( cur_col == fst_col ) {
        in_quote = true;
        cur_col++;
        continue;
      } else {
        if ( in_quote ) {
          in_quote = false;
          cur_col++;
        } else {
          in_quote = true;
        }
        break;
      }
    }
    if ( is_ws( c ) ) {
      if ( !in_quote ) break;
    }
    t.push_back( c );
    cur_col++;
  }
  return !in_quote;
}

////////////////////////////////////////////////////////////////////////////////

bool get_key( std::string& key )
{
  key = "";
  skip_ws( true );
  if ( at_eof() ) return false;
  if ( cur_col > 0 ) parse_err( "KEY must be unindented" );
  key = get_identifier();
  skip_ws();
  if ( key == "" ) parse_err( "KEY expected" );
  if ( get_char( false ) != ':' ) parse_err( "':' expected" );
  get_char();
  return true;
}

void get_text( std::string& txt, bool multi_line )
{
  txt = "";
  skip_ws();
  while ( !at_eol() ) txt.push_back( get_char() );
  trunc_ws( txt );
  if ( txt != "" || !multi_line ) return;
  next_line();
  auto txt_line = cur_line;
  size_t indent = 0;
  while ( !at_eof() ) {
    while ( at_ws() ) cur_col++;
    if ( !at_eol() ) {
      if ( cur_col == 0 ) break;
      if ( indent == 0 || cur_col < indent ) indent = cur_col;
    }
    next_line();
  }
  cur_line = txt_line;
  if ( indent < 1 ) return;
  while ( !at_eof() ) {
    if ( txt != "" ) txt.push_back( '\n' );
    while ( at_ws() ) cur_col++;
    if ( !at_eol() ) {
      if ( cur_col < indent ) break;
      cur_col = indent;
      while ( !at_eol() ) txt.push_back( get_char() );
      trunc_ws( txt );
    }
    next_line();
  }
  trunc_ws( txt );
}

////////////////////////////////////////////////////////////////////////////////

void expect_eol( void )
{
  skip_ws();
  if ( !at_eol() ) parse_err( "garbage at EOL" );
}

void expect_ws( const std::string err_msg_if_eol = "" )
{
  auto old_col = cur_col;
  skip_ws();
  if ( cur_col > old_col && !at_eol() ) return;
  if ( at_eol() && err_msg_if_eol != "" ) parse_err( err_msg_if_eol );
  if ( !(old_col < cur_col) ) parse_err( "whitespace expected" );
}

////////////////////////////////////////////////////////////////////////////////

void do_Pos(
  Chart::Pos& pos
)
{
  std::string id = get_identifier( true );
  if ( id == "Auto"   ) pos = Chart::Pos::Auto  ; else
  if ( id == "Center" ) pos = Chart::Pos::Center; else
  if ( id == "Left"   ) pos = Chart::Pos::Left  ; else
  if ( id == "Right"  ) pos = Chart::Pos::Right ; else
  if ( id == "Top"    ) pos = Chart::Pos::Top   ; else
  if ( id == "Bottom" ) pos = Chart::Pos::Bottom; else
  if ( id == "Above"  ) pos = Chart::Pos::Top   ; else
  if ( id == "Below"  ) pos = Chart::Pos::Bottom; else
  if ( id == "" ) parse_err( "position expected" ); else
  parse_err( "unknown position '" + id + "'", true );
}

void do_Switch(
  bool& flag
)
{
  std::string id = get_identifier( true );
  if ( id == "On"  ) flag = true ; else
  if ( id == "Off" ) flag = false; else
  if ( id == "" ) parse_err( "On/Off expected" ); else
  parse_err( "On/Off expected, saw '" + id + "'", true );
}

//-----------------------------------------------------------------------------

void do_ChartArea( void )
{
  int64_t w;
  int64_t h;

  skip_ws();
  if ( at_eol() ) parse_err( "width expected" );
  if ( !get_int64( w ) ) parse_err( "malformed width" );
  if ( w < 100 || w > 100000 ) parse_err( "width out of range", true );

  expect_ws( "height expected" );
  if ( !get_int64( h ) ) parse_err( "malformed height" );
  if ( h < 100 || h > 100000 ) parse_err( "height out of range", true );

  expect_eol();
  chart.SetChartArea( w, h );
}

void do_Margin( void )
{
  int64_t m;
  skip_ws();
  if ( at_eol() ) parse_err( "margin expected" );
  if ( !get_int64( m ) ) parse_err( "malformed margin" );
  if ( m < 0 || m > 1000 ) parse_err( "margin out of range", true );
  expect_eol();
  chart.SetMargin( m );
}

void do_Title( void )
{
  std::string txt;
  get_text( txt, true );
  chart.SetTitle( txt );
}

void do_SubTitle( void )
{
  std::string txt;
  get_text( txt, true );
  chart.SetSubTitle( txt );
}

void do_SubSubTitle( void )
{
  std::string txt;
  get_text( txt, true );
  chart.SetSubSubTitle( txt );
}

void do_Footnote( void )
{
  std::string txt;
  get_text( txt, true );
  chart.SetFootnote( txt );
}

void do_FootnotePos( void )
{
  Chart::Pos pos;
  skip_ws();
  do_Pos( pos );
  expect_eol();
  chart.SetFootnotePos( pos );
}

//-----------------------------------------------------------------------------

void do_Axis_Orientation( Chart::Axis* axis )
{
  bool vertical;

  skip_ws();
  std::string id = get_identifier( true );
  if ( id == "Horizontal" ) vertical = false; else
  if ( id == "Vertical"   ) vertical = true ; else
  if ( id == "" ) parse_err( "axis orientation expected" ); else
  parse_err( "unknown axis orientation '" + id + "'", true );
  expect_eol();

  vertical = (axis == chart.AxisX()) ? vertical : !vertical;
  chart.AxisX(   )->SetAngle( vertical ? 90 :  0 );
  chart.AxisY( 0 )->SetAngle( vertical ?  0 : 90 );
  chart.AxisY( 1 )->SetAngle( vertical ?  0 : 90 );
}

//-----------------------------------------------------------------------------

void do_Axis_Reverse( Chart::Axis* axis )
{
  bool reverse;
  skip_ws();
  do_Switch( reverse );
  expect_eol();
  axis->SetReverse( reverse );
}

//-----------------------------------------------------------------------------

void do_Axis_Style( Chart::Axis* axis )
{
  Chart::AxisStyle style;
  skip_ws();

  std::string id = get_identifier( true );
  if ( id == "Auto"   ) style = Chart::AxisStyle::Auto ; else
  if ( id == "None"   ) style = Chart::AxisStyle::None ; else
  if ( id == "Arrow"  ) style = Chart::AxisStyle::Arrow; else
  if ( id == "Edge"   ) style = Chart::AxisStyle::Edge ; else
  if ( id == "" ) parse_err( "axis style expected" ); else
  parse_err( "unknown axis style '" + id + "'", true );

  expect_eol();
  axis->SetStyle( style );
}

//-----------------------------------------------------------------------------

void do_Axis_Label( Chart::Axis* axis )
{
  std::string txt;
  get_text( txt, true );
  axis->SetLabel( txt );
}

//-----------------------------------------------------------------------------

void do_Axis_SubLabel( Chart::Axis* axis )
{
  std::string txt;
  get_text( txt, true );
  axis->SetSubLabel( txt );
}

//-----------------------------------------------------------------------------

void do_Axis_Unit( Chart::Axis* axis )
{
  std::string txt;
  get_text( txt, true );
  axis->SetUnit( txt );
}

//-----------------------------------------------------------------------------

void do_Axis_UnitPos( Chart::Axis* axis )
{
  Chart::Pos pos;
  skip_ws();
  do_Pos( pos );
  expect_eol();
  axis->SetUnitPos( pos );
}

//-----------------------------------------------------------------------------

void do_Axis_LogScale( Chart::Axis* axis )
{
  bool log_scale;
  skip_ws();
  do_Switch( log_scale );
  expect_eol();
  axis->SetLogScale( log_scale );
}

//-----------------------------------------------------------------------------

void do_Axis_Range( Chart::Axis* axis )
{
  double min;
  double max;
  double cross;

  skip_ws();
  if ( at_eol() ) parse_err( "min expected" );
  if ( !get_double( min ) ) parse_err( "malformed min" );

  expect_ws( "max expected" );
  if ( !get_double( max ) ) parse_err( "malformed max" );
  if ( !(max > min) ) parse_err( "max must be greater than min", true );

  cross = 0;
  if ( !at_eol() ) {
    expect_ws();
    if ( !at_eol() ) {
      if ( !get_double( cross ) ) parse_err( "malformed orthogonal axis cross" );
    }
    expect_eol();
  }

  axis->SetRange( min, max, cross );
}

//-----------------------------------------------------------------------------

void do_Axis_Pos( Chart::Axis* axis )
{
  Chart::Pos pos;
  skip_ws();
  do_Pos( pos );
  expect_eol();
  axis->SetPos( pos );
}

//-----------------------------------------------------------------------------

void do_Axis_Tick( Chart::Axis* axis )
{
  double major;
  int64_t minor;

  skip_ws();
  if ( at_eol() ) parse_err( "major tick expected" );
  if ( !get_double( major ) ) parse_err( "malformed major tick" );
  if ( !(major > 0) ) parse_err( "major tick must be positive", true );

  expect_ws( "minor tick expected" );
  if ( !get_int64( minor ) ) parse_err( "malformed minor tick" );
  if ( minor < 0 || minor > 100 ) parse_err( "minor tick out of range", true );

  expect_eol();

  axis->SetTick( major, minor );
}

//-----------------------------------------------------------------------------

void do_Axis_Grid( Chart::Axis* axis )
{
  bool major;
  bool minor;

  skip_ws();
  if ( at_eol() ) parse_err( "major grid expected" );
  do_Switch( major );

  expect_ws( "minor grid expected" );
  do_Switch( minor );

  expect_eol();

  axis->SetGrid( major, minor );
}

//-----------------------------------------------------------------------------

void do_Axis_GridStyle( Chart::Axis* axis )
{
  Chart::GridStyle style;
  skip_ws();
  std::string id = get_identifier( true );
  if ( id == "Auto"  ) style = Chart::GridStyle::Auto ; else
  if ( id == "Dash"  ) style = Chart::GridStyle::Dash ; else
  if ( id == "Solid" ) style = Chart::GridStyle::Solid; else
  if ( id == "" ) parse_err( "grid style expected" ); else
  parse_err( "unknown grid style '" + id + "'", true );
  expect_eol();
  axis->SetGridStyle( style );
}

//-----------------------------------------------------------------------------

void do_Axis_NumberFormat( Chart::Axis* axis )
{
  Chart::NumberFormat number_format;
  skip_ws();

  std::string id = get_identifier( true );
  if ( id == "Auto"       ) number_format = Chart::NumberFormat::Auto      ; else
  if ( id == "None"       ) number_format = Chart::NumberFormat::None      ; else
  if ( id == "Fixed"      ) number_format = Chart::NumberFormat::Fixed     ; else
  if ( id == "Scientific" ) number_format = Chart::NumberFormat::Scientific; else
  if ( id == "Magnitude"  ) number_format = Chart::NumberFormat::Magnitude ; else
  if ( id == "" ) parse_err( "number format expected" ); else
  parse_err( "unknown number format '" + id + "'", true );

  expect_eol();
  axis->SetNumberFormat( number_format );
}

//-----------------------------------------------------------------------------

void do_Axis_NumberUnit( Chart::Axis* axis )
{
  std::string txt;
  get_text( txt, false );
  for ( char& c : txt ) {
    if ( c != '_' ) break;
    c = ' ';
  }
  axis->SetNumberUnit( txt );
}

//-----------------------------------------------------------------------------

void do_Axis_MinorNumber( Chart::Axis* axis )
{
  bool minor_num;
  skip_ws();
  do_Switch( minor_num );
  expect_eol();
  axis->ShowMinorNumbers( minor_num );
}

//-----------------------------------------------------------------------------

void do_Axis_NumberPos( Chart::Axis* axis )
{
  Chart::Pos pos;
  skip_ws();
  do_Pos( pos );
  expect_eol();
  axis->SetNumberPos( pos );
}

//-----------------------------------------------------------------------------

void do_LegendPos( void )
{
  Chart::Pos pos;
  skip_ws();
  do_Pos( pos );
  expect_eol();
  chart.SetLegendPos( pos );
}

//-----------------------------------------------------------------------------

bool defining_series = false;
Chart::SeriesType series_type = Chart::SeriesType::XY;
bool x_is_text = false;
int32_t category_idx = 0;
int axis_y_n = 0;
int64_t marker_size = 0;
Chart::MarkerShape marker_shape = Chart::MarkerShape::Circle;
int64_t style = 0;

void NextSeriesStyle( void )
{
  if ( style < 64 ) {
    style = (style + 1) % 64;
  } else {
    style = 64 + ((style + 1) % 8);
  }
}

void AddSeries( std::string name = "" )
{
  series_list.push_back( chart.AddSeries( name ) );
  series_list.back()->SetType( series_type );
  series_list.back()->SetAxisY( axis_y_n );
  if ( marker_size > 0 ) {
    series_list.back()->SetMarkerSize( marker_size );
  }
  series_list.back()->SetMarkerShape( marker_shape );
  series_list.back()->SetStyle( style );
  NextSeriesStyle();
  defining_series = true;
}

void do_Series_New( void )
{
  std::string txt;
  get_text( txt, true );
  AddSeries( txt );
}

void do_Series_Type( void )
{
  skip_ws();
  std::string id = get_identifier( true );
  if ( id == "XY"       ) series_type = Chart::SeriesType::XY      ; else
  if ( id == "Scatter"  ) series_type = Chart::SeriesType::Scatter ; else
  if ( id == "Line"     ) series_type = Chart::SeriesType::Line    ; else
  if ( id == "Lollipop" ) series_type = Chart::SeriesType::Lollipop; else
  if ( id == "Bar"      ) series_type = Chart::SeriesType::Bar     ; else
  if ( id == "" ) parse_err( "series type expected" ); else
  parse_err( "unknown series type '" + id + "'", true );
  expect_eol();
  if ( defining_series ) {
    series_list.back()->SetType( series_type );
  }
  if (
    series_type != Chart::SeriesType::XY &&
    series_type != Chart::SeriesType::Scatter
  ) {
    x_is_text = true;
  }
}

void do_Series_AxisY( void )
{
  skip_ws();
  std::string id = get_identifier( true );
  if ( id == "Primary"   ) axis_y_n = 0; else
  if ( id == "Secondary" ) axis_y_n = 1; else
  if ( id == "" ) parse_err( "Primary or Secondary expected" ); else
  parse_err( "unknown axis '" + id + "'", true );
  expect_eol();
  if ( defining_series ) {
    series_list.back()->SetAxisY( axis_y_n );
  }
}

void do_Series_MarkerSize( void )
{
  skip_ws();
  if ( at_eol() ) parse_err( "marker size expected" );
  if ( !get_int64( marker_size ) ) parse_err( "malformed marker size" );
  if ( marker_size < 0 || marker_size > 100 ) parse_err( "marker size out of range", true );
  expect_eol();
  if ( defining_series ) {
    series_list.back()->SetMarkerSize( marker_size );
  }
}

void do_Series_MarkerShape( void )
{
  skip_ws();
  std::string id = get_identifier( true );
  if ( id == "Circle"   ) marker_shape = Chart::MarkerShape::Circle  ; else
  if ( id == "Square"   ) marker_shape = Chart::MarkerShape::Square  ; else
  if ( id == "Triangle" ) marker_shape = Chart::MarkerShape::Triangle; else
  if ( id == "Diamond"  ) marker_shape = Chart::MarkerShape::Diamond ; else
  if ( id == "" ) parse_err( "marker shape expected" ); else
  parse_err( "unknown marker shape '" + id + "'", true );
  expect_eol();
  if ( defining_series ) {
    series_list.back()->SetMarkerShape( marker_shape );
  }
}

void do_Series_Style( void )
{
  skip_ws();
  if ( at_eol() ) parse_err( "style expected" );
  if ( !get_int64( style ) ) parse_err( "malformed style" );
  if ( style < 0 || style > 71 ) parse_err( "style out of range", true );
  expect_eol();
  if ( defining_series ) {
    series_list.back()->SetStyle( style );
    NextSeriesStyle();
  }
}

void do_Series_Data( void )
{
  expect_eol();
  next_line();
  bool first = true;
  uint32_t y_values = 0;
  std::string category;
  while ( !at_eof() ) {
    skip_ws( true );
    cur_col = 0;
    if ( !at_ws() ) break;
    expect_ws( "x-value expected" );
    double x;
    if ( x_is_text ) {
      if ( !get_category( category ) ) parse_err( "malformed x-value" );
      chart.AddCategory( category );
      x = category_idx;
      category_idx++;
    } else {
      if ( !get_double( x ) ) parse_err( "malformed x-value" );
    }
    uint32_t n = 0;
    auto fst_y_col = cur_col;
    while ( true ) {
      expect_ws( "y-value expected" );
      auto old_col = cur_col;
      double y;
      if ( !get_double( y ) ) parse_err( "malformed y-value" );
      if ( !at_ws() && !at_eol() ) parse_err( "syntax error" );
      if ( !first ) {
        if ( n >= y_values ) {
          cur_col = old_col;
          parse_err( "too many y-values" );
        }
        series_list[ series_list.size() - y_values + n ]->Add( x, y );
      }
      n++;
      old_col = cur_col;
      skip_ws();
      if ( at_eol() ) {
        if ( first ) {
          first = false;
          y_values = n;
          n = 0;
          cur_col = fst_y_col;
          for ( uint32_t i = 0; i < y_values; i++ ) {
            if (
              series_list.size() == i ||
              series_list[ series_list.size() - i - 1 ]->Size() > 0
            )
              AddSeries();
          }
          continue;
        }
        break;
      }
      cur_col = old_col;
    }
    expect_eol();
    if ( n < y_values ) parse_err( "another y-value expected" );
  }
  defining_series = false;
}

////////////////////////////////////////////////////////////////////////////////

using ChartAction = std::function< void() >;

std::unordered_map< std::string, ChartAction > chart_actions = {
  { "ChartArea"         , do_ChartArea          },
  { "Margin"            , do_Margin             },
  { "Title"             , do_Title              },
  { "SubTitle"          , do_SubTitle           },
  { "SubSubTitle"       , do_SubSubTitle        },
  { "Footnote"          , do_Footnote           },
  { "FootnotePos"       , do_FootnotePos        },
  { "LegendPos"         , do_LegendPos          },
  { "Series.New"        , do_Series_New         },
  { "Series.Type"       , do_Series_Type        },
  { "Series.AxisY"      , do_Series_AxisY       },
  { "Series.MarkerSize" , do_Series_MarkerSize  },
  { "Series.MarkerShape", do_Series_MarkerShape },
  { "Series.Style"      , do_Series_Style       },
  { "Series.Data"       , do_Series_Data        },
};

using AxisAction = std::function< void( Chart::Axis* ) >;

std::unordered_map< std::string, AxisAction > axis_actions = {
  { "Orientation" , do_Axis_Orientation  },
  { "Reverse"     , do_Axis_Reverse      },
  { "Style"       , do_Axis_Style        },
  { "Label"       , do_Axis_Label        },
  { "SubLabel"    , do_Axis_SubLabel     },
  { "Unit"        , do_Axis_Unit         },
  { "UnitPos"     , do_Axis_UnitPos      },
  { "LogScale"    , do_Axis_LogScale     },
  { "Range"       , do_Axis_Range        },
  { "Pos"         , do_Axis_Pos          },
  { "Tick"        , do_Axis_Tick         },
  { "Grid"        , do_Axis_Grid         },
  { "GridStyle"   , do_Axis_GridStyle    },
  { "NumberFormat", do_Axis_NumberFormat },
  { "NumberUnit"  , do_Axis_NumberUnit   },
  { "MinorNumber" , do_Axis_MinorNumber  },
  { "NumberPos"   , do_Axis_NumberPos    },
};

bool parse_spec( void )
{
  std::string key;
  if ( !get_key( key ) ) return false;

  bool ok = false;

  do {
    if ( key.substr( 0, 5 ) == "Axis." ) {
      size_t i = key.find( '.', 5 );
      if ( i == std::string::npos ) break;
      std::string axis_id = key.substr( 5, i - 5 );
      Chart::Axis* axis = nullptr;
      if ( axis_id == "X"    ) axis = chart.AxisX(   ); else
      if ( axis_id == "Y"    ) axis = chart.AxisY(   ); else
      if ( axis_id == "PriY" ) axis = chart.AxisY( 0 ); else
      if ( axis_id == "SecY" ) axis = chart.AxisY( 1 ); else
      break;
      auto it = axis_actions.find( key.substr( i + 1 ) );
      if ( it == axis_actions.end() ) break;
      it->second( axis );
      ok = true;
    } else {
      auto it = chart_actions.find( key );
      if ( it == chart_actions.end() ) break;
      it->second();
      ok = true;
    }
  } while ( false );

  if ( !ok ) parse_err( "Unknown KEY '" + key + "'", true );

  return true;
}

////////////////////////////////////////////////////////////////////////////////

void parse_lines( void )
{
  cur_line = lines.begin();
  cur_col = 0;
  while ( parse_spec() ) {}
}

////////////////////////////////////////////////////////////////////////////////

void process_files( const std::vector< std::string >& file_list )
{
  for ( const auto& file_name : file_list ) {
    size_t line_number = 0;
    if ( file_name == "-" ) {
      std::string line;
      while ( std::getline( std::cin, line ) ) {
        trunc_nl( line );
        lines.push_back( { line, "-", ++line_number } );
      }
    } else {
      std::ifstream file( file_name );
      if ( file ) {
        std::string line;
        while ( std::getline( file, line ) ) {
          trunc_nl( line );
          lines.push_back( { line, file_name, ++line_number } );
        }
        file.close();
      } else {
        ERR( "Unable to open file '" << file_name << "'" );
      }
    }
  }
  parse_lines();
}

////////////////////////////////////////////////////////////////////////////////

std::jmp_buf sigfpe_jmp;

void sigfpe_handler( int signum )
{
  longjmp( sigfpe_jmp, 1 );
}

int main( int argc, char* argv[] )
{
  if ( setjmp( sigfpe_jmp ) ) {
    SVG::Canvas* canvas = new SVG::Canvas();
    SVG::Group* g = canvas->TopGroup();
    g->Add( new SVG::Text( 0, 0, "Floating point exception" ) );
    g->Last()->Attr()->TextFont()->SetSize( 48 )->SetBold();
    SVG::BoundaryBox bb = g->Last()->GetBB();
    g->Add(
      new SVG::Rect(
        bb.min.x - 20, bb.min.y - 20, bb.max.x + 20, bb.max.y + 20, 20
      )
    );
    g->FrontToBack();
    g->Last()->Attr()->SetLineWidth( 10 )->FillColor()->Set( SVG::ColorName::Red, 0.25 );
    std::cout << canvas->GenSVG( 10 );
    ERR( "Floating point exception" );
  }
  signal( SIGFPE, sigfpe_handler );
  feenableexcept( FE_DIVBYZERO | FE_INVALID );

  std::vector< std::string > file_list;

  bool out_of_options = false;
  for ( int i = 1; i < argc; i++ ) {
    std::string a( argv[ i ] );
    if ( a == "--" ) {
      out_of_options = true;
      continue;
    }
    if ( !out_of_options ) {
      if ( a == "-v" || a == "--version" ) {
        show_version();
        return 0;
      }
      if ( a == "-h" || a == "--help" ) {
        show_help();
        return 0;
      }
      if ( a == "-t" ) {
        gen_template( false );
        return 0;
      }
      if ( a == "-T" ) {
        gen_template( true );
        return 0;
      }
      if ( a != "-" && a[ 0 ] == '-' ) {
        ERR( "Unrecognized option '" << a << "'; try --help" );
      }
    }
    file_list.push_back( a );
  }

  if ( file_list.size() == 0 ) {
    file_list.push_back( "-" );
  }

  process_files( file_list );

  std::ostringstream oss;
  SVG::Canvas* canvas = chart.Build();
  oss << canvas->GenSVG();
  delete canvas;
  std::cout << oss.str();

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
