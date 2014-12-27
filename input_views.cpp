#include "input_views.h"
#include "matrix_keypad.h"
#include "char_lcd_stm32f4.h"
#include "telescope_model.h"
#include "controller.h"
#include "rtc_management.h"
#include "telescope_model.h"
#include <cmath>

/****************************************
Integer_Input_View methods
 ****************************************/
Integer_Input_View::Integer_Input_View(std::string label)
{
  _label = label;
  _width = INPUT_VIEW_DEFAULT_WIDTH;
  is_two_line = true;
  _value = 0;
  finished = false;
  saved_cr = dsc_controller::get_character_reciever();
  dsc_controller::set_character_reciever(this);
}

Integer_Input_View::~Integer_Input_View()
{
  dsc_controller::set_character_reciever(saved_cr);
}

void Integer_Input_View::put_char(char c)
{
  if (_value == 0 && c == '0') {
    return;
  }
  if (c == keypad_enter_char) {
    enter_char_action();
    return;
  }
  if (c == keypad_backspace_char) {
    backspace_char_action();
    return;
  }
  if (c == scroll_up_char) {
    //decrement_position();
    return;
  }

  int sign = _value < 0 ? -1 : 1;
  int value = _value < 0 ? -_value : _value;
  if ('0' <= c && c <= '9') {
    value = 10 * value + (c - '0');
  }
  _value = sign * value;
}

void Integer_Input_View::enter_char_action()
{
  /* Presently rely on the destructor to restore the character_reciever */
  // dsc_controller::set_character_reciever(nullptr);
  finished = true;
}

void Integer_Input_View::backspace_char_action()
{
  // _value = (_value - _value%10)/10;
  _value = _value / 10;
}

void Integer_Input_View::set_value(int32_t v)
{
  _value = v;
}

const int32_t Integer_Input_View::get_value()
{
  return _value;
}

void Integer_Input_View::change_sign()
{
  _value = -_value;
}

void Integer_Input_View::set_width(int32_t w)
{
  _width = w;
}

std::unique_ptr < CharLCD_STM32F >
    Integer_Input_View::write_first_line(std::unique_ptr < CharLCD_STM32F > lcd)
{
  /* Assumes the lcd position is at the start of the line. */
  int space = _width - _label.size();
  int n = 0;
  while (n < space / 2) {
    n += lcd->print(' ');
  }
  n += lcd->print(_label);
  while (n < _width) {
    n += lcd->print(' ');
  }
  return std::move(lcd);
}

std::unique_ptr < CharLCD_STM32F >
    Integer_Input_View::write_second_line(std::unique_ptr < CharLCD_STM32F >
					  lcd)
{
  /* Assumes the lcd position is at the start of the line. */
  int n = 0;
  n += lcd->print(":=");	/* A command line symbol. */
  n += lcd->print(_value);
  while (n < _width) {
    n += lcd->print(' ');
  }
  return std::move(lcd);
}

/****************************************
Sexagesimal_Input_View methods
 ****************************************/

Sexagesimal_Input_View::Sexagesimal_Input_View(std::string label)
{
  _label = label;
  hhh = 0;
  mm = 0;
  ss = 0;
  subseconds = 0.0;
  position = 0;
  _width = INPUT_VIEW_DEFAULT_WIDTH;
  is_two_line = true;  is_negative = false;
  finished = false;
  plus_char = ' ';
  minus_char = '-';
  cursor_column = 0;
  msd_digits = 3;
  center_label = true;
  saved_cr = dsc_controller::get_character_reciever();
  dsc_controller::set_character_reciever(this);
}

Sexagesimal_Input_View::~Sexagesimal_Input_View()
{
  dsc_controller::set_character_reciever(saved_cr);
}

void Sexagesimal_Input_View::put_char(char c)
{
  if (c == keypad_enter_char) {
    enter_char_action();
    return;
  }
  if (c == keypad_backspace_char) {
    backspace_char_action();
    return;
  }
  if ('0' <= c && c <= '9') {
    stuff_digit(c);
    return;
  }
  if (c == scroll_up_char) {
    decrement_position();
    return;
  }
  if (c == scroll_down_char) {
    increment_position();
    return;
  }
  if (c == select_char) {
    change_sign();
    return;
  }
}

/* Enter or return character indicates user is finished. */
void Sexagesimal_Input_View::enter_char_action()
{
  // dsc_controller::set_character_reciever(nullptr);
  finished = true;
}

