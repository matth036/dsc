#include "output_views.h"
#include "input_views.h"
#include "controller.h"
#include "char_lcd_stm32f4.h"
#include "rtc_management.h"
#include "keypad_layout.h"
#include "sexagesimal.h"
#include "AADate.h"
#include "AASidereal.h"
#include <cmath>
#include "main.h"
#include "binary_tidbits.h"
#include "solar_system.h"
#include "ngc_objects.h"

/******************************************************************/

Current_Time_View::Current_Time_View(){
  finished = false;
  width_ = INPUT_VIEW_DEFAULT_WIDTH;
  saved_cr = dsc_controller::get_character_reciever();
  dsc_controller::set_character_reciever(this);
  double julian_date = 2440587.5; /* Zero Unix time, January 1, 1970 00:00:00 GMT.  
                              Will be reset, intended as a recognizable error value. */
  JD = CAADate( julian_date, true );
}

Current_Time_View::~Current_Time_View( ){
  dsc_controller::set_character_reciever(saved_cr);
}


void Current_Time_View::put_char( char c ){
  if( c == keypad_return_char ){
    dismiss_action();
  }
}

void Current_Time_View::dismiss_action(){
  finished = true;
}

std::unique_ptr < CharLCD_STM32F > Current_Time_View::write_first_line(std::unique_ptr < CharLCD_STM32F > lcd)
{
  RTC_TimeTypeDef RTC_TimeStructure;
  uint32_t Secondfraction =
    1000 -
    ((uint32_t) ((uint32_t) RTC_GetSubSecond() * 1000) / (uint32_t) 0x3FF);
  RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
  sexagesimal::Sexagesimal rtc_time {
    RTC_TimeStructure.RTC_Hours,
      RTC_TimeStructure.RTC_Minutes,
      RTC_TimeStructure.RTC_Seconds,
      static_cast < uint16_t > (Secondfraction)};

  double julian_date = JD_timestamp_pretty_good_000();
  JD = CAADate( julian_date, true );

  int n = 0;
  n += lcd->print( "GMT: " );
  n += lcd->print(rtc_time.to_string());
  while (n < width_) {
    n += lcd->print(' ');
  } 
  return std::move(lcd);
}

std::unique_ptr < CharLCD_STM32F > Current_Time_View::write_second_line(std::unique_ptr <
									      CharLCD_STM32F > lcd)
{
  int month = JD.Month();
  int n = 0;
  n += lcd->print( Current_Time_View::month_string(month) );
  n += lcd->print( ' ' );
  n += lcd->print( JD.Day() );
  n += lcd->print( ", " );
  n += lcd->print( JD.Year() );
  while (n < width_) {
    n += lcd->print(' ');
  } 
  return std::move(lcd);
}


std::unique_ptr < CharLCD_STM32F > Current_Time_View::write_third_line(std::unique_ptr <
									      CharLCD_STM32F > lcd)
{
  // double sidereal_time = CAASidereal::ApparentGreenwichSiderealTime( JD.Julian() );
  double sidereal_time = CAASidereal::MeanGreenwichSiderealTime( JD.Julian() );

  sexagesimal::Sexagesimal longitude = get_backup_domain_longitude();
  
  /* Astronomical Almanac 2014 page B11, 
   * local mean sidereal time = (Greenwich mean sidereal time) + (east lingitude).
   * and Note indicates "ensure that the units of all quantities are compatable." 
   *
   * Compatabilty requires the (24/360) factor.
   */

  if( binary_tidbits::west_longitude_is_positive() ){
    sidereal_time -= (24.0/360.0)*longitude.to_double();
  }else{
    sidereal_time += (24.0/360.0)*longitude.to_double();
  }
  sidereal_time = CAACoordinateTransformation::MapTo0To24Range(sidereal_time);

  std::string sidereal_time_str = sexagesimal::Sexagesimal(sidereal_time).to_string();

  int n = 0;
  n += lcd->print( "Sidereal" );
  n += lcd->print( sidereal_time_str );

  while (n < width_) {
    n += lcd->print(' ');
  } 
  return std::move(lcd);
}

std::unique_ptr < CharLCD_STM32F > Current_Time_View::write_fourth_line(std::unique_ptr <
									      CharLCD_STM32F > lcd)
{
  int n = 0;
  n += lcd->print( "JD " );
  n +=   lcd->print(JD.Julian(), 8);
  while (n < width_) {
    n += lcd->print(' ');
  } 
  return std::move(lcd);
}

