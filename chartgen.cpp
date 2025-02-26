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
#include <random>

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
    {
      #include <dash_e1.h>
      break;
    }
    case 2:
    {
      #include <dash_e2.h>
      break;
    }
    case 3:
    {
      std::random_device rd{};
      std::mt19937 gen{ rd() };
      std::normal_distribution< double > md{ 0.0, 1.0 };
      std::uniform_real_distribution< double > ad{ 0.0, 2 * M_PI };
      #include <dash_e3.h>
      int samples = 10000;
      while ( samples > 0 ) {
        double m = md(gen) / 2;
        double a = ad(gen);
        double x = m * std::cos( a );
        double y = m * std::sin( a );
        if ( std::abs( m ) <= 1.0 ) {
          std::cout << ' ' << x << ' ' << y << '\n';
          samples--;
        }
      }
      break;
    }
    case 4:
    {
      #include <dash_e4.h>
      break;
    }
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
bool series_type_defined = false;
Chart::SeriesType series_type = Chart::SeriesType::XY;
int32_t category_idx = 0;
int axis_y_n = 0;
double series_base = 0;

int64_t style = 0;
Chart::MarkerShape marker_shape = Chart::MarkerShape::Circle;
double marker_size = -1;
double line_width = -1;
double line_dash = -1;
double line_hole = -1;
double lighten = 0.0;
double fill_transparency = -1;
bool tag_enable = false;
Chart::Pos tag_pos = Chart::Pos::Auto;
double tag_size = 1.0;
bool tag_box = false;
SVG::Color tag_text_color;
SVG::Color tag_fill_color;
SVG::Color tag_line_color;

//------------------------------------------------------------------------------

std::vector< std::string > file_names;

struct LineRec {
  std::string line;
  size_t      line_number;
  uint32_t    file_name_idx;
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
      << file_names[ cur_line->file_name_idx ] << " ("
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
  if ( at_eol() ) return '\r';
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
  if ( none_allowed ) {
    char c = get_char();
    if ( c == '-' || c == '!' ) {
      char nc = get_char( false );
      if ( is_ws( nc ) ) {
        d = (c == '!') ? Chart::num_invalid : Chart::num_skip;
        return true;
      }
    }
  }
  cur_col = id_col;
  bool success = false;
  try {
    std::string str = cur_line->line.substr( cur_col );
    str.push_back( ' ' );
    std::istringstream iss( str );
    if ( !(iss >> d).fail() ) {
      cur_col += iss.tellg();
      success = true;
    }
  } catch ( const std::exception& e ) {
    success = false;
  }
  if ( success ) {
    if ( std::abs( d ) > Chart::num_hi ) {
      parse_err( "number too big", true );
    }
  }
  return success;
}