void Sexagesimal_Input_View::backspace_char_action()
{
  switch (position) {
  case 0:
    if (hhh_str.size() > 0) {
      hhh_str.pop_back();
    }
    return;
  case 1:
    if (mm_str.size() > 0) {
      mm_str.pop_back();
    } else {
      position = 0;
      backspace_char_action();
    }
    return;
  case 2:
    if (ss_str.size() > 0) {
      ss_str.pop_back();
    } else {
      position = 1;
      backspace_char_action();
    }
    return;
  case 3:
    if (subseconds_str.size() > 0) {
      subseconds_str.pop_back();
    } else {
      position = 2;
      backspace_char_action();
    }
    return;
  }

}

void Sexagesimal_Input_View::stuff_digit(char d)
{
  switch (position) {
  case 0:
    if (hhh_str.size() < msd_digits) {
      hhh_str += d;
      if (hhh_str.size() >= msd_digits) {
	position = 1;
      }
    }
    //hhh_read();
    return;
  case 1:
    if (mm_str.size() < 2) {
      mm_str += d;
      if (mm_str.size() >= 2) {
	position = 2;
      }
    }
    //mm_read();
    return;
  case 2:
    if (ss_str.size() < 2) {
      ss_str += d;
      if (ss_str.size() >= 2) {
	position = 3;
      }
    }
    //ss_read();
    return;
  case 3:
    if (subseconds_str.size() < 6) {
      subseconds_str += d;
    }
    //subseconds_read();
    return;
  }
}

void Sexagesimal_Input_View::hhh_read()
{
  unsigned int value;
  int n = sscanf(hhh_str.data(), "%3u", &value);
  if (n == 1) {
    hhh = is_negative ? -value : value;
  } else {
    hhh = 0;
  }
}

void Sexagesimal_Input_View::mm_read()
{
  unsigned int value;
  int n = sscanf(mm_str.data(), "%2u", &value);
  if (n == 1) {
    mm = value;
  } else {
    mm = 0;
  }
}

void Sexagesimal_Input_View::ss_read()
{
  unsigned int value;
  int n = sscanf(ss_str.data(), "%2u", &value);
  if (n == 1) {
    ss = value;
  } else {
    ss = 0;
  }
}

void Sexagesimal_Input_View::subseconds_read()
{
  unsigned int ui_value;
  int n = sscanf(subseconds_str.data(), "%u", &ui_value);
  if (n == 1) {
    float temp = static_cast < float >(ui_value);
    for (uint i = 0; i < subseconds_str.size(); ++i) {
      temp /= 10.0;
    }
    subseconds = temp;
  } else {
    subseconds = 0.0;
  }
}

void Sexagesimal_Input_View::change_sign()
{
  is_negative = !is_negative;
  hhh = -hhh;
}

void Sexagesimal_Input_View::make_negative()
{
  is_negative = true;
  hhh = hhh <= 0 ? hhh : -hhh;
}

void Sexagesimal_Input_View::make_positive()
{
  is_negative = false;
  hhh = hhh >= 0 ? hhh : -hhh;
}

sexagesimal::Sexagesimal Sexagesimal_Input_View::get_value()
{
  hhh_read();
  mm_read();
  ss_read();
  subseconds_read();
  uint16_t milli_seconds = round((subseconds * 1000.0));
  return sexagesimal::Sexagesimal {
  hhh, mm, ss, milli_seconds};
}

void Sexagesimal_Input_View::set_width(int32_t w)
{
  _width = w;
}

void Sexagesimal_Input_View::set_plus_char(char p)
{
  plus_char = p;
}

void Sexagesimal_Input_View::set_minus_char(char m)
{
  minus_char = m;
}

void Sexagesimal_Input_View::set_msd_digits(uint8_t d)
{
  msd_digits = d;
}

void Sexagesimal_Input_View::set_center_label(bool value){
  center_label = value;
}

std::string lead_zeros_to_underscore(std::string input)
{
  std::string output = input;
  for (uint i = 0; i < input.size() - 1; ++i) {
    if (output[i] == '0') {
      output[i] = '_';
    } else {
      break;
    }
  }
  return output;
}

std::string lead_zeros_to_space(std::string input)
{
  std::string output = input;
  for (uint i = 0; i < input.size() - 1; ++i) {
    if (output[i] == '0') {
      output[i] = ' ';
    } else {
      break;
    }
  }
  return output;
}