std::string Current_Time_View::month_string( int m ){
  if( m<1 || m>12 ){
    m = 1 + (m-1)%12;
  }
  switch( m ){
  case 1:
    return "January";
  case 2:
    return "February";
  case 3:
    return "March";
  case 4:
    return "April";
  case 5:
    return "May";
  case 6:
    return "June";
  case 7:
    return "July";
  case 8:
    return "August";
  case 9:
    return "September";
  case 10:
    return "October";
  case 11:
    return "November";
  case 12:
    return "December";
  default:
    return "Thermidor";
  }
}


/**************************************************************
I have the topocentric information right but not the equatorial information.


 **************************************************************/


Angular_Coordinates_View::Angular_Coordinates_View( Simple_Altazimuth_Scope* scope ){
  telescope = scope;
  finished = false;
  width_ = INPUT_VIEW_DEFAULT_WIDTH;

  saved_cr = dsc_controller::get_character_reciever();
  dsc_controller::set_character_reciever(this);
}

Angular_Coordinates_View::~Angular_Coordinates_View( ){
  dsc_controller::set_character_reciever(saved_cr);
}

void Angular_Coordinates_View::put_char( char c ){
  if( c == keypad_return_char ){
    dismiss_action();
  }
}

void Angular_Coordinates_View::dismiss_action(){
  finished = true;
}
/*
 * On the left is the topocentric Azimuth and Altitude.
 * On the right is the Telescope frame aziumuth and Altitude
 * These should be almost coincident.
*/
std::unique_ptr < CharLCD_STM32F > Angular_Coordinates_View::write_first_line(std::unique_ptr <
									      CharLCD_STM32F > lcd)
{
  Azi_and_Alt = telescope->current_topocentric_Azi_and_Alt();
  int n = 0;
  n += lcd->print( "Azi " );
  n += lcd->print( Azi_and_Alt.X, 2 );
  while (n < width_ - 9) {
    n += lcd->print(' ');
  } 
  n += lcd->print( "| " );
  n += lcd->print( telescope->azimuth_degrees(), 2 );
  while (n < width_) {
    n += lcd->print(' ');
  } 
  return std::move(lcd);
}

std::unique_ptr < CharLCD_STM32F > Angular_Coordinates_View::write_second_line(std::unique_ptr <
									      CharLCD_STM32F > lcd)
{
  int n = 0;
  n += lcd->print( "Alt " );
  n += lcd->print( Azi_and_Alt.Y, 2 );
  while (n < width_ - 9) {
    n += lcd->print(' ');
  } 
  n += lcd->print( "| " );
  n += lcd->print( telescope->altitude_degrees(), 2 );
  while (n < width_) {
    n += lcd->print(' ');
  } 
  return std::move(lcd);
}

std::unique_ptr < CharLCD_STM32F > Angular_Coordinates_View::write_third_line(std::unique_ptr <
									      CharLCD_STM32F > lcd)
{
  RA_and_Dec = telescope->current_RA_and_Dec();
  RA_str = sexagesimal::Sexagesimal(static_cast<double>(RA_and_Dec.X)).to_dms_string();
  Dec_str = sexagesimal::Sexagesimal(static_cast<double>(RA_and_Dec.Y)).to_dms_string();
  //Dec_str = sexagesimal::Sexagesimal(RA_and_Dec.Y).to_string();
  int n = 0;
  n += lcd->print( " RA  "  );
  n += lcd->print( RA_str );
  //n += lcd->print( RA_and_Dec.X*3600,1);
  while (n < width_) {
    n += lcd->print(' ');
  } 
  return std::move(lcd);
}

std::unique_ptr < CharLCD_STM32F > Angular_Coordinates_View::write_fourth_line(std::unique_ptr <
									       CharLCD_STM32F > lcd)
{
  int n = 0;

  n += lcd->print( " Dec "  );
  // n += lcd->print( RA_and_Dec.Y,4 );
  n += lcd->print( Dec_str );
  while (n < width_) {
    n += lcd->print(' ');
  } 
  return std::move(lcd);
}



/*******************************************************
 *  Presently the Pushto Output View will be a single line
 *  indicating the difference in encoder values from the target.
 *******************************************************/

