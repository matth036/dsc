#ifndef _PROXIMITITY_VIEWS_H
#define _PROXIMITITY_VIEWS_H

#include <string>
#include <vector>
#include <memory>
class CharLCD_STM32F;
#include "character_reciever.h"
#include "alignment_sight_info.h"
#include "AA2DCoordinate.h"

constexpr uint32_t PROXIMITY_VIEW_DEFAULT_WIDTH = 20;  /* The width of the display in characters */

/* 
 * Takes a pointer to Simple_Altazimuth_Scope 
 * (Hope to abstact the concept of "Telescope Mount" eventually.)
 * 
 * Displays nearby stars from the Bright Star Catalog.
 */

class Proximate_Stars_View:Character_Reciever {
 public:
  Proximate_Stars_View( Simple_Altazimuth_Scope* );
  ~Proximate_Stars_View( );
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
  inline bool is_finished(){
    return finished;
  }

  void increment_position();
  void decrement_position();
  void set_size( uint32_t s );

 private:
  void run_algorithm();
  std::vector<uint32_t> stars;
  double JD;
  size_t lambda_size;
  Character_Reciever* saved_cr;
  uint32_t size = 99;
  uint32_t position = 0; 
  int32_t width_;
  Simple_Altazimuth_Scope* telescope;
  CAA2DCoordinate RA_and_Dec;
  bool finished;
};



#endif  /*  _PROXIMITITY_VIEWS_H  */