std::unique_ptr < CharLCD_STM32F >
    Sexagesimal_Input_View::write_first_line(std::unique_ptr < CharLCD_STM32F >
					     lcd)
{
  /* Assumes the lcd position is at the start of the line. */
  int space = _width - _label.size();
  int n = 0;
  if( center_label ){
    while (n < space / 2) {
      n += lcd->print(' ');
    }
  }
  n += lcd->print(_label);
  while (n < _width) {
    n += lcd->print(' ');
  }
  return std::move(lcd);
}

std::unique_ptr < CharLCD_STM32F >
    Sexagesimal_Input_View::write_second_line(std::unique_ptr < CharLCD_STM32F >
					      lcd)
{
  char cursor_char = '_';
  int n = 0;
  n += lcd->print(":=");
  if (is_negative) {
    n += lcd->print(minus_char);
  } else {
    n += lcd->print(plus_char);
  }
  int pad = msd_digits - static_cast < int >(hhh_str.size());
  if (position == 0) {
    --pad;
  }
  n += lcd->print(hhh_str);
  if (position == 0) {
    n += lcd->print(cursor_char);
  }
  while (pad > 0) {
    n += lcd->print(' ');
    --pad;
  }
  n += lcd->print(':');
  if (position < 1) {
    while (n < _width) {
      n += lcd->print(' ');
    }
    return std::move(lcd);
  }
  ///////             ////////////
  pad = 2 - mm_str.size();
  if (position == 1) {
    --pad;
  }
  n += lcd->print(mm_str);
  if (position == 1) {
    n += lcd->print(cursor_char);
  }
  while (pad > 0) {
    n += lcd->print(' ');
    --pad;
  }
  n += lcd->print(':');
  if (position < 2) {
    while (n < _width) {
      n += lcd->print(' ');
    }
    return std::move(lcd);
  }
  //////////              /////////////////////
  pad = 2 - ss_str.size();
  if (position == 2) {
    --pad;
  }
  n += lcd->print(ss_str);
  if (position == 2) {
    n += lcd->print(cursor_char);
  }
  while (pad > 0) {
    n += lcd->print(' ');
    --pad;
  }
  n += lcd->print('.');
  if (position < 2) {
    while (n < _width) {
      n += lcd->print(' ');
    }
    return std::move(lcd);
  }
  /////////////////////////////////////////////
  n += lcd->print(subseconds_str);
  if (position == 3 && subseconds_str.size() < 6) {
    n += lcd->print(cursor_char);
  }
  //////////              /////////////////////
  while (n < _width) {
    n += lcd->print(' ');
  }
  return std::move(lcd);
}

std::unique_ptr < CharLCD_STM32F >
    Sexagesimal_Input_View::write_third_line(std::unique_ptr < CharLCD_STM32F >
					     lcd)
{
  int n = 0;
  n += lcd->print(":=");
  int pad = 4 - static_cast < int >(hhh_str.size());
  if (hhh_str.size() == 0) {
    --pad;			/* Account for printing a zero. */
  }
  int half_pad = pad / 2;
  while (pad > half_pad) {
    n += lcd->print(' ');
    --pad;
  }
  if (is_negative) {
    n += lcd->print(minus_char);
  } else {
    n += lcd->print(plus_char);
  }
  while (pad > 0) {
    n += lcd->print(' ');
    --pad;
  }
  if (hhh_str.size() < 1) {
    n += lcd->print('0');
  }
  n += lcd->print(lead_zeros_to_space(hhh_str));
  if (position == 0) {
    cursor_column = n + 1;
  }
  if (hhh_str.size() >= msd_digits) {
    n += lcd->print(':');
  }
  ///////////////////
  pad = 2 - mm_str.size();
  while (pad > 0) {
    n += lcd->print(' ');
    --pad;
  }
  n += lcd->print(mm_str);
  if (position == 1) {
    cursor_column = n + 1;
  }
  if (mm_str.size() >= 2 || position > 1) {
    n += lcd->print(':');
  }
  ///////////////////
  pad = 2 - ss_str.size();
  while (pad > 0) {
    n += lcd->print(' ');
    --pad;
  }
  n += lcd->print(ss_str);
  if (position == 2) {
    cursor_column = n + 1;
  }
  if (ss_str.size() >= 2 || position > 2) {
    n += lcd->print('.');
  }
  ////////////////////
  n += lcd->print(subseconds_str);
  if (position == 3) {
    cursor_column = n + 1;
  }
  int end_pad = 0;
  while (n < _width) {
    n += lcd->print(' ');
    ++end_pad;
  }
  while (end_pad >= 0) {
    lcd->command(0x10);		// Shift cursor to the left.
    --end_pad;
  }
  for (int n = 1; n <= 20; ++n) {
    lcd->command(0x10);		// Shift cursor to the left.
  }
  return std::move(lcd);
}

