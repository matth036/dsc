#include "proximity_views.h"

#include <string>
#include "controller.h"
#include "keypad_layout.h"
#include "char_lcd_stm32f4.h"
#include "rtc_management.h"
#include "starlist_access.h"
#include "messier.h"
#include "navigation_star.h"
#include "AAAngularSeparation.h"
#include "AACoordinateTransformation.h"
#include "sexagesimal.h"


/* 
 * single precision version of CAAAngularSeparation::Separation( ,,, )
 *
 */
float separation(float Alpha1, float Delta1, float Alpha2, float Delta2)
{
  /* Expect the compiler to recognize the right hand side multiplier as a constant. */
  Delta1 *=
      static_cast < float >(CAACoordinateTransformation::DegreesToRadians(1.0));
  Delta2 *=
      static_cast < float >(CAACoordinateTransformation::DegreesToRadians(1.0));

  Alpha1 *=
      static_cast < float >(CAACoordinateTransformation::HoursToRadians(1.0));
  Alpha2 *=
      static_cast < float >(CAACoordinateTransformation::HoursToRadians(1.0));

  float delta_Alpha = Alpha2 - Alpha1;
  float cos_d2 = cosf(Delta2);
  float cc_temp = cos_d2 * cosf(delta_Alpha);
  float cos_d1 = cosf(Delta1);
  float sin_d1 = sinf(Delta1);
  float sin_d2 = sinf(Delta2);
  float x = cos_d1 * sin_d2 - sin_d1 * cc_temp;
  float y = cos_d2 * sinf(delta_Alpha);
  float z = sin_d1 * sin_d2 + cos_d1 * cc_temp;

  float value = atan2f(sqrtf(x * x + y * y), z);
  value *=
      static_cast < float >(CAACoordinateTransformation::RadiansToDegrees(1.0));
  if (value < 0)
    value += 180;

  return value;
}

Proximate_Stars_View::Proximate_Stars_View(Simple_Altazimuth_Scope * scope)
{
  telescope = scope;
  finished = false;
  width_ = PROXIMITY_VIEW_DEFAULT_WIDTH;

  saved_cr = dsc_controller::get_character_reciever();
  dsc_controller::set_character_reciever(this);
}

Proximate_Stars_View::~Proximate_Stars_View()
{
  dsc_controller::set_character_reciever(saved_cr);
}

void Proximate_Stars_View::put_char(char c)
{
  switch (c) {
  case keypad_return_char:
    dismiss_action();
    return;
  case scroll_up_char:
    decrement_position();
    return;
  case scroll_down_char:
    increment_position();
    return;
  case '*':
  case 'B':
    push_pushto_command();
    return;
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
  case '0':
    input_magnitude_limit( c );
    return;
  }
}

void Proximate_Stars_View::dismiss_action()
{
  finished = true;
}

void Proximate_Stars_View::set_size(uint32_t s)
{
  if (size < 4) {
    return;
  }
  size = s;
  if (position >= size) {
    position = size - 1;
  }
}

void Proximate_Stars_View::increment_position()
{
  if (position + 4 < size) {
    ++position;
  }
}

void Proximate_Stars_View::decrement_position()
{
  if (position > 0) {
    --position;
  }
}

void Proximate_Stars_View::push_pushto_command(){
  std::string cmd = "B*C";
  cmd += sexagesimal::to_string_hack(bsc_selection);
  dsc_controller::push_cmd_buffer(cmd);
  dismiss_action();
}

void Proximate_Stars_View::input_magnitude_limit( char c ){

}

