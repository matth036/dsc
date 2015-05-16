#include "proximity_views.h"

#include "controller.h"
#include "keypad_layout.h"
#include "char_lcd_stm32f4.h"

Proximate_Stars_View::Proximate_Stars_View( Simple_Altazimuth_Scope* scope){
  telescope = scope;
  finished = false;
  width_ = PROXIMITY_VIEW_DEFAULT_WIDTH;

  saved_cr = dsc_controller::get_character_reciever();
  dsc_controller::set_character_reciever(this);
}

Proximate_Stars_View::~Proximate_Stars_View( ){
  dsc_controller::set_character_reciever(saved_cr);
}

void Proximate_Stars_View::put_char( char c ){
  if( c == keypad_return_char ){
    dismiss_action();
  }
}

void Proximate_Stars_View::dismiss_action(){
  finished = true;
}

void Proximate_Stars_View::set_size( uint32_t s){
  size = s;
}

void Proximate_Stars_View::increment_position(){
  if( position + 1 < size ){
    ++position;
  }
}

void Proximate_Stars_View::decrement_position(){
  if( position > 0 ){
    --position;
  }
}










std::unique_ptr < CharLCD_STM32F > Proximate_Stars_View::write_first_line(std::unique_ptr < CharLCD_STM32F > lcd)
{
  int n = 0;
  n += lcd->print( "Under Construction" );
  while (n < width_) {
    n += lcd->print(' ');
  } 
  return std::move(lcd);
}

std::unique_ptr < CharLCD_STM32F > Proximate_Stars_View::write_second_line(std::unique_ptr <
									      CharLCD_STM32F > lcd)
{
  int n = 0;
  n += lcd->print( "Under Construction" );
  while (n < width_) {
    n += lcd->print(' ');
  } 
  return std::move(lcd);
}


std::unique_ptr < CharLCD_STM32F > Proximate_Stars_View::write_third_line(std::unique_ptr <
									      CharLCD_STM32F > lcd)
{
  int n = 0;
  n += lcd->print( "Under Construction" );
  while (n < width_) {
    n += lcd->print(' ');
  } 
  return std::move(lcd);
}

std::unique_ptr < CharLCD_STM32F > Proximate_Stars_View::write_fourth_line(std::unique_ptr <
									      CharLCD_STM32F > lcd)
{
  int n = 0;
  n += lcd->print( "Under Construction" );
  while (n < width_) {
    n += lcd->print(' ');
  } 
  return std::move(lcd);
}