/* This works for an initially empty entry but it is slow. */
std::unique_ptr < CharLCD_STM32F >
    Sexagesimal_Input_View::write_fourth_line(std::unique_ptr < CharLCD_STM32F >
					      lcd)
{
  int n = 0;
  n += lcd->print(":= ");
  if (is_negative) {
    n += lcd->print(minus_char);
  } else {
    n += lcd->print(plus_char);
  }
  std::string my_hhh = hhh_str;
  std::string my_mm = mm_str;
  std::string my_ss = ss_str;
  std::string my_subseconds = subseconds_str;
  while (my_hhh.size() < msd_digits) {
    my_hhh = my_hhh + ' ';
  }
  /////////////////////////
  if (position <= 1) {
    while (my_mm.size() < 2) {
      my_mm = my_mm + ' ';
    }
  } else {
    while (my_mm.size() < 2) {
      my_mm = '0' + my_mm;
    }
  }
  /////////////////////////
  if (position <= 2) {
    while (my_ss.size() < 2) {
      my_ss = my_ss + ' ';
    }
  } else {
    while (my_ss.size() < 2) {
      my_ss = '0' + my_ss;
    }
  }
  char cursor_char = '_';
  switch (position) {
  case 0:
    my_hhh[hhh_str.size()] = cursor_char;
    n += lcd->print(my_hhh);
    n += lcd->print(':');
    break;
  case 1:
    my_mm[mm_str.size()] = cursor_char;
    n += lcd->print(my_hhh);
    n += lcd->print(':');
    n += lcd->print(my_mm);
    n += lcd->print(':');
    break;
  case 2:
    my_ss[ss_str.size()] = cursor_char;
    n += lcd->print(my_hhh);
    n += lcd->print(':');
    n += lcd->print(my_mm);
    n += lcd->print(':');
    n += lcd->print(my_ss);
    n += lcd->print('.');
    break;
  case 3:
    if (my_subseconds.size() < 6) {
      my_subseconds += cursor_char;
    }
    n += lcd->print(my_hhh);
    n += lcd->print(':');
    n += lcd->print(my_mm);
    n += lcd->print(':');
    n += lcd->print(my_ss);
    n += lcd->print('.');
    n += lcd->print(my_subseconds);
    break;
  }
  while (n < _width) {
    n += lcd->print(' ');
  }
  return std::move(lcd);
}

void Sexagesimal_Input_View::increment_position()
{
  int temp = position + 1;
  if (temp > 3) {
    temp = 0;
  }
  position = temp;
}

void Sexagesimal_Input_View::decrement_position()
{
  int temp = position - 1;
  if (temp < 0) {
    temp = 3;
  }
  position = temp;
}

/********************************************************
 *
 *   Confirm_Input_View;
 *
 ********************************************************/
Confirm_Input_View::Confirm_Input_View()
{
  width_ = INPUT_VIEW_DEFAULT_WIDTH;
  saved_cr = dsc_controller::get_character_reciever();
  is_okay = false;
  text_ = "Should I?";
  true_text_ = "Yes";
  false_text_ = "No";
  finished = false;
  dsc_controller::set_character_reciever(this);
}

Confirm_Input_View::~Confirm_Input_View(){
  dsc_controller::set_character_reciever(saved_cr);
}

void Confirm_Input_View::put_char(char c){
  switch( c ){
  case keypad_enter_char:
    is_okay = true;
    finished = true;
    return;
  case keypad_backspace_char:
    is_okay = false;
    finished = true;
    return;
  }
}


void Confirm_Input_View::set_text( std::string text ){
  text_ = text;
}

void Confirm_Input_View::set_true_text( std::string text ){
  true_text_ = text;
}

void Confirm_Input_View::set_false_text( std::string text ){
  false_text_ = text;
}
 
bool Confirm_Input_View::get_is_okay(){
  return is_okay;
}


std::unique_ptr < CharLCD_STM32F >
    Confirm_Input_View::write_first_line(std::unique_ptr < CharLCD_STM32F > lcd)
{
  int space = width_ - text_.size();
  int n = 0;
  while (n < space / 2) {
    n += lcd->print(' ');
  }
  n += lcd->print( text_ );
  while (n < width_) {
    n += lcd->print(' ');
  }
  return std::move(lcd);
}