void Proximate_Stars_View::run_algorithm()
{
  stars.clear();
  uint32_t index = 0;
  while (stars.size() < size && index < starlist_access::starlist_size()) {
    /* We may want to filter for magnitude here. */
    stars.push_back(index);
    ++index;
  }
  /* @TODO convert to J2000 RA and Declination */
  float target_RA = RA_and_Dec.X;
  float target_Dec = RA_and_Dec.Y;
  auto distance_from_target =
      [target_RA, target_Dec] (float subject_RA, float subject_Dec){
    return separation(target_RA, target_Dec, subject_RA, subject_Dec);
  };
  double JD = this->JD;
  auto bsc_distance_from_target =[JD, distance_from_target] (uint32_t index) {
    CAA2DCoordinate position =
        starlist_access::proper_motion_adjusted_position(index, JD);
    return distance_from_target(position.X, position.Y);
  };
  auto target_proximity_compare =
      [bsc_distance_from_target] (const uint32_t a, const uint32_t b){
    auto distance_a = bsc_distance_from_target(a);
    auto distance_b = bsc_distance_from_target(b);
    return distance_a < distance_b;
  };
  /* The algoritm proper begins here. */
  make_heap(stars.begin(), stars.end(), target_proximity_compare);
  /* 
   *   Now *stars.begin() has the largest distance_from_target() 
   *   and, the rest of the list has the heap propery.
   */
  while (index < starlist_access::starlist_size()) {
    if (target_proximity_compare(index, *stars.begin())) {
      pop_heap(stars.begin(), stars.end(), target_proximity_compare);
      stars.pop_back();
      stars.push_back(index);
      push_heap(stars.begin(), stars.end(), target_proximity_compare);
    }
    ++index;
  }
  sort_heap(stars.begin(), stars.end(), target_proximity_compare);
  bsc_selection = stars[position];
}

std::unique_ptr < CharLCD_STM32F >
    Proximate_Stars_View::write_first_line(std::unique_ptr < CharLCD_STM32F >
                                           lcd)
{
  JD = JD_timestamp_pretty_good_000();
  RA_and_Dec = telescope->current_RA_and_Dec();
  run_algorithm();
  return std::move(write_line_n(std::move(lcd), 1));
}

std::unique_ptr < CharLCD_STM32F >
    Proximate_Stars_View::write_second_line(std::unique_ptr < CharLCD_STM32F >
                                            lcd)
{
  return std::move(write_line_n(std::move(lcd), 2));
}

std::unique_ptr < CharLCD_STM32F >
    Proximate_Stars_View::write_third_line(std::unique_ptr < CharLCD_STM32F >
                                           lcd)
{
  return std::move(write_line_n(std::move(lcd), 3));
}

std::unique_ptr < CharLCD_STM32F >
    Proximate_Stars_View::write_fourth_line(std::unique_ptr < CharLCD_STM32F >
                                            lcd)
{
  return std::move(write_line_n(std::move(lcd), 4));
}

std::unique_ptr < CharLCD_STM32F >
    Proximate_Stars_View::write_line_n(std::unique_ptr < CharLCD_STM32F > lcd,
                                       uint32_t line_number)
{
  double target_RA = RA_and_Dec.X;
  double target_Dec = RA_and_Dec.Y;
  auto distance_from_target =
      [target_RA, target_Dec] (double subject_RA, double subject_Dec){
    return CAAAngularSeparation::Separation(target_RA, target_Dec, subject_RA,
                                            subject_Dec);
  };
  double JD = this->JD;
  auto bsc_distance_from_target =[JD, distance_from_target] (uint32_t index) {
    CAA2DCoordinate position =
        starlist_access::proper_motion_adjusted_position(index, JD);
    return distance_from_target(position.X, position.Y);
  };

  int n = 0;
  n += lcd->print(position + line_number);
  while (n < 3) {
    n += lcd->print(' ');
  }
  n += lcd->
      print(starlist_access::bsc_number(stars[position + line_number - 1]));
  while (n < 8) {
    n += lcd->print(' ');
  }
  n += lcd->print(starlist_access::magnitude(stars[position + line_number - 1]),
                  2);
  n += lcd->print("m ");
  while (n < 14) {
    n += lcd->print(' ');
  }
  n += lcd->print(bsc_distance_from_target(stars[position + line_number - 1]),
                  1);
  while (n < width_) {
    n += lcd->print(' ');
  }
  return std::move(lcd);
}

/*************************************************************************/

Proximate_Navigation_Stars_View::
Proximate_Navigation_Stars_View(Simple_Altazimuth_Scope * scope)
{
  telescope = scope;
  finished = false;
  width_ = PROXIMITY_VIEW_DEFAULT_WIDTH;

  saved_cr = dsc_controller::get_character_reciever();
  dsc_controller::set_character_reciever(this);
}

