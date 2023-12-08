//
//  Copyright (C) 2023, S. Kragh
//
//  This file is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This file is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
//  See the GNU General Public License for more details.
//  <http://www.gnu.org/licenses/>.
//

#include <chart_main.h>
#include <cfenv>
#include <fstream>
#include <stdexcept>
#include <limits>
#include <unordered_map>
#include <functional>

///////////////////////////////////////////////////////////////////////////////

#define ERR( MSG_ ) \
  do \
  { \
    std::cerr << "*** ERROR: " << MSG_ << std::endl; \
    exit( 1 ); \
  } while ( 0 )

///////////////////////////////////////////////////////////////////////////////

void show_help( void )
{
  std::cout << R"EOF(
Usage: chartgen [OPTION]... [FILE]...
Generate a chart in SVG format from FILE(s) to standard output.

With no FILE, or when FILE is -, read standard input.

  -t            Output a simple template file; a good starting point.
  -T            Output a full template and documentation file.
  -h, --help    Display this help and exit.

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
# This is a template file to be used as input for the chartgen program.
#
# The file consists of a number of specifiers; any line starting with a '#' at
# column 0 is ignored; empty lines are also ignored.
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
#

# Specifies the dimensions of the core chart area where the data is graphed.
# The values are in points and should typically be around 1000. Since SVG is
# scalable these dimensions primarily determine the relative size of text
# annotations and line thicknesses; texts will appear relatively larger if the
# core chart area is small and vice versa.
ChartArea: 1200 800

# Margin around chart in points; default is 2.
Margin: 2

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

# Units should be selected and data scaled accordingly to avoid very small or
# very large numbers in the graph.
Axis.X.Unit: Mb/s
Axis.Y.Unit:
  micro
  seconds

# Position of unit text; may be Auto, Left, Right, Top/Above, or
# Bottom/Below. Not all may apply; default is Auto.
Axis.X.UnitPos: Auto
Axis.Y.UnitPos: Auto

# Min, max, and optionally where the other orthogonal axis crosses this axis.
# Auto ranging is applied if no Range specifier is given (recommended).
Axis.X.Range: 0 100 90
Axis.Y.Range: -5 25

# First number is the major tick interval and the second number is an integer
# specifying the number of minor sub-intervals per major tick. Tick intervals
# are determined automatically if no Tick specifier is given (recommended).
Axis.X.Tick: 10.0 5
Axis.Y.Tick: 1.0 0

# Turn grid lines on/off for major and minor ticks; may be On or Off.
Axis.X.Grid: On On
Axis.Y.Grid: On Off

# Position of axis numbers; may be Auto, Left, Right, Top/Above, or
# Bottom/Below. Not all may apply; default is Auto. Very large or very small
# numbers will be hard to read, so it is a very good idea to scale the data and
# use suitable units.
Axis.X.NumberPos: Auto
Axis.Y.NumberPos: Auto

# May be Auto, Left, Right, or Below. Legends for the series are normally
# (Auto) placed somewhere inside the core chart area, but if this gets too
# cluttered you may place the series legends outside the core chart area.
LegendPos: Auto

# Each new series must start with this specifier giving the name of the series
# (may be multi-line). The following Series specifiers associate to this.
Series.New: Name of series

# The style of the X/Y line graph. The style is a number in the range from 0 to
# 63; if no Style specifier is given it is assigned an incrementing number.
#  0 to  7: Solid line using 8 different colors
#  8 to 15: Same as 0 to 7 but with short dashed line
# 16 to 23: Same as 0 to 7 but with medium dashed line
# 24 to 31: Same as 0 to 7 but with long dashed line
# 32 to 63: Same as 0 to 31, but with thinner line
Series.Style: 4

# These are the X/Y values for the series
Series.Data:
        0       23.7
        7.0     2.3
        23      -2e-1
        47      10.0
        71      -4.3
        97      14

# EOF
)EOF";
  } else {
    std::cout << R"EOF(# Simple template
Title       : Chart Title
Axis.X.Label: X-Axis
Axis.Y.Label: Y-Axis
Axis.X.Unit : missing unit
Axis.Y.Unit : missing unit
Series.New  : Name of series
Series.Data :
        0       23.7
        7.0     2.3
        23      -2e-1
        47      10.0
        71      -4.3
        97      14
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
Chart::Series* series = NULL;

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
  if ( id == "Auto"   ) pos = Chart::Auto  ; else
  if ( id == "Center" ) pos = Chart::Center; else
  if ( id == "Left"   ) pos = Chart::Left  ; else
  if ( id == "Right"  ) pos = Chart::Right ; else
  if ( id == "Top"    ) pos = Chart::Top   ; else
  if ( id == "Bottom" ) pos = Chart::Bottom; else
  if ( id == "Above"  ) pos = Chart::Above ; else
  if ( id == "Below"  ) pos = Chart::Below ; else
  if ( id == "" ) parse_err( "position expected" ); else
  parse_err( "unknown position '" + id + "'", true );
}

