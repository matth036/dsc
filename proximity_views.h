#ifndef PROXIMITY_VIEWS_H_
#define PROXIMITY_VIEWS_H_

#include <string>
#include <vector>
#include <memory>
#include "character_reciever.h"
#include "alignment_sight_info.h"
#include "AA2DCoordinate.h"
class CharLCD_STM32F;

constexpr uint32_t PROXIMITY_VIEW_DEFAULT_WIDTH = 20;   /* The width of the display in characters */

/* 
 * Takes a pointer to Simple_Altazimuth_Scope 
 * (Hope to abstact the concept of "Telescope Mount" eventually.)
 * 
 * Displays nearby stars from the Bright Star Catalog.
 */

class Proximate_Stars_View:Character_Reciever {
 public:
  Proximate_Stars_View(Simple_Altazimuth_Scope *scope);
  ~Proximate_Stars_View();
  void put_char(char);
   std::unique_ptr < CharLCD_STM32F > write_first_line(std::unique_ptr <
                                                       CharLCD_STM32F >);
   std::unique_ptr < CharLCD_STM32F > write_second_line(std::unique_ptr <
                                                        CharLCD_STM32F >);
   std::unique_ptr < CharLCD_STM32F > write_third_line(std::unique_ptr <
                                                       CharLCD_STM32F >);
   std::unique_ptr < CharLCD_STM32F > write_fourth_line(std::unique_ptr <
                                                        CharLCD_STM32F >);

  void dismiss_action();
  inline bool is_finished() {
    return finished;
  }
  inline bool prompt_for_magnitude() {
    return _prompt_for_magnitude;
  }
  char get_first_digit();
  void clear_prompts();
  void set_magnitude_limit( float mag );
  void increment_position();
  void decrement_position();
  void set_size(uint32_t s);

 private:
  std::unique_ptr < CharLCD_STM32F > write_line_n(std::unique_ptr <
                                                  CharLCD_STM32F >, uint32_t n);
  void push_pushto_command();
  void run_algorithm();
  void request_magnitude( char c );
  std::vector < uint32_t > stars;
  double JD;
  float _magnitude_limit = 6.0;
  Character_Reciever *saved_cr;
  uint32_t size = 99;
  uint32_t position = 0;
  uint32_t bsc_selection;
  int32_t width_;
  Simple_Altazimuth_Scope *telescope;
  CAA2DCoordinate RA_and_Dec;
  CAA2DCoordinate RA_and_Dec_J2000;
  char _first_digit;
  bool finished;
  bool _prompt_for_magnitude = false;
};

class Proximate_Messier_Objects_View:Character_Reciever {
 public:
  Proximate_Messier_Objects_View(Simple_Altazimuth_Scope *);
  ~Proximate_Messier_Objects_View();
  void put_char(char);
   std::unique_ptr < CharLCD_STM32F > write_first_line(std::unique_ptr <
                                                       CharLCD_STM32F >);
   std::unique_ptr < CharLCD_STM32F > write_second_line(std::unique_ptr <
                                                        CharLCD_STM32F >);
   std::unique_ptr < CharLCD_STM32F > write_third_line(std::unique_ptr <
                                                       CharLCD_STM32F >);
   std::unique_ptr < CharLCD_STM32F > write_fourth_line(std::unique_ptr <
                                                        CharLCD_STM32F >);

  void dismiss_action();
  inline bool is_finished() {
    return finished;
  }
  void increment_position();
  void decrement_position();
  void set_size(uint32_t s);

 private:
  std::unique_ptr < CharLCD_STM32F > write_line_n(std::unique_ptr <
                                                  CharLCD_STM32F >, uint32_t n);

  void run_algorithm();
  void push_pushto_command();
  std::vector < uint32_t > objects;
  double JD;
  size_t lambda_size;
  Character_Reciever *saved_cr;
  uint32_t size = 15;
  uint32_t position = 0;
  uint32_t messier_selection;
  int32_t width_;
  Simple_Altazimuth_Scope *telescope;
  CAA2DCoordinate RA_and_Dec;
  bool finished;
};

class Proximate_Navigation_Stars_View:Character_Reciever {
 public:
  explicit Proximate_Navigation_Stars_View(Simple_Altazimuth_Scope *scope);
  ~Proximate_Navigation_Stars_View();
  void put_char(char);
   std::unique_ptr < CharLCD_STM32F > write_first_line(std::unique_ptr <
                                                       CharLCD_STM32F >);
   std::unique_ptr < CharLCD_STM32F > write_second_line(std::unique_ptr <
                                                        CharLCD_STM32F >);
   std::unique_ptr < CharLCD_STM32F > write_third_line(std::unique_ptr <
                                                       CharLCD_STM32F >);
   std::unique_ptr < CharLCD_STM32F > write_fourth_line(std::unique_ptr <
                                                        CharLCD_STM32F >);

  void dismiss_action();
  inline bool is_finished() {
    return finished;
  }
  void increment_position();
  void decrement_position();
  void set_size(uint32_t s);

 private:
  std::unique_ptr < CharLCD_STM32F > write_line_n(std::unique_ptr <
                                                  CharLCD_STM32F >, uint32_t n);

  void run_algorithm();
  void push_pushto_command();
  std::vector < uint32_t > stars;
  double JD;
  size_t lambda_size;
  Character_Reciever *saved_cr;
  uint32_t size = 58;
  uint32_t position = 0;
  uint32_t navstar_selection;
  int32_t width_;
  Simple_Altazimuth_Scope *telescope;
  CAA2DCoordinate RA_and_Dec;
  CAA2DCoordinate RA_and_Dec_J2000;
  bool finished;
};

class Proximate_NGC_View:Character_Reciever {
 public:
  Proximate_NGC_View(Simple_Altazimuth_Scope *scope);
  ~Proximate_NGC_View();
  void put_char(char);
   std::unique_ptr < CharLCD_STM32F > write_first_line(std::unique_ptr <
                                                       CharLCD_STM32F >);
   std::unique_ptr < CharLCD_STM32F > write_second_line(std::unique_ptr <
                                                        CharLCD_STM32F >);
   std::unique_ptr < CharLCD_STM32F > write_third_line(std::unique_ptr <
                                                       CharLCD_STM32F >);
   std::unique_ptr < CharLCD_STM32F > write_fourth_line(std::unique_ptr <
                                                        CharLCD_STM32F >);

  void dismiss_action();
  inline bool is_finished() {
    return finished;
  }
  inline bool prompt_for_magnitude() {
    return _prompt_for_magnitude;
  }
  char get_first_digit();
  void clear_prompts();
  void set_magnitude_limit( float mag );
  void increment_position();
  void decrement_position();
  void set_size(uint32_t s);

 private:
  std::unique_ptr < CharLCD_STM32F > write_line_n(std::unique_ptr <
                                                  CharLCD_STM32F >, uint32_t n);
  void push_pushto_command();
  void run_algorithm();
  void request_magnitude( char c );
  std::vector < uint32_t > ngc_objects;
  double JD;
  float _magnitude_limit = 9.999;
  Character_Reciever *saved_cr;
  uint32_t size = 99;
  uint32_t position = 0;
  uint32_t ngc_selection;
  int32_t width_;
  Simple_Altazimuth_Scope *telescope;
  CAA2DCoordinate RA_and_Dec;
  char _first_digit;
  bool finished;
  bool _prompt_for_magnitude = false;
};

#endif  // PROXIMITY_VIEWS_H_