Pushto_Output_View::Pushto_Output_View(  CAA2DCoordinate RA_and_Dec  ){
  RA_Dec = RA_and_Dec;
  finished = false;
  width_ = INPUT_VIEW_DEFAULT_WIDTH;
  label_1 = "Push To";
  label_2 = "Not set";
  pressure = Alignment_Sight_Item::DEFAULT_PRESSURE;
  temperature = Alignment_Sight_Item::DEFAULT_TEMPERATURE;
  data_set = get_main_sight_data();  
  telescope = get_main_simple_telescope();

  saved_cr = dsc_controller::get_character_reciever();
  dsc_controller::set_character_reciever(this);
}

Pushto_Output_View::~Pushto_Output_View(){
  dsc_controller::set_character_reciever(saved_cr);
}

void Pushto_Output_View::set_label_1(std::string text){
  label_1 = text;
}

void Pushto_Output_View::set_label_2(std::string text){
  label_2 = text;
}

void Pushto_Output_View::put_char(char c){
  if( c == keypad_return_char ){
    finished = true;
  }
}

std::unique_ptr < CharLCD_STM32F >
    Pushto_Output_View::write_first_line(std::unique_ptr < CharLCD_STM32F > lcd)
{
  /* Assumes the lcd position is at the start of the line. */
  int space = width_ - label_1.size();
  int n = 0;
  while (n < space / 2) {
    n += lcd->print(' ');
  }
  n += lcd->print(label_1);
  while (n < width_) {
    n += lcd->print(' ');
  }
  return std::move(lcd);
}

std::unique_ptr < CharLCD_STM32F >
    Pushto_Output_View::write_second_line(std::unique_ptr < CharLCD_STM32F > lcd)
{
  double JD = JD_timestamp_pretty_good_000();
  CAA3DCoordinate uv_topo = data_set->topocentric_unit_vector( RA_Dec, 
							       JD, 
							       pressure, 
							       temperature );
  CAA3Dfloat uv_h{uv_topo};  /* _h for horizontal coordinates. */
  CAA3Dfloat uv_t = telescope->topo_to_tele( uv_h );
  target = telescope->calculate_target_snapshot( uv_t );
  current = telescope->get_snapshot();
 
  /* Assumes the lcd position is at the start of the line. */
  int n = 0;
  int32_t diff = current.azi_value - target.azi_value;
  int32_t modulus = telescope->get_azimuth_ticks_per_revolution();
  while( diff > modulus/2 ){
    diff -= modulus;
  }
  while( diff < -modulus/2 ){
    diff += modulus;
  }
  n += lcd->print( diff );
  while (n < width_ / 2) {
    n += lcd->print(' ');
  }
  diff = current.alt_value - target.alt_value;
  modulus = telescope->get_altitude_ticks_per_revolution();
  while( diff > modulus/2 ){
    diff -= modulus;
  }
  while( diff < -modulus/2 ){
    diff += modulus;
  }
  n += lcd->print( diff );
  while (n < width_) {
    n += lcd->print(' ');
  }
  return std::move(lcd);
}

std::unique_ptr < CharLCD_STM32F >
    Pushto_Output_View::write_third_line(std::unique_ptr < CharLCD_STM32F > lcd)
{
  double JD = JD_timestamp_pretty_good_000();
  CAA3DCoordinate uv_topo = data_set->topocentric_unit_vector( RA_Dec, 
							       JD, 
							       pressure, 
							       temperature );
  CAA3Dfloat uv_h{uv_topo};  /* _h for horizontal coordinates. */
  CAA3Dfloat uv_t = telescope->topo_to_tele( uv_h );
  Alt_Azi_Snapshot_t current_snapshot = telescope->get_snapshot();
  CAA3DCoordinate uv_current = telescope->calculate_unit_vector( current_snapshot );
  CAA3Dfloat diff;
  diff.X = uv_current.X - uv_t.X;
  diff.Y = uv_current.Y - uv_t.Y;
  diff.Z = uv_current.Z - uv_t.Z;
  float chord = sqrt( diff.X*diff.X + diff.Y*diff.Y + diff.Z*diff.Z );
  float degrees = 2.0*atan( chord*0.5 );
  degrees = CAACoordinateTransformation::RadiansToDegrees( degrees );
  int n = 0;
  n += lcd->print( "dist = " );
  n += lcd->print( degrees, 2 );
  while (n < width_) {
    n += lcd->print(' ');
  }
  return std::move(lcd);
}