Proximate_Navigation_Stars_View::~Proximate_Navigation_Stars_View()
{
  dsc_controller::set_character_reciever(saved_cr);
}

void Proximate_Navigation_Stars_View::put_char(char c)
{
  switch (c) {
  case keypad_return_char:
    dismiss_action();
    return;
  case scroll_up_char:
    decrement_position();
    return;
  case scroll_down_char:
    increment_position();
    return;
  case '*':
  case 'B':
    push_pushto_command();
    return;
  }
}

void Proximate_Navigation_Stars_View::dismiss_action()
{
  finished = true;
}

void Proximate_Navigation_Stars_View::set_size(uint32_t s)
{
  if (size < 4) {
    return;
  }
  size = s;
  if (position >= size) {
    position = size - 1;
  }
}

void Proximate_Navigation_Stars_View::increment_position()
{
  if (position + 4 < size) {
    ++position;
  }
}

void Proximate_Navigation_Stars_View::decrement_position()
{
  if (position > 0) {
    --position;
  }
}

void Proximate_Navigation_Stars_View::push_pushto_command()
{
  std::string cmd = "A*";
  cmd += sexagesimal::to_string_hack(navstar_selection);
  dsc_controller::push_cmd_buffer(cmd);
  dismiss_action();
}

void Proximate_Navigation_Stars_View::run_algorithm()
{
  stars.clear();
  uint32_t navstar_number = 0;
  while (stars.size() < size
         && navstar_number <= navigation_star::NAVIGATION_LIST_MAX) {
    stars.push_back(navstar_number);
    ++navstar_number;
  }
  /* @TODO convert to J2000 RA and Declination */
  float target_RA = RA_and_Dec.X;
  float target_Dec = RA_and_Dec.Y;

  auto distance_from_target =
      [target_RA, target_Dec] (float subject_RA, float subject_Dec){
    return separation(target_RA, target_Dec, subject_RA, subject_Dec);
  };

  double JD = this->JD;

  auto navstar_distance_from_target =
      [JD, distance_from_target] (uint32_t navstar) {
    uint32_t bsc = navigation_star::nav2bsc[navstar];
    uint32_t index = starlist_access::get_index(bsc);
    CAA2DCoordinate position =
        starlist_access::proper_motion_adjusted_position(index, JD);
    return distance_from_target(position.X, position.Y);
  };

  auto target_proximity_compare =
      [navstar_distance_from_target] (const uint32_t a, const uint32_t b){
    auto distance_a = navstar_distance_from_target(a);
    auto distance_b = navstar_distance_from_target(b);
    return distance_a < distance_b;
  };

  /* The algoritm proper begins here. */
  make_heap(stars.begin(), stars.end(), target_proximity_compare);
  /* 
   *   Now *stars.begin() has the largest distance_from_target() 
   *   and the entire list has the heap propery.
   */
  while (navstar_number <= navigation_star::NAVIGATION_LIST_MAX) {
    if (target_proximity_compare(navstar_number, *stars.begin())) {
      pop_heap(stars.begin(), stars.end(), target_proximity_compare);
      stars.pop_back();
      stars.push_back(navstar_number);
      push_heap(stars.begin(), stars.end(), target_proximity_compare);
    }
    ++navstar_number;
  }
  sort_heap(stars.begin(), stars.end(), target_proximity_compare);
  navstar_selection = stars[position];
}

std::unique_ptr < CharLCD_STM32F >
    Proximate_Navigation_Stars_View::write_first_line(std::unique_ptr <
                                                      CharLCD_STM32F > lcd)
{
  JD = JD_timestamp_pretty_good_000();
  RA_and_Dec = telescope->current_RA_and_Dec();
  run_algorithm();
  return std::move(write_line_n(std::move(lcd), 1));
}

std::unique_ptr < CharLCD_STM32F >
    Proximate_Navigation_Stars_View::write_second_line(std::unique_ptr <
                                                       CharLCD_STM32F > lcd)
{
  return std::move(write_line_n(std::move(lcd), 2));
}

