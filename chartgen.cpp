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

///////////////////////////////////////////////////////////////////////////////

#define ERR( MSG_ ) \
  do \
  { \
    std::cerr << "*** ERROR: " << MSG_ << std::endl; \
    exit( 1 ); \
  } while ( 0 )

///////////////////////////////////////////////////////////////////////////////

void show_version( void )
{
  std::cout << R"EOF(chartgen 1.00
This is free software: you are free to change and redistribute it.

Written by S. Kragh
)EOF";
}

///////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////

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
  1) First footnote.
  2) Second footnote;
     with an extra line.

Axis.X.Label: X-Axis
Axis.Y.Label: Y-Axis

# Especially for linear scale, it is often a good idea to select units and
# scale the data accordingly to avoid very small or very large numbers in the
# graph. It is a cardinal sin to not have units on your axes.
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

# Min, max, and optionally where the other orthogonal axis crosses this axis.
# Auto ranging is applied if no Range specifier is given (recommended).
#Axis.X.Range: 0 100 90
#Axis.Y.Range: -5 25

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

# Turn grid lines on/off for major and minor ticks; may be On or Off.
#Axis.X.Grid: On On
#Axis.Y.Grid: On Off

# Number format may be Fixed, Scientific, or Magnitude. Default is Fixed for
# linear scale and Magnitude for logarithmic scale. Magnitude means showing
# e.g. "10k" instead of "10000" etc.
#Axis.X.NumberFormat: Fixed
#Axis.Y.NumberFormat: Fixed

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

# Set size of point markers; default is zero (no point markers). The size
# indicates by how much the point marker width is larger than the width of the
# graph line. This attribute applies to the current series and all subsequent
# series, or until it is redefined.
#Series.PointSize: 0

# The style of the X/Y graph. The style is a number in the range from 0 to 63;
# if no Style specifier is given (recommended) it is assigned an incrementing
# number based on the last given Series.Style.
#  0 to  7: Solid line using 8 different colors
#  8 to 15: Same as 0 to 7 but with short dashed line
# 16 to 23: Same as 0 to 7 but with medium dashed line
# 24 to 31: Same as 0 to 7 but with long dashed line
# 32 to 63: Same as 0 to 31, but with thinner line
#Series.Style: 4

# These are the X/Y values for the series; only X/Y chart type is supported for
# now. If no new series was created beforehand, an anonymous one will be
# automatically created. Series.Data is the last specifier for a given series;
# any Series specifies after the Series.Data specifier apply to the next
# series.
Series.Data:
        0       23.7
        7.0     2.3
        23      7e-2
        47      10.0
        71      4.3
        97      14

# Several series sharing the same X-values can be specified in one go. If not
# enough new series were created beforehand, anonymous ones will be
# automatically created as needed. Series.Data is the last specifier for a
# given series; any Series specifies after the Series.Data specifier apply to
# the next series.
Series.New: Series 1
Series.New: Series 2
Series.Data:
#       X-value         Series 1        Series 2
        8               22              5
        30              3               18
        80              14              2

# EOF
)EOF";
  } else {
    std::cout << R"EOF(# Simple template
Title       : Chart Title
Axis.X.Unit : missing unit
Axis.Y.Unit : missing unit
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
# Axis.X.Label:
# Axis.Y.Label:
# Axis.X.Unit:
# Axis.Y.Unit:
# Axis.X.UnitPos: Above
# Axis.Y.UnitPos: Right
# Axis.X.LogScale: On
# Axis.Y.LogScale: On
# Axis.X.Range: 0 100 90
# Axis.Y.Range: -5 25
# Axis.X.Tick: 10.0 4
# Axis.Y.Tick: 1.0 0
# Axis.X.Grid: Off On
# Axis.Y.Grid: On Off
# Axis.X.NumberFormat: Fixed
# Axis.Y.NumberFormat: Fixed
# Axis.X.NumberUnit: s
# Axis.Y.NumberUnit: _Volts
# Axis.X.MinorNumber: On
# Axis.Y.MinorNumber: Off
# Axis.X.NumberPos: Auto
# Axis.Y.NumberPos: Auto
# LegendPos: Below
# Series.New: Name of series
# Series.PointSize: 8
# Series.Style: 32
# Series.Data:

)EOF";
  }
}