std::unique_ptr < CharLCD_STM32F >
    Confirm_Input_View::write_second_line(std::unique_ptr < CharLCD_STM32F >
					  lcd)
{
  int n = 0;
  n += lcd->print( keypad_backspace_char );
  n += lcd->print( ") " );
  n += lcd->print( false_text_ );
  while ( n < width_/2 ) {
    n += lcd->print(' ');
  }
  n += lcd->print( keypad_enter_char );
  n += lcd->print( ") " );
  n += lcd->print( true_text_ );
  while ( n < width_ ) {
    n += lcd->print(' ');
  }
  return std::move(lcd);
}



/****************************************
 *
 *  Burnham_Format_Input_View methods
 *
 ****************************************/

Burnham_Format_Input_View::Burnham_Format_Input_View(){

  cursor_char = '_';
  plus_char = 'n';
  minus_char = 's';
  finished = false;
  saved_cr = dsc_controller::get_character_reciever();
  dsc_controller::set_character_reciever(this);
  position = 0;
  char_buffer[0] = '\0';
  /* An invariant of this class is that char_buffer[5] equals [plus_char or minus_char] */
  char_buffer[5] = plus_char;
  width_ = INPUT_VIEW_DEFAULT_WIDTH;
  label_ = "Burnham RA and Dec.";
}

Burnham_Format_Input_View::~Burnham_Format_Input_View(){
  dsc_controller::set_character_reciever(saved_cr);
}

void Burnham_Format_Input_View::put_char(char c){
  switch( c ){
  case keypad_enter_char:
    finished = true;
    return;
  case keypad_backspace_char:
    decrement_position();
    return;
  case select_char:
    toggle_plus_minus();
    return;
  case scroll_up_char:
    make_positive();
    return;
  case scroll_down_char:
    make_negative();
    return;
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
    put_digit( c );
    return;
  default:
    return;
  }
}

void Burnham_Format_Input_View::put_digit(char d){
  if( position < 10 ){
    char_buffer[position] = d;
  }
  // char_buffer[position] = d;
  // char_buffer[10] = '\0';
  increment_position(); /* positon is the rightmost unset character. */
}

void Burnham_Format_Input_View::increment_position(){
  if( position >= 10 ){
    position = 10;
    char_buffer[10] = '\0';
    return;
  }
  ++position;
  if( position == 5 ){
    ++position; // Skip over the [n|s] character.
  }
  char_buffer[position] = '\0';
}

void Burnham_Format_Input_View::decrement_position(){
  if( position == 0 ){
    return;
  }
  char_buffer[position] = '\0';
  --position;
  if( position == 5 ){
    --position; // Skip over the [n|s] character.
  }
  char_buffer[position] = '\0';
}

void Burnham_Format_Input_View::toggle_plus_minus(){
  if( char_buffer[5] == plus_char ){
    char_buffer[5] = minus_char;
  }else{
    char_buffer[5] = plus_char;
  }
}

void Burnham_Format_Input_View::make_positive(){
  char_buffer[5] = plus_char;
}

void Burnham_Format_Input_View::make_negative(){
  char_buffer[5] = minus_char;
}

double Burnham_Format_Input_View::get_RA(){
  double RA = 0;
  unsigned int scan_value;
  int n;
  scan_value = 0;
  n = sscanf( char_buffer, "%2u", &scan_value  );
  if( n == 1 ){
    RA += static_cast<double>( scan_value );
  }
  scan_value = 0;
  n = sscanf( char_buffer + 2, "%2u", &scan_value  );
  if( n == 1 ){
    RA += (1.0/60.0)*static_cast<double>( scan_value );
  }
  scan_value = 0;
  n = sscanf( char_buffer + 4, "%1u", &scan_value  );
  if( n == 1 ){
    RA += (1.0/600.0)*static_cast<double>( scan_value );
  }

  return RA;
}


double Burnham_Format_Input_View::get_Declination(){
  double dec = 0;
  unsigned int scan_value;
  int n;
  scan_value = 0;
  n = sscanf( char_buffer + 6, "%2u", &scan_value  );
  if( n == 1 ){
    dec += static_cast<double>( scan_value );
  }
  scan_value = 0;
  n = sscanf( char_buffer + 8, "%2u", &scan_value  );
  if( n == 1 ){
    dec += (1.0/60.0)*static_cast<double>( scan_value );
  }
  if( char_buffer[5] == minus_char ){
    return -dec;
  }else{
    return dec;
  }
}