std::unique_ptr < CharLCD_STM32F >
    Proximate_Navigation_Stars_View::write_third_line(std::unique_ptr <
                                                      CharLCD_STM32F > lcd)
{
  return std::move(write_line_n(std::move(lcd), 3));
}

std::unique_ptr < CharLCD_STM32F >
    Proximate_Navigation_Stars_View::write_fourth_line(std::unique_ptr <
                                                       CharLCD_STM32F > lcd)
{
  return std::move(write_line_n(std::move(lcd), 4));
}

std::unique_ptr < CharLCD_STM32F >
    Proximate_Navigation_Stars_View::write_line_n(std::unique_ptr <
                                                  CharLCD_STM32F > lcd,
                                                  uint32_t line_number)
{
#if 0
  double target_RA = RA_and_Dec.X;
  double target_Dec = RA_and_Dec.Y;
  auto distance_from_target =
      [target_RA, target_Dec] (double subject_RA, double subject_Dec){
    return CAAAngularSeparation::Separation(target_RA, target_Dec, subject_RA,
                                            subject_Dec);
  };
  double JD = this->JD;
  auto navstar_distance_from_target =
      [JD, distance_from_target] (uint32_t navstar) {
    uint32_t bsc = navigation_star::nav2bsc[navstar];
    uint32_t index = starlist_access::get_index(bsc);
    CAA2DCoordinate position =
        starlist_access::proper_motion_adjusted_position(index, JD);
    return distance_from_target(position.X, position.Y);
  };
#endif
  int n = 0;
  n += lcd->print(position + line_number);
  while (n < 3) {
    n += lcd->print(' ');
  }
  n += lcd->print(stars[position + line_number - 1]);
  while (n < 6) {
    n += lcd->print(' ');
  }
  /* 6 chars plus "Rigel Kentaurus" will overflow the 20 character width.
   * All others fit.
   */
  n += lcd->
      print(navigation_star::
            get_navigation_star_name(stars[position + line_number - 1]));
  while (n < width_) {
    n += lcd->print(' ');
  }
  return std::move(lcd);
}

/*************************************************************************/

Proximate_Messier_Objects_View::
Proximate_Messier_Objects_View(Simple_Altazimuth_Scope * scope)
{
  telescope = scope;
  finished = false;
  width_ = PROXIMITY_VIEW_DEFAULT_WIDTH;

  saved_cr = dsc_controller::get_character_reciever();
  dsc_controller::set_character_reciever(this);
}

Proximate_Messier_Objects_View::~Proximate_Messier_Objects_View()
{
  dsc_controller::set_character_reciever(saved_cr);
}

void Proximate_Messier_Objects_View::put_char(char c)
{
  switch (c) {
  case keypad_return_char:
    dismiss_action();
    return;
  case scroll_up_char:
    decrement_position();
    return;
  case scroll_down_char:
    increment_position();
    return;
  case '*':
  case 'B':
    push_pushto_command();
    return;
  }
}

void Proximate_Messier_Objects_View::dismiss_action()
{
  finished = true;
}

void Proximate_Messier_Objects_View::set_size(uint32_t s)
{
  if (size < 4) {
    return;
  }
  size = s;
  if (position >= size) {
    position = size - 1;
  }
}

void Proximate_Messier_Objects_View::increment_position()
{
  if (position + 4 < size) {
    ++position;
  }
}

void Proximate_Messier_Objects_View::decrement_position()
{
  if (position > 0) {
    --position;
  }
}

void Proximate_Messier_Objects_View::push_pushto_command()
{
  std::string cmd = "AAC";
  cmd += sexagesimal::to_string_hack(messier_selection);
  dsc_controller::push_cmd_buffer(cmd);
  dismiss_action();
}