//-----------------------------------------------------------------------------

void do_ChartArea( void )
{
  int64_t w;
  int64_t h;

  skip_ws();
  if ( at_eol() ) parse_err( "width expected" );
  if ( !get_int64( w ) ) parse_err( "malformed width" );
  if ( w < 100 || w > 10000 ) parse_err( "width out of range", true );

  expect_ws( "height expected" );
  if ( !get_int64( h ) ) parse_err( "malformed height" );
  if ( h < 100 || h > 10000 ) parse_err( "height out of range", true );

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
  std::string id;

  skip_ws();
  if ( at_eol() ) parse_err( "major grid expected" );
  id = get_identifier( true );
  if ( id == "On"  ) major = true ; else
  if ( id == "Off" ) major = false; else
  parse_err( "malformed major grid '" + id + "'", true );

  expect_ws( "minor grid expected" );
  id = get_identifier( true );
  if ( id == "On"  ) minor = true ; else
  if ( id == "Off" ) minor = false; else
  parse_err( "malformed minor grid '" + id + "'", true );

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

void do_Series_New( void )
{
  std::string txt;
  get_text( txt, true );
  series = chart.AddSeries( txt );
}

void do_Series_Style( void )
{
  if ( series == NULL ) series = chart.AddSeries( "" );
  int64_t style;
  skip_ws();
  if ( at_eol() ) parse_err( "style expected" );
  if ( !get_int64( style ) ) parse_err( "malformed style" );
  if ( style < 0 || style > 63 ) parse_err( "style out of range", true );
  expect_eol();
  series->SetStyle( style );
}

void do_Series_Data( void )
{
  if ( series == NULL ) series = chart.AddSeries( "" );
  expect_eol();
  next_line();
  while ( !at_eof() ) {
    skip_ws( true );
    cur_col = 0;
    if ( !at_ws() ) break;
    double x;
    double y;
    expect_ws( "x-value expected" );
    if ( !get_double( x ) ) parse_err( "malformed x-value" );
    expect_ws( "y-value expected" );
    if ( !get_double( y ) ) parse_err( "malformed y-value" );
    expect_eol();
    series->Add( x, y );
  }
  series = NULL;
}

///////////////////////////////////////////////////////////////////////////////

using ParseAction = std::function<void()>;

std::unordered_map< std::string, ParseAction > actions = {
  { "ChartArea"       , do_ChartArea        },
  { "Margin"          , do_Margin           },
  { "Title"           , do_Title            },
  { "SubTitle"        , do_SubTitle         },
  { "SubSubTitle"     , do_SubSubTitle      },
  { "Footnote"        , do_Footnote         },
  { "Axis.X.Label"    , do_Axis_X_Label     },
  { "Axis.Y.Label"    , do_Axis_Y_Label     },
  { "Axis.X.Unit"     , do_Axis_X_Unit      },
  { "Axis.Y.Unit"     , do_Axis_Y_Unit      },
  { "Axis.X.UnitPos"  , do_Axis_X_UnitPos   },
  { "Axis.Y.UnitPos"  , do_Axis_Y_UnitPos   },
  { "Axis.X.Range"    , do_Axis_X_Range     },
  { "Axis.Y.Range"    , do_Axis_Y_Range     },
  { "Axis.X.Tick"     , do_Axis_X_Tick      },
  { "Axis.Y.Tick"     , do_Axis_Y_Tick      },
  { "Axis.X.Grid"     , do_Axis_X_Grid      },
  { "Axis.Y.Grid"     , do_Axis_Y_Grid      },
  { "Axis.X.NumberPos", do_Axis_X_NumberPos },
  { "Axis.Y.NumberPos", do_Axis_Y_NumberPos },
  { "LegendPos"       , do_LegendPos        },
  { "Series.New"      , do_Series_New       },
  { "Series.Style"    , do_Series_Style     },
  { "Series.Data"     , do_Series_Data      },
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

int main( int argc, char* argv[] )
{
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

  SVG::Canvas* canvas = chart.Build();
  std::cout << canvas->GenSVG();
  delete canvas;

  return 0;
}

///////////////////////////////////////////////////////////////////////////////