CAA2DCoordinate Burnham_Format_Input_View::get_RA_and_Dec(){
  CAA2DCoordinate RA_and_Dec;
  RA_and_Dec.X = get_RA();
  RA_and_Dec.Y = get_Declination();
  return RA_and_Dec;
}


std::unique_ptr < CharLCD_STM32F >
    Burnham_Format_Input_View::write_first_line(std::unique_ptr < CharLCD_STM32F > lcd)
{
  int32_t space = width_ - label_.size();
  int32_t n = 0;
  while (n < space / 2) {
    n += lcd->print(' ');
  }
  n += lcd->print(label_);
  while ( n < width_ ) {
    n += lcd->print(' ');
  }
  return std::move(lcd);
}

std::unique_ptr < CharLCD_STM32F >
    Burnham_Format_Input_View::write_second_line(std::unique_ptr < CharLCD_STM32F > lcd)
{
  int32_t n = 0;
  n += lcd->print( " > " );
  n += lcd->print( char_buffer );
  if( position <= 10 ){
    n += lcd->print( cursor_char );
  }
  while ( n < width_ ) {
    n += lcd->print(' ');
  }
  return std::move(lcd);
}

/*****************************************
 *
 * Alignment_Timestamp_Prompt methods
 *
 ******************************************/

Alignment_Timestamp_Prompt::Alignment_Timestamp_Prompt(  Simple_Altazimuth_Scope* tele, std::string object){
  prompt_text_1 = "Center telescope on";
  object_name = object;
  prompt_text_3 = "  #| Timestamp";
  prompt_text_4 = "  D| Cancel";
  telescope = tele;
  _width = INPUT_VIEW_DEFAULT_WIDTH;
  timestamp = 0.0;
  _has_data = false;
  finished = false;
  saved_cr = dsc_controller::get_character_reciever();
  dsc_controller::set_character_reciever(this);
}

Alignment_Timestamp_Prompt::~Alignment_Timestamp_Prompt(){
  dsc_controller::set_character_reciever(saved_cr);
}

void Alignment_Timestamp_Prompt::grab_timestamp_data(){
  /* Possibly the telescope is moving. It is easier to average the time than the position data. */
  double t0 = JD_timestamp_pretty_good_000();
  encoder_data = telescope->get_snapshot();
  double t1 = JD_timestamp_pretty_good_000();
  timestamp = (t0 + t1)/2.0;
  _has_data = true;
}

void Alignment_Timestamp_Prompt::put_char(char c){
  switch( c ){
  case keypad_enter_char:
    grab_timestamp_data();
    finished = true;
    return;
  case keypad_backspace_char:
    finished = true;
    return;
  }
}

double Alignment_Timestamp_Prompt::get_timestamp(){
  return timestamp;
}

Alt_Azi_Snapshot_t Alignment_Timestamp_Prompt::get_encoder_data(){
  return encoder_data;
}

std::string Alignment_Timestamp_Prompt::get_object_name(){
  return object_name;
}

bool Alignment_Timestamp_Prompt::has_data(){
  return _has_data;
}



std::unique_ptr < CharLCD_STM32F > Alignment_Timestamp_Prompt::write_first_line(std::unique_ptr <
										CharLCD_STM32F > lcd){
  int n = 0;
  n += lcd->print( prompt_text_1 );
  while (n < _width) {
    n += lcd->print(' ');
  }
  return std::move(lcd);
}

std::unique_ptr < CharLCD_STM32F > Alignment_Timestamp_Prompt::write_second_line(std::unique_ptr <
										 CharLCD_STM32F > lcd){
  int n = 0;
  int space = _width - object_name.size();
  for( int i=1; i<space/2; ++i  ){
    n += lcd->print( ' ' );
  }
  n += lcd->print( object_name );
  while (n < _width) {
    n += lcd->print(' ');
  }
  return std::move(lcd);
}

std::unique_ptr < CharLCD_STM32F > Alignment_Timestamp_Prompt::write_third_line(std::unique_ptr <
										CharLCD_STM32F > lcd){
  int n = 0;
  n += lcd->print( prompt_text_3 );
  while (n < _width) {
    n += lcd->print(' ');
  }
  return std::move(lcd);
}

std::unique_ptr < CharLCD_STM32F > Alignment_Timestamp_Prompt::write_fourth_line(std::unique_ptr <
										 CharLCD_STM32F > lcd){
  int n = 0;
  n += lcd->print( prompt_text_4 );
  while (n < _width) {
    n += lcd->print(' ');
  }
  return std::move(lcd);
}