std::unique_ptr < CharLCD_STM32F >
    Pushto_Output_View::write_fourth_line(std::unique_ptr < CharLCD_STM32F > lcd)
{
  int space = width_ - label_2.size();
  int n = 0;
  while (n < space / 2) {
    n += lcd->print(' ');
  }
  n += lcd->print(label_2);
  while (n < width_) {
    n += lcd->print(' ');
  }
  return std::move(lcd);
}



/********************************************************************/

Countdown_View::Countdown_View(){
  finished = false;
  width_ = INPUT_VIEW_DEFAULT_WIDTH;
  saved_cr = dsc_controller::get_character_reciever();
  dsc_controller::set_character_reciever(this);
}

Countdown_View::~Countdown_View(){
  /* Effectivly, unsubscribe. */
  dsc_controller::set_character_reciever(saved_cr);
}

void Countdown_View::set_label_1( std::string s ){
  label_1 = s;
}

void Countdown_View::set_label_2( std::string s ){
  label_2 = s;
}

void Countdown_View::set_countdown_moment( double d ){
  countdown_moment = d;
}


void Countdown_View::update( ){
  double now = JD_timestamp_pretty_good_000();
  double diff = countdown_moment - now;
  days = static_cast<int>( floor(diff) );
  diff -= days;
  diff *= 24.0;
  hours = static_cast<int>( floor(diff) );
  diff -= hours;
  diff *= 60.0;
  minutes = static_cast<int>( floor(diff) );
  diff -= minutes;
  diff *= 60;
  seconds = static_cast<int>( floor(diff) );
  diff -= seconds;
  diff *= 1000;
  xxx = static_cast<int>( round(diff) );
}

void Countdown_View::put_char(char c){
  if( c == keypad_return_char ){
    finished = true;
  }
}

std::unique_ptr < CharLCD_STM32F >
    Countdown_View::write_first_line(std::unique_ptr < CharLCD_STM32F >
					     lcd)
{
  /* Assumes the lcd position is at the start of the line. */
  int space = width_ - label_1.size();
  int n = 0;
  while (n < space / 2) {
    n += lcd->print(' ');
  }
  n += lcd->print(label_1);
  while (n < width_) {
    n += lcd->print(' ');
  }
  return std::move(lcd);
}

std::unique_ptr < CharLCD_STM32F >
    Countdown_View::write_second_line(std::unique_ptr < CharLCD_STM32F >
					     lcd)
{
  /* Assumes the lcd position is at the start of the line. */
  int space = width_ - label_2.size();
  int n = 0;
  while (n < space / 2) {
    n += lcd->print(' ');
  }
  n += lcd->print(label_2);
  while (n < width_) {
    n += lcd->print(' ');
  }
  return std::move(lcd);
}

std::unique_ptr < CharLCD_STM32F >
    Countdown_View::write_third_line(std::unique_ptr < CharLCD_STM32F >
					     lcd)
{
  /* Assumes the lcd position is at the start of the line. */
  int n = 0;
  
  lcd->print( days );
  lcd->print( " days and" );


  while (n < width_) {
    n += lcd->print(' ');
  }
  return std::move(lcd);
}

std::unique_ptr < CharLCD_STM32F >
    Countdown_View::write_fourth_line(std::unique_ptr < CharLCD_STM32F >
					     lcd)
{
  /* Assumes the lcd position is at the start of the line. */
  int n = 0;
  sexagesimal::Sexagesimal hms{ hours, minutes, seconds, xxx };
  n += lcd->print( hms.to_string() );
  while (n < width_) {
    n += lcd->print(' ');
  }
  return std::move(lcd);
}

/*************************************
 * Alignment_Sights_View methods
 **************************************/

Alignment_Sights_View::Alignment_Sights_View( Alignment_Data_Set* data ){
  data_set = data;
  finished = false;
  // _prompt_for_new_star_sight = false;
  //_prompt_for_new_planet_sight = false;
  clear_prompts();
  width_ = INPUT_VIEW_DEFAULT_WIDTH;
  position = 0;
  saved_cr = dsc_controller::get_character_reciever();
  dsc_controller::set_character_reciever(this);
} 

Alignment_Sights_View::~Alignment_Sights_View( ){
  dsc_controller::set_character_reciever(saved_cr);
} 