void Proximate_Messier_Objects_View::run_algorithm()
{
  objects.clear();
  uint32_t messier_number = 1;
  while (objects.size() < size && messier_number <= 110) {
    /* We may want to filter for magnitude here. */
    objects.push_back(messier_number);
    ++messier_number;
  }
  /* @TODO convert to J2000 RA and Declination */
  float target_RA = RA_and_Dec.X;
  float target_Dec = RA_and_Dec.Y;
  auto distance_from_target =
      [target_RA, target_Dec] (float subject_RA, float subject_Dec){
    return separation(target_RA, target_Dec, subject_RA, subject_Dec);
  };
  auto messier_distance_from_target =[distance_from_target] (uint32_t m_number) {
    bool OK = false;
    CAA2DCoordinate position;
    while (!OK) {
      position = messier_numbers::messier_J2000_RA_and_Dec(m_number, OK);
    }
    return distance_from_target(position.X, position.Y);
  };
  auto target_proximity_compare =
      [messier_distance_from_target] (const uint32_t a, const uint32_t b){
    auto distance_a = messier_distance_from_target(a);
    auto distance_b = messier_distance_from_target(b);
    return distance_a < distance_b;
  };
  lambda_size = sizeof(target_proximity_compare);
  /* The algoritm proper begins here. */
  make_heap(objects.begin(), objects.end(), target_proximity_compare);
  /* 
   *   Now *objects.begin() has the largest distance_from_target() 
   *   and, the rest of the list has the heap propery.
   */
  while (messier_number <= 110) {
    if (target_proximity_compare(messier_number, *objects.begin())) {
      pop_heap(objects.begin(), objects.end(), target_proximity_compare);
      objects.pop_back();
      objects.push_back(messier_number);
      push_heap(objects.begin(), objects.end(), target_proximity_compare);
    }
    ++messier_number;
  }
  sort_heap(objects.begin(), objects.end(), target_proximity_compare);
  messier_selection = objects[position];
}

std::unique_ptr < CharLCD_STM32F >
    Proximate_Messier_Objects_View::write_first_line(std::unique_ptr <
                                                     CharLCD_STM32F > lcd)
{
  JD = JD_timestamp_pretty_good_000();
  RA_and_Dec = telescope->current_RA_and_Dec();
  run_algorithm();
  return std::move(write_line_n(std::move(lcd), 1));
}

std::unique_ptr < CharLCD_STM32F >
    Proximate_Messier_Objects_View::write_second_line(std::unique_ptr <
                                                      CharLCD_STM32F > lcd)
{
  return std::move(write_line_n(std::move(lcd), 2));
}

std::unique_ptr < CharLCD_STM32F >
    Proximate_Messier_Objects_View::write_third_line(std::unique_ptr <
                                                     CharLCD_STM32F > lcd)
{
  return std::move(write_line_n(std::move(lcd), 3));
}

std::unique_ptr < CharLCD_STM32F >
    Proximate_Messier_Objects_View::write_fourth_line(std::unique_ptr <
                                                      CharLCD_STM32F > lcd)
{
  return std::move(write_line_n(std::move(lcd), 4));
}

std::unique_ptr < CharLCD_STM32F >
    Proximate_Messier_Objects_View::write_line_n(std::unique_ptr <
                                                 CharLCD_STM32F > lcd,
                                                 uint32_t line_number)
{
  double target_RA = RA_and_Dec.X;
  double target_Dec = RA_and_Dec.Y;
  auto distance_from_target =
      [target_RA, target_Dec] (double subject_RA, double subject_Dec){
    return CAAAngularSeparation::Separation(target_RA, target_Dec, subject_RA,
                                            subject_Dec);
  };
  auto messier_distance_from_target =[distance_from_target] (uint32_t m_number) {
    bool OK = false;
    CAA2DCoordinate position;
    while (!OK) {
      position = messier_numbers::messier_J2000_RA_and_Dec(m_number, OK);
    }
    return distance_from_target(position.X, position.Y);
  };
  int n = 0;
  n += lcd->print(position + line_number);
  while (n < 3) {
    n += lcd->print(' ');
  }
  std::string str = "M";
  str += sexagesimal::to_string_hack(objects[position + line_number - 1]);
  while (str.length() < 4) {
    str = " " + str;            // front pad with spaces.
  }
  n += lcd->print(str);
  while (n < 9) {
    n += lcd->print(' ');
  }
  while (n < 9) {
    n += lcd->print(' ');
  }
  n += lcd->
      print(messier_distance_from_target(objects[position + line_number - 1]),
            1);
  while (n < width_) {
    n += lcd->print(' ');
  }
  return std::move(lcd);
}