// Read in a text based X-value which defines a category for the series.
bool get_category( std::string& t, bool& quoted )
{
  t.clear();
  quoted = false;
  id_col = cur_col;
  bool in_quote = false;
  while ( !at_eol() ) {
    char c = cur_line->line[ cur_col ];
    if ( c == '"' ) {
      if ( cur_col == id_col ) {
        in_quote = true;
        cur_col++;
        continue;
      } else {
        if ( in_quote ) {
          in_quote = false;
          quoted = true;
          cur_col++;
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
  if ( in_quote ) return false;
  if ( !quoted && t == "-" ) t.clear();
  return true;
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
  Chart::Pos& pos, int& axis_y_n
)
{
  std::string id = get_identifier( true );
  if ( id == "Auto"    ) pos = Chart::Pos::Auto  ; else
  if ( id == "Center"  ) pos = Chart::Pos::Center; else
  if ( id == "Left"    ) pos = Chart::Pos::Left  ; else
  if ( id == "Right"   ) pos = Chart::Pos::Right ; else
  if ( id == "Top"     ) pos = Chart::Pos::Top   ; else
  if ( id == "Bottom"  ) pos = Chart::Pos::Bottom; else
  if ( id == "Above"   ) pos = Chart::Pos::Top   ; else
  if ( id == "Below"   ) pos = Chart::Pos::Bottom; else
  if ( id == "Base"    ) { pos = Chart::Pos::Base; axis_y_n = 0; } else
  if ( id == "BasePri" ) { pos = Chart::Pos::Base; axis_y_n = 0; } else
  if ( id == "BaseSec" ) { pos = Chart::Pos::Base; axis_y_n = 1; } else
  if ( id == "End"     ) pos = Chart::Pos::End   ; else
  if ( id == "Beyond"  ) pos = Chart::Pos::Beyond; else
  if ( id == "" ) parse_err( "position expected" ); else
  parse_err( "unknown position '" + id + "'", true );
}

void do_Pos(
  Chart::Pos& pos
)
{
  int axis_y_n;
  do_Pos( pos, axis_y_n );
}

void do_Switch(
  bool& flag
)
{
  skip_ws();
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

  bool color_ok = true;

  color->Clear();
  color->SetTransparency( 0.0 );

  if ( color_id != "None" ) {
    if (color_id.size() != 7 || color_id[0] != '#') {
      color_ok = false;
    }
    for ( char c : color_id.substr( 1 ) ) {
      if ( !std::isxdigit( c ) ) color_ok = false;
    }
    if ( color_ok ) {
      uint8_t r = static_cast<uint8_t>( std::stoi( color_id.substr(1, 2), nullptr, 16) );
      uint8_t g = static_cast<uint8_t>( std::stoi( color_id.substr(3, 2), nullptr, 16) );
      uint8_t b = static_cast<uint8_t>( std::stoi( color_id.substr(5, 2), nullptr, 16) );
      color->Set( r, g, b );
    } else {
      color_ok = color->Set( color_id ) == color;
    }
  }

  if ( !color_ok ) {
    parse_err( "invalid color", true );
  }

  if ( !at_eol() ) {
    double lighten = 0.0;
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
    double transparency = 0.0;
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

//------------------------------------------------------------------------------

void do_FrameWidth( void )
{
  int64_t m;
  skip_ws();
  if ( at_eol() ) parse_err( "frame width expected" );
  if ( !get_int64( m ) ) parse_err( "malformed frame width" );
  if ( m < 0 || m > 1000 ) {
    parse_err( "frame width out of range [0;1000]", true );
  }
  expect_eol();
  chart.SetFrameWidth( m );
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

void do_ChartBox( void )
{
  bool chart_box;
  do_Switch( chart_box );
  expect_eol();
  chart.SetChartBox( chart_box );
}

//------------------------------------------------------------------------------

void do_FrameColor( void )
{
  do_Color( chart.FrameColor() );
}

void do_BackgroundColor( void )
{
  do_Color( chart.BackgroundColor() );
}

void do_ChartAreaColor( void )
{
  do_Color( chart.ChartAreaColor() );
}

void do_AxisColor( void )
{
  do_Color( chart.AxisColor() );
}

void do_GridColor( void )
{
  do_Color( chart.AxisX()->GridColor() );
  for ( auto n : { 0, 1 } ) {
    chart.AxisY( n )->GridColor()->Set( chart.AxisX()->GridColor() );
  }
}

void do_TextColor( void )
{
  do_Color( chart.TextColor() );
}

//------------------------------------------------------------------------------

void do_LetterSpacing( void )
{
  double width_adj    = 1.0;
  double height_adj   = 1.0;
  double baseline_adj = 1.0;

  skip_ws();
  if ( at_eol() ) parse_err( "width adjustment expected" );
  if ( !get_double( width_adj ) ) {
    parse_err( "malformed width adjustment" );
  }
  if ( width_adj < 0 || width_adj > 100 ) {
    parse_err( "width adjustment out of range [0;100]", true );
  }

  if ( !at_eol() ) {
    expect_ws();
    if ( !at_eol() ) {
      if ( !get_double( height_adj ) ) {
        parse_err( "malformed height adjustment" );
      }
      if ( height_adj < 0 || height_adj > 100 ) {
        parse_err( "height adjustment out of range [0;100]", true );
      }
    }
  }

  if ( !at_eol() ) {
    expect_ws();
    if ( !at_eol() ) {
      if ( !get_double( baseline_adj ) ) {
        parse_err( "malformed baseline adjustment" );
      }
      if ( baseline_adj < 0 || baseline_adj > 100 ) {
        parse_err( "baseline adjustment out of range [0;100]", true );
      }
    }
  }

  expect_eol();

  chart.SetLetterSpacing( width_adj, height_adj, baseline_adj );
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

void do_TitlePos( void )
{
  Chart::Pos pos_x;
  Chart::Pos pos_y = Chart::Pos::Top;

  skip_ws();
  do_Pos( pos_x );

  if ( !at_eol() ) {
    expect_ws();
    if ( !at_eol() ) {
      do_Pos( pos_y );
    }
  }

  expect_eol();
  chart.SetTitlePos( pos_x, pos_y );
}

void do_TitleInside( void )
{
  bool inside;
  do_Switch( inside );
  expect_eol();
  chart.SetTitleInside( inside );
}

void do_Footnote( void )
{
  std::string txt;
  get_text( txt, true );
  chart.AddFootnote( txt );
}

void do_FootnotePos( void )
{
  Chart::Pos pos;
  skip_ws();
  do_Pos( pos );
  expect_eol();
  chart.SetFootnotePos( pos );
}

void do_FootnoteLine( void )
{
  bool footnote_line;
  do_Switch( footnote_line );
  expect_eol();
  chart.SetFootnoteLine( footnote_line );
}

//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------

void do_Axis_Reverse( Chart::Axis* axis )
{
  bool reverse;
  do_Switch( reverse );
  expect_eol();
  axis->SetReverse( reverse );
}

//------------------------------------------------------------------------------

void do_Axis_Style( Chart::Axis* axis )
{
  Chart::AxisStyle style;
  skip_ws();

  std::string id = get_identifier( true );
  if ( id == "Auto"   ) style = Chart::AxisStyle::Auto ; else
  if ( id == "None"   ) style = Chart::AxisStyle::None ; else
  if ( id == "Line"   ) style = Chart::AxisStyle::Line ; else
  if ( id == "Arrow"  ) style = Chart::AxisStyle::Arrow; else
  if ( id == "Edge"   ) style = Chart::AxisStyle::Edge ; else
  if ( id == "" ) parse_err( "axis style expected" ); else
  parse_err( "unknown axis style '" + id + "'", true );

  expect_eol();
  axis->SetStyle( style );
}

//------------------------------------------------------------------------------

void do_Axis_Label( Chart::Axis* axis )
{
  std::string txt;
  get_text( txt, true );
  axis->SetLabel( txt );
}

//------------------------------------------------------------------------------

void do_Axis_SubLabel( Chart::Axis* axis )
{
  std::string txt;
  get_text( txt, true );
  axis->SetSubLabel( txt );
}

//------------------------------------------------------------------------------

void do_Axis_Unit( Chart::Axis* axis )
{
  std::string txt;
  get_text( txt, true );
  axis->SetUnit( txt );
}

//------------------------------------------------------------------------------

void do_Axis_UnitPos( Chart::Axis* axis )
{
  Chart::Pos pos;
  skip_ws();
  do_Pos( pos );
  expect_eol();
  axis->SetUnitPos( pos );
}

//------------------------------------------------------------------------------

void do_Axis_LogScale( Chart::Axis* axis )
{
  bool log_scale;
  do_Switch( log_scale );
  expect_eol();
  axis->SetLogScale( log_scale );
}

//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------

void do_Axis_Pos( Chart::Axis* axis )
{
  Chart::Pos pos;
  int axis_y_n;
  skip_ws();
  do_Pos( pos, axis_y_n );
  expect_eol();
  axis->SetPos( pos, axis_y_n );
}

//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------

void do_Axis_Grid( Chart::Axis* axis )
{
  bool major;
  bool minor;

  do_Switch( major );

  minor = major;

  if ( !at_eol() ) {
    expect_ws();
    if ( !at_eol() ) {
      do_Switch( minor );
    }
  }

  expect_eol();

  axis->SetGrid( major, minor );
}

//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------

void do_Axis_GridColor( Chart::Axis* axis )
{
  do_Color( axis->GridColor() );
}

//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------

void do_Axis_NumberSign( Chart::Axis* axis )
{
  bool number_sign;
  do_Switch( number_sign );
  expect_eol();
  axis->SetNumberSign( number_sign );
}

//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------

void do_Axis_MinorNumber( Chart::Axis* axis )
{
  bool minor_num;
  do_Switch( minor_num );
  expect_eol();
  axis->ShowMinorNumbers( minor_num );
}

//------------------------------------------------------------------------------

void do_Axis_NumberPos( Chart::Axis* axis )
{
  Chart::Pos pos;
  skip_ws();
  do_Pos( pos );
  expect_eol();
  axis->SetNumberPos( pos );
}

//------------------------------------------------------------------------------

void do_Axis_NumberSize( Chart::Axis* axis )
{
  double size;
  skip_ws();
  if ( at_eol() ) parse_err( "number size value expected" );
  if ( !get_double( size ) ) {
    parse_err( "malformed number size value" );
  }
  if ( size < 0.01 || size > 100 ) {
    parse_err( "number size value out of range", true );
  }
  expect_eol();
  axis->SetNumberSize( size );
}

//------------------------------------------------------------------------------

void do_LegendPos( void )
{
  Chart::Pos pos;
  skip_ws();
  do_Pos( pos );
  expect_eol();
  chart.SetLegendPos( pos );
}

void do_LegendColor( void )
{
  do_Color( chart.LegendColor() );
}

void do_LegendOutline( void )
{
  bool outline;
  do_Switch( outline );
  expect_eol();
  chart.SetLegendOutline( outline );
}

//------------------------------------------------------------------------------

void do_BarWidth( void )
{
  double one_width;
  double all_width;

  skip_ws();
  if ( at_eol() ) parse_err( "width expected" );
  if ( !get_double( one_width ) ) parse_err( "malformed width" );
  if ( one_width <= 0.0 || one_width > 1.0 ) parse_err( "invalid width", true );

  all_width = 1.0;
  if ( !at_eol() ) {
    expect_ws();
    if ( !at_eol() ) {
      if ( !get_double( all_width ) ) parse_err( "malformed width" );
      if ( all_width <= 0.0 || all_width > 1.0 ) parse_err( "invalid width", true );
    }
  }

  expect_eol();

  chart.SetBarWidth( one_width, all_width );
}

void do_BarMargin( void )
{
  double margin;

  skip_ws();
  if ( at_eol() ) parse_err( "margin expected" );
  if ( !get_double( margin ) ) parse_err( "malformed margin" );
  if ( margin < 0.0 ) parse_err( "invalid margin", true );

  expect_eol();

  chart.SetBarMargin( margin );
}

//------------------------------------------------------------------------------

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
  if ( !series_type_defined ) {
    parse_err( "undefined SeriesType" );
  }
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
  if ( fill_transparency >= 0 ) {
    series_list.back()->FillColor()->SetTransparency( fill_transparency );
  }
  series_list.back()->LineColor()->Lighten( lighten );
  series_list.back()->FillColor()->Lighten( lighten );
  series_list.back()->SetTagEnable( tag_enable );
  series_list.back()->SetTagPos( tag_pos );
  series_list.back()->SetTagSize( tag_size );
  series_list.back()->SetTagBox( tag_box );
  series_list.back()->TagTextColor()->Set( &tag_text_color );
  series_list.back()->TagFillColor()->Set( &tag_fill_color );
  series_list.back()->TagLineColor()->Set( &tag_line_color );
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
  series_type_defined = true;
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
    series_list.back()->LineColor()->Lighten( lighten );
    series_list.back()->FillColor()->Lighten( lighten );
    series_list.back()->TagTextColor()->Undef();
    series_list.back()->TagFillColor()->Undef();
    series_list.back()->TagLineColor()->Undef();
  }
  marker_size = -1;
  line_width = -1;
  line_dash = -1;
  line_hole = -1;
  fill_transparency = -1;
  tag_text_color.Undef();
  tag_fill_color.Undef();
  tag_line_color.Undef();
}

void do_Series_MarkerShape( void )
{
  skip_ws();
  std::string id = get_identifier( true );
  if ( id == "Circle"      ) marker_shape = Chart::MarkerShape::Circle     ; else
  if ( id == "Square"      ) marker_shape = Chart::MarkerShape::Square     ; else
  if ( id == "Triangle"    ) marker_shape = Chart::MarkerShape::Triangle   ; else
  if ( id == "InvTriangle" ) marker_shape = Chart::MarkerShape::InvTriangle; else
  if ( id == "Diamond"     ) marker_shape = Chart::MarkerShape::Diamond    ; else
  if ( id == "Cross"       ) marker_shape = Chart::MarkerShape::Cross      ; else
  if ( id == "HorLine"     ) marker_shape = Chart::MarkerShape::HorLine    ; else
  if ( id == "VerLine"     ) marker_shape = Chart::MarkerShape::VerLine    ; else
  if ( id == "" ) parse_err( "marker shape expected" ); else
  parse_err( "unknown marker shape '" + id + "'", true );
  expect_eol();
  if ( defining_series ) {
    series_list.back()->SetMarkerShape( marker_shape );
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

void do_Series_Lighten( void )
{
  skip_ws();
  if ( at_eol() ) parse_err( "lighten value expected" );
  if ( !get_double( lighten ) ) {
    parse_err( "malformed lighten value" );
  }
  if ( lighten < -1.0 || lighten > +1.0 ) {
    parse_err( "lighten value out of range [-1.0;1.0]", true );
  }
  expect_eol();
  if ( defining_series ) {
    series_list.back()->LineColor()->Lighten( lighten );
    series_list.back()->FillColor()->Lighten( lighten );
  }
}

void do_Series_FillTransparency( void )
{
  skip_ws();
  if ( at_eol() ) parse_err( "transparency value expected" );
  if ( !get_double( fill_transparency ) ) {
    parse_err( "malformed transparency value" );
  }
  if ( fill_transparency < 0.0 || fill_transparency > 1.0 ) {
    parse_err( "transparency value out of range [-1.0;1.0]", true );
  }
  expect_eol();
  if ( defining_series ) {
    series_list.back()->FillColor()->SetTransparency( fill_transparency );
  }
}

void do_Series_LineColor( void )
{
  if ( !defining_series ) {
    parse_err( "LineColor outside defining series" );
  }
  do_Color( series_list.back()->LineColor() );
  series_list.back()->LineColor()->Lighten( lighten );
}

void do_Series_FillColor( void )
{
  if ( !defining_series ) {
    parse_err( "FillColor outside defining series" );
  }
  do_Color( series_list.back()->FillColor() );
  series_list.back()->FillColor()->Lighten( lighten );
  if ( fill_transparency >= 0 ) {
    series_list.back()->FillColor()->SetTransparency( fill_transparency );
  }
}

//------------------------------------------------------------------------------

void do_Series_Tag( void )
{
  do_Switch( tag_enable );
  expect_eol();
  if ( defining_series ) {
    series_list.back()->SetTagEnable( tag_enable );
  }
}

void do_Series_TagPos( void )
{
  skip_ws();
  do_Pos( tag_pos );
  expect_eol();
  if ( defining_series ) {
    series_list.back()->SetTagPos( tag_pos );
  }
}

void do_Series_TagSize( void )
{
  skip_ws();
  if ( at_eol() ) parse_err( "tag size value expected" );
  if ( !get_double( tag_size ) ) {
    parse_err( "malformed tag size value" );
  }
  if ( tag_size < 0.01 || tag_size > 100 ) {
    parse_err( "tag size value out of range", true );
  }
  expect_eol();
  if ( defining_series ) {
    series_list.back()->SetTagSize( tag_size );
  }
}

void do_Series_TagBox( void )
{
  do_Switch( tag_box );
  expect_eol();
  if ( defining_series ) {
    series_list.back()->SetTagBox( tag_box );
  }
}

void do_Series_TagTextColor( void )
{
  do_Color( &tag_text_color );
  if ( defining_series ) {
    series_list.back()->TagTextColor()->Set( &tag_text_color );
  }
}

void do_Series_TagFillColor( void )
{
  do_Color( &tag_fill_color );
  if ( defining_series ) {
    series_list.back()->TagFillColor()->Set( &tag_fill_color );
  }
}

void do_Series_TagLineColor( void )
{
  do_Color( &tag_line_color );
  if ( defining_series ) {
    series_list.back()->TagLineColor()->Set( &tag_line_color );
  }
}

//------------------------------------------------------------------------------

void parse_series_data( void )
{
  defining_series = false;

  uint32_t y_values = 0;
  uint32_t rows = 0;
  bool no_x_value = false;

  // Do a pre-scan of all the data.
  {
    auto org_line = cur_line;
    cur_col = 0;
    bool x_is_text = false;
    while ( !at_eof() ) {
      skip_ws( true );
      if ( at_eol() ) break;
      double d;
      bool got_number = false;
      if ( get_double( d, true ) ) {
        got_number = at_eol() || at_ws();
      }
      if ( !got_number ) {
        cur_col = id_col;
        std::string t;
        bool quoted;
        if ( !get_category( t, quoted ) ) {
          parse_err( "unmatched quote", true );
        }
        if ( !quoted && !t.empty() && id_col == 0 ) {
          if ( t.back() == ':' ) break;
          auto save_col = cur_col;
          skip_ws();
          if ( get_char() == ':' ) break;
          cur_col = save_col;
        }
        x_is_text = true;
      }
      if ( !at_eol() && !at_ws() ) {
        parse_err( "syntax error" );
      }
      ++rows;
      uint32_t n = 0;
      while ( at_ws() ) {
        skip_ws();
        if ( at_eol() ) break;
        while ( !at_eol() && !at_ws() ) cur_col++;
        ++n;
      }
      y_values = std::max( y_values, n );
      expect_eol();
    }
    if ( rows == 0 ) {
      cur_col = 0;
      return;
    }
    if (
      !x_is_text && y_values == 0 &&
      ( !series_type_defined ||
        ( series_type != Chart::SeriesType::XY &&
          series_type != Chart::SeriesType::Scatter
        )
      )
    ) {
      x_is_text = true;
      no_x_value = true;
    }
    if ( y_values == 0 ) y_values = 1;
    if ( !series_type_defined ) {
      series_type = x_is_text ? Chart::SeriesType::Line : Chart::SeriesType::XY;
      series_type_defined = true;
    }
    cur_line = org_line;
    cur_col = 0;
  }

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
  std::string_view tag_x;
  while ( rows-- ) {
    skip_ws( true );
    id_col = cur_col;
    if ( at_eol() ) parse_err( "X-value expected" );
    double x;
    if ( x_is_txt ) {
      if ( !no_x_value ) {
        bool quoted;
        if ( !get_category( category, quoted ) ) {
          parse_err( "unmatched quote", true );
        }
      }
      chart.AddCategory( category );
      x = category_idx;
      category_idx++;
    } else {
      if ( !get_double( x, true ) ) parse_err( "malformed X-value" );
    }
    if ( !no_x_value && !at_eol() && !at_ws() ) {
      parse_err( "syntax error" );
    }
    tag_x =
      std::string_view(
        cur_line->line
      ).substr( id_col, cur_col - id_col );
    for ( uint32_t n = 0; n < y_values; ++n ) {
      uint32_t series_idx = series_list.size() - y_values + n;
      skip_ws();
      double y;
      if ( at_eol() && x_is_txt ) {
        y = Chart::num_skip;
        series_list[ series_idx ]->Add( x, y );
      } else {
        if ( at_eol() ) parse_err( "Y-value expected" );
        if ( !get_double( y, true ) ) parse_err( "malformed Y-value" );
        if ( !at_eol() && !at_ws() ) parse_err( "syntax error" );
        series_list[ series_idx ]->Add(
          x, y,
          tag_x,
          std::string_view(
            cur_line->line
          ).substr( id_col, cur_col - id_col )
        );
      }
    }
    expect_eol();
  }

  return;
}

void do_Series_Data( void )
{
  expect_eol();
  next_line();
  parse_series_data();
}

////////////////////////////////////////////////////////////////////////////////

using ChartAction = std::function< void() >;

std::unordered_map< std::string, ChartAction > chart_actions = {
  { "FrameWidth"             , do_FrameWidth              },
  { "Margin"                 , do_Margin                  },
  { "ChartArea"              , do_ChartArea               },
  { "ChartBox"               , do_ChartBox                },
  { "FrameColor"             , do_FrameColor              },
  { "BackgroundColor"        , do_BackgroundColor         },
  { "ChartAreaColor"         , do_ChartAreaColor          },
  { "AxisColor"              , do_AxisColor               },
  { "GridColor"              , do_GridColor               },
  { "TextColor"              , do_TextColor               },
  { "LetterSpacing"          , do_LetterSpacing           },
  { "Title"                  , do_Title                   },
  { "SubTitle"               , do_SubTitle                },
  { "SubSubTitle"            , do_SubSubTitle             },
  { "TitlePos"               , do_TitlePos                },
  { "TitleInside"            , do_TitleInside             },
  { "Footnote"               , do_Footnote                },
  { "FootnotePos"            , do_FootnotePos             },
  { "FootnoteLine"           , do_FootnoteLine            },
  { "LegendPos"              , do_LegendPos               },
  { "LegendColor"            , do_LegendColor             },
  { "LegendOutline"          , do_LegendOutline           },
  { "BarWidth"               , do_BarWidth                },
  { "BarMargin"              , do_BarMargin               },
  { "Series.New"             , do_Series_New              },
  { "Series.Type"            , do_Series_Type             },
  { "Series.AxisY"           , do_Series_AxisY            },
  { "Series.Base"            , do_Series_Base             },
  { "Series.Style"           , do_Series_Style            },
  { "Series.MarkerShape"     , do_Series_MarkerShape      },
  { "Series.MarkerSize"      , do_Series_MarkerSize       },
  { "Series.LineWidth"       , do_Series_LineWidth        },
  { "Series.LineDash"        , do_Series_LineDash         },
  { "Series.Lighten"         , do_Series_Lighten          },
  { "Series.FillTransparency", do_Series_FillTransparency },
  { "Series.LineColor"       , do_Series_LineColor        },
  { "Series.FillColor"       , do_Series_FillColor        },
  { "Series.Tag"             , do_Series_Tag              },
  { "Series.TagPos"          , do_Series_TagPos           },
  { "Series.TagSize"         , do_Series_TagSize          },
  { "Series.TagBox"          , do_Series_TagBox           },
  { "Series.TagTextColor"    , do_Series_TagTextColor     },
  { "Series.TagFillColor"    , do_Series_TagFillColor     },
  { "Series.TagLineColor"    , do_Series_TagLineColor     },
  { "Series.Data"            , do_Series_Data             },
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
  { "GridColor"   , do_Axis_GridColor    },
  { "NumberFormat", do_Axis_NumberFormat },
  { "NumberSign"  , do_Axis_NumberSign   },
  { "NumberUnit"  , do_Axis_NumberUnit   },
  { "MinorNumber" , do_Axis_MinorNumber  },
  { "NumberPos"   , do_Axis_NumberPos    },
  { "NumberSize"  , do_Axis_NumberSize   },
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

  // Support delivering nothing but data (implicit Series.Data).
  parse_series_data();

  while ( parse_spec() ) {}
}

////////////////////////////////////////////////////////////////////////////////

void process_files( const std::vector< std::string >& file_list )
{
  for ( const auto& file_name : file_list ) {
    uint32_t file_name_idx = file_names.size();
    file_names.push_back( file_name );
    size_t line_number = 0;
    if ( file_name == "-" ) {
      std::string line;
      while ( std::getline( std::cin, line ) ) {
        trunc_nl( line );
        lines.push_back( { line, ++line_number, file_name_idx } );
      }
    } else {
      std::ifstream file( file_name );
      if ( file ) {
        std::string line;
        while ( std::getline( file, line ) ) {
          trunc_nl( line );
          lines.push_back( { line, ++line_number, file_name_idx } );
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
      if ( a == "-e2" ) {
        gen_example( 2 );
        return 0;
      }
      if ( a == "-e3" ) {
        gen_example( 3 );
        return 0;
      }
      if ( a == "-e4" ) {
        gen_example( 4 );
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