void Alignment_Sights_View::put_char(char c)
{
 switch( c ){
 case scroll_up_char:
   scroll_up();
   return;
 case scroll_down_char:
   scroll_down();
   return;
 case select_char:
   select_char_action();
   return;
 case keypad_return_char:
   return_char_action();
   return;
 case '*':
   asterix_char_action();
   return;
 case keypad_backspace_char:
   delete_with_confirm();
   return;
 }
 return;
}

void Alignment_Sights_View::scroll_up(){
  if( position > 0 ){
    --position;
  }
}

void Alignment_Sights_View::scroll_down(){
  if( position < size() - 1 ){
    ++position;
  }
}

void Alignment_Sights_View::trim_position(){
  if( position > size() - 1 ){
    position = size() - 1;
  }
}

uint32_t Alignment_Sights_View::get_position(){
  return position;
}


void Alignment_Sights_View::delete_with_confirm(){
  _delete_item_with_confirm = true;
}

void Alignment_Sights_View::delete_without_confirm(){

}


void Alignment_Sights_View::select_char_action(){
  _prompt_for_new_planet_sight = true;
}

void Alignment_Sights_View::clear_prompts(){
  _prompt_for_new_planet_sight = false;
  _prompt_for_new_star_sight = false;
  _delete_item_with_confirm = false;
}

void Alignment_Sights_View::return_char_action(){
  finished = true;
}

void Alignment_Sights_View::asterix_char_action(){
  _prompt_for_new_star_sight = true;
}

size_t Alignment_Sights_View::size(){
  return data_set->size();
}

/* 
 * The longest common name is
 * Rigil Kentaurus 
 * 123456789012345  (15 characters)
*/
std::unique_ptr < CharLCD_STM32F >
    Alignment_Sights_View::write_first_line(std::unique_ptr < CharLCD_STM32F >
					     lcd)
{
  /* Assumes the lcd position is at the start of the line. */
  if( size() == 0 ){
    lcd->print( "      Empty List    " );
    return std::move(lcd);
  }
  uint32_t item = position;
  if( item < size() ){
    int n = 0;
    n += lcd->print("--> ");
    n += lcd->print( data_set->get_object_name(item) );
    while (n < width_) {
      n += lcd->print(' ');
    }
    return std::move(lcd);
  }else{
    lcd = write_blank_line( std::move(lcd) );
    return std::move(lcd);
  }
}

std::unique_ptr < CharLCD_STM32F >
    Alignment_Sights_View::write_second_line(std::unique_ptr < CharLCD_STM32F >
					     lcd)
{
  /* Assumes the lcd position is at the start of the line. */
  uint32_t item = position;
  if( item < size() ){
    double JD = data_set->get_julian_date(item);
    lcd = write_date_line( std::move(lcd), JD );
    return std::move(lcd);
  }else{
    lcd = write_blank_line( std::move(lcd) );
    return std::move(lcd);
  }
}

std::unique_ptr < CharLCD_STM32F >
    Alignment_Sights_View::write_third_line(std::unique_ptr < CharLCD_STM32F >
					     lcd)
{
  /* Assumes the lcd position is at the start of the line. */
  uint32_t item = position + 1;
  if( item < size() ){
    int n = 0;
    n += lcd->print("--> ");
    n += lcd->print( data_set->get_object_name(item) );
    while (n < width_) {
      n += lcd->print(' ');
    }
    return std::move(lcd);
  }else{
    if( size() < 2 ){
      lcd = write_text_line( std::move(lcd), "*) Star Sight" );
    }else{
      lcd = write_blank_line( std::move(lcd) );
    }
    return std::move(lcd);    
  }
}

std::unique_ptr < CharLCD_STM32F >
    Alignment_Sights_View::write_fourth_line(std::unique_ptr < CharLCD_STM32F >
					     lcd)
{
  /* Assumes the lcd position is at the start of the line. */
  uint32_t item = position + 1;
  if( item < size() ){
    double JD = data_set->get_julian_date(item);
    lcd = write_date_line( std::move(lcd), JD );
    return std::move(lcd);
  }else{
    if( size() < 2 ){
      lcd = write_text_line( std::move(lcd), "B) Planet Sight" );
    }else{
      lcd = write_blank_line( std::move(lcd) );
    }
    return std::move(lcd);    
  }
}

