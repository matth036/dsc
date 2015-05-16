#include "proximity_views.h"

#include "controller.h"
#include "keypad_layout.h"
#include "char_lcd_stm32f4.h"
#include "rtc_management.h"
#include "starlist_access.h"
#include "AAAngularSeparation.h"

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
  switch( c ){
  case keypad_return_char:
    dismiss_action();
    return;
  case scroll_up_char:
    decrement_position();
    return;
  case scroll_down_char:
    increment_position();
    return;
  }
}

void Proximate_Stars_View::dismiss_action(){
  finished = true;
}

void Proximate_Stars_View::set_size( uint32_t s){
  if( size < 4 ){
    return;
  }
  size = s;
  if( position >= size ){
    position = size - 1;
  }
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


void Proximate_Stars_View::run_algorithm(){
  while( stars.size() > 0 ){
    stars.pop_back();
  }
  uint32_t index = 0;
  while( stars.size() < size && index < starlist_access::starlist_size() ){
    /* We may want to filter for magnitude here. */
    stars.push_back( index );
    ++index;
  }
  /* @TODO convert to J2000 RA and Declination */
  double target_RA = RA_and_Dec.X;
  double target_Dec = RA_and_Dec.Y;
  auto distance_from_target = [target_RA, target_Dec]( CAA2DCoordinate subject ){
    return CAAAngularSeparation::Separation(target_RA, target_Dec, subject.X, subject.Y );
  };
  double JD = this->JD;
  auto bsc_distance_from_target = [JD, distance_from_target]( uint32_t index ){
    CAA2DCoordinate position = starlist_access::proper_motion_adjusted_position( index, JD);
    return distance_from_target( position );
  };
  auto target_proximity_compare = [bsc_distance_from_target]( const uint32_t a, const uint32_t b ){
    double distance_a = bsc_distance_from_target( a );
    double distance_b = bsc_distance_from_target( b );
    return distance_a < distance_b;
  };
  make_heap( stars.begin(), stars.end(), target_proximity_compare);



}









std::unique_ptr < CharLCD_STM32F > Proximate_Stars_View::write_first_line(std::unique_ptr < CharLCD_STM32F > lcd)
{
  JD = JD_timestamp_pretty_good_000();
  RA_and_Dec = telescope->current_RA_and_Dec();
  run_algorithm();
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