///////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////

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
  if ( id == "Above"  ) pos = Chart::Pos::Above ; else
  if ( id == "Below"  ) pos = Chart::Pos::Below ; else
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

void do_NumberFormat(
  Chart::NumberFormat& number_format
)
{
  std::string id = get_identifier( true );
  if ( id == "Fixed"      ) number_format = Chart::NumberFormat::Fixed     ; else
  if ( id == "Scientific" ) number_format = Chart::NumberFormat::Scientific; else
  if ( id == "Magnitude"  ) number_format = Chart::NumberFormat::Magnitude ; else
  if ( id == "" ) parse_err( "number format expected" ); else
  parse_err( "unknown number format '" + id + "'", true );
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

//-----------------------------------------------------------------------------

void do_Axis_X_Label( void )
{
  std::string txt;
  get_text( txt, true );
  chart.AxisX()->SetLabel( txt );
}

void do_Axis_Y_Label( void )
{
  std::string txt;
  get_text( txt, true );
  chart.AxisY()->SetLabel( txt );
}

//-----------------------------------------------------------------------------

void do_Axis_X_Unit( void )
{
  std::string txt;
  get_text( txt, true );
  chart.AxisX()->SetUnit( txt );
}

void do_Axis_Y_Unit( void )
{
  std::string txt;
  get_text( txt, true );
  chart.AxisY()->SetUnit( txt );
}

//-----------------------------------------------------------------------------

void do_Axis_X_UnitPos( void )
{
  Chart::Pos pos;
  skip_ws();
  do_Pos( pos );
  expect_eol();
  chart.AxisX()->SetUnitPos( pos );
}

void do_Axis_Y_UnitPos( void )
{
  Chart::Pos pos;
  skip_ws();
  do_Pos( pos );
  expect_eol();
  chart.AxisY()->SetUnitPos( pos );
}

//-----------------------------------------------------------------------------

void do_Axis_X_LogScale( void )
{
  bool log_scale;
  skip_ws();
  do_Switch( log_scale );
  expect_eol();
  chart.AxisX()->SetLogScale( log_scale );
}

void do_Axis_Y_LogScale( void )
{
  bool log_scale;
  skip_ws();
  do_Switch( log_scale );
  expect_eol();
  chart.AxisY()->SetLogScale( log_scale );
}

//-----------------------------------------------------------------------------

void do_Axis_Range(
  double& min,
  double& max,
  double& cross
)
{
  skip_ws();
  if ( at_eol() ) parse_err( "min expected" );
  if ( !get_double( min ) ) parse_err( "malformed min" );

  expect_ws( "max expected" );
  if ( !get_double( max ) ) parse_err( "malformed max" );
  if ( !(max > min) ) parse_err( "max must be greater than min", true );

  cross = min;
  if ( at_eol() ) return;
  expect_ws();
  if ( !at_eol() ) {
    if ( !get_double( cross ) ) parse_err( "malformed orthogonal axis cross" );
  }

  expect_eol();
}

void do_Axis_X_Range( void )
{
  double min;
  double max;
  double cross;
  do_Axis_Range( min, max, cross );
  chart.AxisX()->SetRange( min, max, cross );
}

void do_Axis_Y_Range( void )
{
  double min;
  double max;
  double cross;
  do_Axis_Range( min, max, cross );
  chart.AxisY()->SetRange( min, max, cross );
}

//-----------------------------------------------------------------------------

void do_Axis_Tick(
  double& major,
  int64_t& minor
)
{
  skip_ws();
  if ( at_eol() ) parse_err( "major tick expected" );
  if ( !get_double( major ) ) parse_err( "malformed major tick" );
  if ( !(major > 0) ) parse_err( "major tick must be positive", true );

  expect_ws( "minor tick expected" );
  if ( !get_int64( minor ) ) parse_err( "malformed minor tick" );
  if ( minor < 0 || minor > 100 ) parse_err( "minor tick out of range", true );

  expect_eol();
}

void do_Axis_X_Tick( void )
{
  double major;
  int64_t minor;
  do_Axis_Tick( major, minor );
  chart.AxisX()->SetTick( major, minor );
}

void do_Axis_Y_Tick( void )
{
  double major;
  int64_t minor;
  do_Axis_Tick( major, minor );
  chart.AxisY()->SetTick( major, minor );
}

//-----------------------------------------------------------------------------

void do_Axis_Grid(
  bool& major,
  bool& minor
)
{
  skip_ws();
  if ( at_eol() ) parse_err( "major grid expected" );
  do_Switch( major );

  expect_ws( "minor grid expected" );
  do_Switch( minor );

  expect_eol();
}

void do_Axis_X_Grid( void )
{
  bool major;
  bool minor;
  do_Axis_Grid( major, minor );
  chart.AxisX()->SetGrid( major, minor );
}

void do_Axis_Y_Grid( void )
{
  bool major;
  bool minor;
  do_Axis_Grid( major, minor );
  chart.AxisY()->SetGrid( major, minor );
}

//-----------------------------------------------------------------------------

void do_Axis_X_NumberFormat( void )
{
  Chart::NumberFormat number_format;
  skip_ws();
  do_NumberFormat( number_format );
  expect_eol();
  chart.AxisX()->SetNumberFormat( number_format );
}

void do_Axis_Y_NumberFormat( void )
{
  Chart::NumberFormat number_format;
  skip_ws();
  do_NumberFormat( number_format );
  expect_eol();
  chart.AxisY()->SetNumberFormat( number_format );
}

//-----------------------------------------------------------------------------

void do_Axis_X_NumberUnit( void )
{
  std::string txt;
  get_text( txt, false );
  for ( char& c : txt ) {
    if ( c != '_' ) break;
    c = ' ';
  }
  chart.AxisX()->SetNumberUnit( txt );
}

void do_Axis_Y_NumberUnit( void )
{
  std::string txt;
  get_text( txt, false );
  for ( char& c : txt ) {
    if ( c != '_' ) break;
    c = ' ';
  }
  chart.AxisY()->SetNumberUnit( txt );
}

//-----------------------------------------------------------------------------

void do_Axis_X_MinorNumber( void )
{
  bool minor_num;
  skip_ws();
  do_Switch( minor_num );
  expect_eol();
  chart.AxisX()->ShowMinorNumbers( minor_num );
}

void do_Axis_Y_MinorNumber( void )
{
  bool minor_num;
  skip_ws();
  do_Switch( minor_num );
  expect_eol();
  chart.AxisY()->ShowMinorNumbers( minor_num );
}

//-----------------------------------------------------------------------------

void do_Axis_X_NumberPos( void )
{
  Chart::Pos pos;
  skip_ws();
  do_Pos( pos );
  expect_eol();
  chart.AxisX()->SetNumberPos( pos );
}

void do_Axis_Y_NumberPos( void )
{
  Chart::Pos pos;
  skip_ws();
  do_Pos( pos );
  expect_eol();
  chart.AxisY()->SetNumberPos( pos );
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
int64_t point_size = 0;
int64_t style = 0;

void AddSeries( std::string name = "" )
{
  series_list.push_back( chart.AddSeries( name ) );
  if ( point_size > 0 ) {
    series_list.back()->SetPointSize( point_size );
  }
  series_list.back()->SetStyle( style );
  style++;
  defining_series = true;
}

void do_Series_New( void )
{
  std::string txt;
  get_text( txt, true );
  AddSeries( txt );
}

void do_Series_PointSize( void )
{
  skip_ws();
  if ( at_eol() ) parse_err( "point size expected" );
  if ( !get_int64( point_size ) ) parse_err( "malformed point size" );
  if ( point_size < 0 || point_size > 100 ) parse_err( "point size out of range", true );
  expect_eol();
  if ( defining_series ) {
    series_list.back()->SetPointSize( point_size );
  }
}

void do_Series_Style( void )
{
  skip_ws();
  if ( at_eol() ) parse_err( "style expected" );
  if ( !get_int64( style ) ) parse_err( "malformed style" );
  if ( style < 0 || style > 63 ) parse_err( "style out of range", true );
  expect_eol();
  if ( defining_series ) {
    series_list.back()->SetStyle( style++ );
  }
}

void do_Series_Data( void )
{
  expect_eol();
  next_line();
  bool first = true;
  uint32_t y_values = 0;
  while ( !at_eof() ) {
    skip_ws( true );
    cur_col = 0;
    if ( !at_ws() ) break;
    expect_ws( "x-value expected" );
    double x;
    if ( !get_double( x ) ) parse_err( "malformed x-value" );
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

///////////////////////////////////////////////////////////////////////////////

using ParseAction = std::function<void()>;

std::unordered_map< std::string, ParseAction > actions = {
  { "ChartArea"          , do_ChartArea           },
  { "Margin"             , do_Margin              },
  { "Title"              , do_Title               },
  { "SubTitle"           , do_SubTitle            },
  { "SubSubTitle"        , do_SubSubTitle         },
  { "Footnote"           , do_Footnote            },
  { "Axis.X.Label"       , do_Axis_X_Label        },
  { "Axis.Y.Label"       , do_Axis_Y_Label        },
  { "Axis.X.Unit"        , do_Axis_X_Unit         },
  { "Axis.Y.Unit"        , do_Axis_Y_Unit         },
  { "Axis.X.UnitPos"     , do_Axis_X_UnitPos      },
  { "Axis.Y.UnitPos"     , do_Axis_Y_UnitPos      },
  { "Axis.X.LogScale"    , do_Axis_X_LogScale     },
  { "Axis.Y.LogScale"    , do_Axis_Y_LogScale     },
  { "Axis.X.Range"       , do_Axis_X_Range        },
  { "Axis.Y.Range"       , do_Axis_Y_Range        },
  { "Axis.X.Tick"        , do_Axis_X_Tick         },
  { "Axis.Y.Tick"        , do_Axis_Y_Tick         },
  { "Axis.X.Grid"        , do_Axis_X_Grid         },
  { "Axis.Y.Grid"        , do_Axis_Y_Grid         },
  { "Axis.X.NumberFormat", do_Axis_X_NumberFormat },
  { "Axis.Y.NumberFormat", do_Axis_Y_NumberFormat },
  { "Axis.X.NumberUnit"  , do_Axis_X_NumberUnit   },
  { "Axis.Y.NumberUnit"  , do_Axis_Y_NumberUnit   },
  { "Axis.X.MinorNumber" , do_Axis_X_MinorNumber  },
  { "Axis.Y.MinorNumber" , do_Axis_Y_MinorNumber  },
  { "Axis.X.NumberPos"   , do_Axis_X_NumberPos    },
  { "Axis.Y.NumberPos"   , do_Axis_Y_NumberPos    },
  { "LegendPos"          , do_LegendPos           },
  { "Series.New"         , do_Series_New          },
  { "Series.PointSize"   , do_Series_PointSize    },
  { "Series.Style"       , do_Series_Style        },
  { "Series.Data"        , do_Series_Data         },
};

bool parse_spec( void )
{
  std::string key;
  if ( !get_key( key ) ) return false;
  auto it = actions.find( key );
  if ( it == actions.end() ) parse_err( "Unknown KEY '" + key + "'", true );
  it->second();
  return true;
}

///////////////////////////////////////////////////////////////////////////////

void parse_lines( void )
{
  cur_line = lines.begin();
  cur_col = 0;
  while ( parse_spec() ) {}
}

///////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////