std::unique_ptr < CharLCD_STM32F >
Alignment_Sights_View::write_blank_line(std::unique_ptr < CharLCD_STM32F > lcd )
{
  int n = 0; 
  while (n < width_) {
    n += lcd->print(' ');
  }
  return std::move(lcd);
}

std::unique_ptr < CharLCD_STM32F >
Alignment_Sights_View::write_text_line(std::unique_ptr < CharLCD_STM32F > lcd, std::string text )
{
  int n = 0; 
  n += lcd->print( text );
  while (n < width_) {
    n += lcd->print(' ');
  }
  return std::move(lcd);
}

std::unique_ptr < CharLCD_STM32F > 
  Alignment_Sights_View::write_date_line(std::unique_ptr <CharLCD_STM32F> lcd, double jd)
{
  CAADate date_time{jd,true};
  long year = 0;
  long month = 0;
  long day = 0;
  long hour = 0;
  long minute = 0;
  double second = 0;
  date_time.Get(year, month, day, hour, minute, second);
  int n = 0;
  std::string num;
  //n += lcd->print( date.Year() );
  //n += lcd->print( ' ' );
  num = sexagesimal::to_string_hack(static_cast<uint32_t>(month));
  while( num.size() < 2 ){
    num = '0' + num;
  }
  n += lcd->print( num );
  n += lcd->print( ' ' );

  num = sexagesimal::to_string_hack(static_cast<uint32_t>(day));
  while( num.size() < 2 ){
    num = '0' + num;
  }
  n += lcd->print( num );
  n += lcd->print( ' ' );  

  num = sexagesimal::to_string_hack(static_cast<uint32_t>(hour));
  while( num.size() < 2 ){
    num = '0' + num;
  }
  n += lcd->print( num );
  n += lcd->print( ':' );  

  num = sexagesimal::to_string_hack(static_cast<uint32_t>(minute));
  while( num.size() < 2 ){
    num = '0' + num;
  }
  n += lcd->print( num );
  n += lcd->print( ':' ); 

  n += lcd->print( second, 1 );  /* Printing one decimal place indicates subsecond accuracy. */ 
  while (n < width_) {
    n += lcd->print(' ');
  }
  return std::move(lcd);
}


/***************************************
 *  Planetary Details View
 ***************************************/
//                         Planetary_Details_View

Planetary_Details_View::Planetary_Details_View( int planet_num  ){
  planet_num_ = planet_num;
  finished = false;
  width_ = INPUT_VIEW_DEFAULT_WIDTH;
  saved_cr = dsc_controller::get_character_reciever();
  dsc_controller::set_character_reciever(this);
  body_name = solar_system::solar_system_body_name( planet_num_ );
}

Planetary_Details_View::~Planetary_Details_View(){
  dsc_controller::set_character_reciever(saved_cr);
}

void Planetary_Details_View::put_char( char c ){
  if( c == keypad_return_char ){
    dismiss_action();
  }
}

void Planetary_Details_View::dismiss_action( ){
  finished = true;
}

std::unique_ptr < CharLCD_STM32F > Planetary_Details_View::write_first_line(std::unique_ptr <
									      CharLCD_STM32F > lcd)
{
  JD = JD_timestamp_pretty_good_000();
  if ( planet_num_ == 3) {
    CAA3DCoordinate RA_Dec_Dist = solar_system::calculate_moon_RA_Dec_Dist(JD);
    RA_Dec.X = RA_Dec_Dist.X;
    RA_Dec.Y = RA_Dec_Dist.Y;
    distance = RA_Dec_Dist.Z; // KM
  } else {
    CAAEllipticalPlanetaryDetails details =
	solar_system::calculate_details(body_name, JD);
    RA_Dec.X = details.ApparentGeocentricRA;
    RA_Dec.Y = details.ApparentGeocentricDeclination;
    distance = details.ApparentGeocentricDistance; // A.U. 
  }
  int n = 0;
  int space = (width_ - body_name.size())/2;
  while( n < space ){
    n += lcd->print( ' ' );
  }
  n += lcd->print( body_name );
  while (n < width_) {
    n += lcd->print(' ');
  } 
  return std::move(lcd);
}

std::unique_ptr < CharLCD_STM32F > Planetary_Details_View::write_second_line(std::unique_ptr <
									      CharLCD_STM32F > lcd)
{
  int n = 0;
  n += lcd->print( " RA " );
  n += lcd->print( sexagesimal::Sexagesimal(RA_Dec.X).to_string());
  while (n < width_) {
    n += lcd->print(' ');
  } 
  return std::move(lcd);
}

