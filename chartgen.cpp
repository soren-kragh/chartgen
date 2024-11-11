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
  -eN               Output example N; good for inspiration.
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
    #include <dash_uc_t.h>
  } else {
    #include <dash_t.h>
  }
  return;
}

////////////////////////////////////////////////////////////////////////////////

void gen_example( int N )
{
  switch ( N ) {
    case 1:
      #include <dash_e1.h>
      break;
  }
  return;
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
std::vector< Chart::SeriesType > type_list;

bool defining_series = false;
Chart::SeriesType series_type = Chart::SeriesType::XY;
int32_t category_idx = 0;
int axis_y_n = 0;
double series_base = 0;
Chart::MarkerShape marker_shape = Chart::MarkerShape::Circle;
double marker_size = -1;
double line_width = -1;
double line_dash = -1;
double line_hole = -1;
int64_t style = 0;

//------------------------------------------------------------------------------

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
bool get_double( double& d, bool none_allowed = false )
{
  id_col = cur_col;
  if ( none_allowed && get_char() == '-' ) {
    char c = get_char( false );
    if ( is_ws( c ) || c == '#' ) {
      d = 100e300;
      return true;
    }
  }
  cur_col = id_col;
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

void do_Color(
  SVG::Color* color
)
{
  skip_ws();
  std::string color_id = get_identifier( true );
  uint8_t r, g, b;
  double lighten = 0.0;
  double transparency = 0.0;

  bool color_ok = true;

  if ( color_id == "None" ) {
    color->Clear();
  } else {
    if (color_id.size() != 7 || color_id[0] != '#') {
      color_ok = false;
    }
    for ( char c : color_id.substr( 1 ) ) {
      if ( !std::isxdigit( c ) ) color_ok = false;
    }
    if ( color_ok ) {
      r = static_cast<uint8_t>( std::stoi( color_id.substr(1, 2), nullptr, 16) );
      g = static_cast<uint8_t>( std::stoi( color_id.substr(3, 2), nullptr, 16) );
      b = static_cast<uint8_t>( std::stoi( color_id.substr(5, 2), nullptr, 16) );
      color->Set( r, g, b );
    } else {
      color_ok = color->Set( color_id ) == color;
    }
  }

  if ( !color_ok ) {
    parse_err( "invalid color", true );
  }

  if ( !at_eol() ) {
    expect_ws();
    if ( !at_eol() ) {
      if ( !get_double( lighten ) ) {
        parse_err( "malformed lighten value" );
      }
      if ( lighten < -1.0 || lighten > 1.0 ) {
        parse_err( "lighten value out of range [-1.0;1.0]", true );
      }
      if ( lighten < 0 )
        color->Darken( -lighten );
      else
        color->Lighten( lighten );
    }
  }

  if ( !at_eol() ) {
    expect_ws();
    if ( !at_eol() ) {
      if ( !get_double( transparency ) ) {
        parse_err( "malformed transparency value" );
      }
      if ( transparency < 0.0 || transparency > 1.0 ) {
        parse_err( "transparency value out of range [0.0;1.0]", true );
      }
      color->SetTransparency( transparency );
    }
  }

  expect_eol();
}

//-----------------------------------------------------------------------------

void do_ChartArea( void )
{
  int64_t w;
  int64_t h;

  skip_ws();
  if ( at_eol() ) parse_err( "width expected" );
  if ( !get_int64( w ) ) parse_err( "malformed width" );
  if ( w < 100 || w > 100000 ) {
    parse_err( "width out of range [100;100000]", true );
  }

  expect_ws( "height expected" );
  if ( !get_int64( h ) ) parse_err( "malformed height" );
  if ( h < 100 || h > 100000 ) {
    parse_err( "height out of range [100;100000]", true );
  }

  expect_eol();
  chart.SetChartArea( w, h );
}

void do_Margin( void )
{
  int64_t m;
  skip_ws();
  if ( at_eol() ) parse_err( "margin expected" );
  if ( !get_int64( m ) ) parse_err( "malformed margin" );
  if ( m < 0 || m > 1000 ) {
    parse_err( "margin out of range [0;1000]", true );
  }
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
  }

  expect_eol();

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
  if ( minor < 0 || minor > 100 ) {
    parse_err( "minor tick out of range [0;100]", true );
  }

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

void NextSeriesStyle( void )
{
  style = (style + 1) % 80;
}

void ApplyMarkerSize( Chart::Series* series )
{
  if ( marker_size >= 0 ) {
    if (
      marker_size == 0 &&
      ( series_type == Chart::SeriesType::Scatter ||
        series_type == Chart::SeriesType::Point
      )
    ) {
      series_list.back()->SetMarkerSize( 12 );
    } else {
      series_list.back()->SetMarkerSize( marker_size );
    }
  }
}

void AddSeries( std::string name = "" )
{
  type_list.push_back( series_type );
  series_list.push_back( chart.AddSeries( series_type ) );
  series_list.back()->SetName( name );
  series_list.back()->SetAxisY( axis_y_n );
  series_list.back()->SetBase( series_base );
  series_list.back()->SetStyle( style );
  NextSeriesStyle();
  series_list.back()->SetMarkerShape( marker_shape );
  ApplyMarkerSize( series_list.back() );
  if ( line_width >= 0 ) {
    series_list.back()->SetLineWidth( line_width );
  }
  if ( line_dash >= 0 ) {
    series_list.back()->SetLineDash( line_dash, line_hole );
  }
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
  if ( id == "XY"          ) series_type = Chart::SeriesType::XY         ; else
  if ( id == "Scatter"     ) series_type = Chart::SeriesType::Scatter    ; else
  if ( id == "Line"        ) series_type = Chart::SeriesType::Line       ; else
  if ( id == "Point"       ) series_type = Chart::SeriesType::Point      ; else
  if ( id == "Lollipop"    ) series_type = Chart::SeriesType::Lollipop   ; else
  if ( id == "Bar"         ) series_type = Chart::SeriesType::Bar        ; else
  if ( id == "StackedBar"  ) series_type = Chart::SeriesType::StackedBar ; else
  if ( id == "Area"        ) series_type = Chart::SeriesType::Area       ; else
  if ( id == "StackedArea" ) series_type = Chart::SeriesType::StackedArea; else
  if ( id == "" ) parse_err( "series type expected" ); else
  parse_err( "unknown series type '" + id + "'", true );
  expect_eol();
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

void do_Series_Base( void )
{
  skip_ws();
  if ( at_eol() ) parse_err( "base expected" );
  if ( !get_double( series_base ) ) parse_err( "malformed base" );
  expect_eol();
  if ( defining_series ) {
    series_list.back()->SetBase( series_base );
  }
}

void do_Series_MarkerSize( void )
{
  skip_ws();
  if ( at_eol() ) parse_err( "marker size expected" );
  if ( !get_double( marker_size ) ) parse_err( "malformed marker size" );
  if ( marker_size < 0 || marker_size > 100 ) {
    parse_err( "marker size out of range [0;100]", true );
  }
  expect_eol();
  if ( defining_series ) {
    ApplyMarkerSize( series_list.back() );
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
  if ( style < 0 || style > 79 ) {
    parse_err( "style out of range [0;79]", true );
  }
  expect_eol();
  if ( defining_series ) {
    series_list.back()->SetStyle( style );
    NextSeriesStyle();
    marker_size = -1;
    line_width = -1;
    line_dash = -1;
    line_hole = -1;
  }
}

void do_Series_LineWidth( void )
{
  skip_ws();
  if ( at_eol() ) parse_err( "line width expected" );
  if ( !get_double( line_width ) ) parse_err( "malformed line width" );
  if ( line_width < 0 || line_width > 100 ) {
    parse_err( "line width out of range [0;100]", true );
  }
  expect_eol();
  if ( defining_series ) {
    series_list.back()->SetLineWidth( line_width );
  }
}

void do_Series_LineDash( void )
{
  line_dash = 0;
  skip_ws();
  if ( at_eol() ) parse_err( "line dash expected" );
  if ( !get_double( line_dash ) ) {
    parse_err( "malformed line dash" );
  }
  if ( line_dash < 0 || line_dash > 100 ) {
    parse_err( "line dash out of range [0;100]", true );
  }
  line_hole = line_dash;
  if ( !at_eol() ) {
    expect_ws();
    if ( !at_eol() ) {
      if ( !get_double( line_hole ) ) {
        parse_err( "malformed line hole" );
      }
      if ( line_hole < 0 || line_hole > 100 ) {
        parse_err( "line hole out of range [0;100]", true );
      }
    }
  }
  expect_eol();
  if ( defining_series ) {
    series_list.back()->SetLineDash( line_dash, line_hole );
  }
}

void do_Series_LineColor( void )
{
  if ( !defining_series ) {
    parse_err( "LineColor outside defining series" );
  }
  do_Color( series_list.back()->LineColor() );
}

void do_Series_FillColor( void )
{
  if ( !defining_series ) {
    parse_err( "FillColor outside defining series" );
  }
  do_Color( series_list.back()->FillColor() );
}

void do_Series_Data( void )
{
  expect_eol();
  next_line();

  // Pre-parse first line in order to determine number of Y-values.
  uint32_t y_values = 0;
  skip_ws( true );
  cur_col = 0;
  while ( at_ws() ) {
    skip_ws();
    if ( at_eol() ) break;
    while ( !at_eol() && !at_ws() ) cur_col++;
    y_values++;
  }
  if ( y_values > 0 ) y_values--;
  cur_col = 0;

  // Auto-add new series if needed.
  for ( uint32_t i = 0; i < y_values; i++ ) {
    if (
      series_list.size() == i ||
      series_list[ series_list.size() - i - 1 ]->Size() > 0
    )
      AddSeries();
  }

  // Detect series types.
  bool x_is_num = false;
  bool x_is_txt = false;
  for ( uint32_t i = 0; i < y_values; i++ ) {
    auto t = type_list[ type_list.size() - i - 1 ];
    if (
      t == Chart::SeriesType::XY ||
      t == Chart::SeriesType::Scatter
    )
      x_is_num = true;
    else
      x_is_txt = true;
  }
  if ( x_is_num && x_is_txt ) {
    parse_err( "cannot mix XY/Scatter series types with other series types" );
  }

  std::string category;
  while ( !at_eof() ) {
    skip_ws( true );
    cur_col = 0;
    if ( !at_ws() ) break;
    expect_ws( "x-value expected" );
    double x;
    if ( x_is_txt ) {
      if ( !get_category( category ) ) parse_err( "malformed x-value" );
      chart.AddCategory( category );
      x = category_idx;
      category_idx++;
    } else {
      if ( !get_double( x ) ) parse_err( "malformed x-value" );
    }
    uint32_t n = 0;
    while ( true ) {
      expect_ws( "y-value expected" );
      auto old_col = cur_col;
      double y;
      if ( !get_double( y, true ) ) parse_err( "malformed y-value" );
      if ( !at_ws() && !at_eol() ) parse_err( "syntax error" );
      if ( n >= y_values ) {
        cur_col = old_col;
        parse_err( "too many y-values" );
      }
      series_list[ series_list.size() - y_values + n ]->Add( x, y );
      n++;
      old_col = cur_col;
      skip_ws();
      if ( at_eol() ) break;
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
  { "Series.Base"       , do_Series_Base        },
  { "Series.MarkerSize" , do_Series_MarkerSize  },
  { "Series.MarkerShape", do_Series_MarkerShape },
  { "Series.Style"      , do_Series_Style       },
  { "Series.LineWidth"  , do_Series_LineWidth   },
  { "Series.LineDash"   , do_Series_LineDash    },
  { "Series.LineColor"  , do_Series_LineColor   },
  { "Series.FillColor"  , do_Series_FillColor   },
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
    g->Last()->Attr()->SetLineWidth( 10 )->FillColor()->Set( SVG::ColorName::tomato );
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
      if ( a == "-e1" ) {
        gen_example( 1 );
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