std::unique_ptr < CharLCD_STM32F > Planetary_Details_View::write_third_line(std::unique_ptr <
									      CharLCD_STM32F > lcd)
{
  int n = 0;
  n += lcd->print( "DEC " );
  n += lcd->print( sexagesimal::Sexagesimal(RA_Dec.Y).to_string());
  while (n < width_) {
    n += lcd->print(' ');
  } 
  return std::move(lcd);
}

std::unique_ptr < CharLCD_STM32F > Planetary_Details_View::write_fourth_line(std::unique_ptr <
									       CharLCD_STM32F > lcd)
{
  int n = 0;
  n += lcd->print( "Distance " );
  n += lcd->print( distance, 3 );
  if ( planet_num_ == 3) {
    n += lcd->print( " km" );
  }else{
    n += lcd->print( " AU" );
  }
  while (n < width_) {
    n += lcd->print(' ');
  } 
  return std::move(lcd);
}

/*******************************************
 * NGC_Details_View
 *********************************************/

NGC_Details_View::NGC_Details_View( int num  ){
  finished = false;
  width_ = INPUT_VIEW_DEFAULT_WIDTH;
  saved_cr = dsc_controller::get_character_reciever();
  dsc_controller::set_character_reciever(this);
  setup( num );
}

NGC_Details_View::~NGC_Details_View(){
  dsc_controller::set_character_reciever(saved_cr);
}

void NGC_Details_View::put_char( char c ){
  if( c == keypad_return_char ){
    dismiss_action();
  }
}

void NGC_Details_View::dismiss_action( ){
  finished = true;
}

void NGC_Details_View::setup( int num ){
  ngc_num_ = num;
  index = ngc_objects::get_index( num );
  if( index != -1 ){
    magnitude = ngc_objects::get_magnitude_i( index );
    dimension_a = ngc_objects::get_dimension_a_i( index );
    dimension_b = ngc_objects::get_dimension_b_i( index );
    sexagesimal::Sexagesimal RA = ngc_objects::get_RA_i(index);
    sexagesimal::Sexagesimal Dec = ngc_objects::get_Dec_i(index);
    RA_Dec.X = RA.to_double();
    RA_Dec.Y = Dec.to_double();
    /* @TODO perform precesion transformation. */


  }else{
    magnitude = 0.0;
    dimension_a = 0.0;
    dimension_b = 0.0;
    RA_Dec.X = 0.0;
    RA_Dec.Y = 0.0;
  }
}

std::unique_ptr < CharLCD_STM32F > NGC_Details_View::write_first_line(std::unique_ptr <
									      CharLCD_STM32F > lcd)
{
  int n = 0;
  n += lcd->print( "NGC " );
  n += lcd->print( ngc_num_ );
  while (n < width_/2 -1 ) {
    n += lcd->print(' ');
  } 
  n += lcd->print( "Vmag = " );
  n += lcd->print( magnitude, 2 );
  while (n < width_ ) {
    n += lcd->print(' ');
  } 
  return std::move(lcd);
}

std::unique_ptr < CharLCD_STM32F > NGC_Details_View::write_second_line(std::unique_ptr <
									      CharLCD_STM32F > lcd)
{
  int n = 0;
  n += lcd->print( " RA " );
  n += lcd->print( sexagesimal::Sexagesimal(RA_Dec.X).to_string());
  while (n < width_) {
    n += lcd->print(' ');
  } 
  return std::move(lcd);
}

std::unique_ptr < CharLCD_STM32F > NGC_Details_View::write_third_line(std::unique_ptr <
									      CharLCD_STM32F > lcd)
{
  int n = 0;
  n += lcd->print( "DEC " );
  n += lcd->print( sexagesimal::Sexagesimal(RA_Dec.Y).to_string());
  while (n < width_) {
    n += lcd->print(' ');
  } 
  return std::move(lcd);
}

std::unique_ptr < CharLCD_STM32F > NGC_Details_View::write_fourth_line(std::unique_ptr <
									      CharLCD_STM32F > lcd)
{
  int n = 0;
  n += lcd->print( dimension_a, 2 );
  n += lcd->print( " X " );
  n += lcd->print( dimension_b, 2 );
  while (n < width_) {
    n += lcd->print(' ');
  } 
  return std::move(lcd);
}
