#ifndef INPUT_VIEWS_H
#define INPUT_VIEWS_H

#include "character_reciever.h"
#include <string>
#include <memory>
#include <vector>
#include "sexagesimal.h"
#include "telescope_model.h"

// #include "CharLCD_STM32F.h"
class CharLCD_STM32F;
class Telescope_Model;

/* Widgetoid for inputing an integer, showing the partial 
 * entry on an LCD
 **/

constexpr uint32_t INPUT_VIEW_DEFAULT_WIDTH = 20;  /* The width of the display in characters */


class Integer_Input_View:Character_Reciever {
 public:
  Integer_Input_View(std::string label);
  Integer_Input_View(std::string label, int value);
  ~Integer_Input_View();
  void put_char(char);

  void set_value(int32_t);
  const int32_t get_value();
  void change_sign();
  void set_width(int32_t);
  void enter_char_action();
  void backspace_char_action();
   std::unique_ptr < CharLCD_STM32F > write_first_line(std::unique_ptr <
						       CharLCD_STM32F >);
   std::unique_ptr < CharLCD_STM32F > write_second_line(std::unique_ptr <
							CharLCD_STM32F >);

  //void write_second_line(CharLCD_STM32F*);
  inline bool is_finished() {
    return finished;
  } 

 private:
  Character_Reciever* saved_cr;
  int32_t _width;
  int32_t _value;
  std::string _label;
  bool is_two_line;
  bool finished;
};


class Confirm_Input_View:Character_Reciever {
 public:
  Confirm_Input_View();
  ~Confirm_Input_View();
  void put_char(char c);
  void set_text( std::string text );
  void set_true_text( std::string text );
  void set_false_text( std::string text );

  void enter_char_action();
  void backspace_char_action();
   std::unique_ptr < CharLCD_STM32F > write_first_line(std::unique_ptr <
						       CharLCD_STM32F >);
   std::unique_ptr < CharLCD_STM32F > write_second_line(std::unique_ptr <
							CharLCD_STM32F >);

  inline bool is_finished() {
    return finished;
  } 
  bool get_is_okay();


 private:
  Character_Reciever* saved_cr;
  bool is_okay;
  int32_t width_;
  std::string text_;
  std::string true_text_;
  std::string false_text_;
  bool finished;
};





//     Sexagesimal( int32_t hhh, uint8_t mm, uint8_t ss, uint16_t xxx);
class Sexagesimal_Input_View:Character_Reciever {
 public:
  Sexagesimal_Input_View(std::string label);
  ~Sexagesimal_Input_View();
  void put_char(char);
  sexagesimal::Sexagesimal get_value();
  void change_sign();
  void make_negative();
  void make_positive();
  void set_width(int32_t);
  void set_plus_char(char);
  void set_minus_char(char);
  void enter_char_action();
  void backspace_char_action();
  void increment_position();
  void decrement_position();
  void set_msd_digits(uint8_t);
  void set_center_label(bool);
   std::unique_ptr < CharLCD_STM32F > write_first_line(std::unique_ptr <
						       CharLCD_STM32F >);
   std::unique_ptr < CharLCD_STM32F > write_second_line(std::unique_ptr <
							CharLCD_STM32F >);
   std::unique_ptr < CharLCD_STM32F > write_third_line(std::unique_ptr <
						       CharLCD_STM32F >);
   std::unique_ptr < CharLCD_STM32F > write_fourth_line(std::unique_ptr <
							CharLCD_STM32F >);
  inline bool is_finished() {
    return finished;
  } 
  inline int32_t get_cursor_column() {
    return cursor_column;
  }
 private:
  Character_Reciever* saved_cr;
  void stuff_digit(char);
  void hhh_read();
  void mm_read();
  void ss_read();
  void subseconds_read();
  std::string _label;
  int32_t _width;
  int32_t cursor_column;
  int32_t hhh;
  uint8_t mm;
  uint8_t ss;
  uint8_t msd_digits;
  float subseconds;
  std::string hhh_str;
  std::string mm_str;
  std::string ss_str;
  std::string subseconds_str;
  //std::string hhh_write_str;
  //std::string mm_write_str;
  //std::string ss_write_str;
  //std::string subseconds_write_str;
  uint8_t position;		/* 0 for hours, 1 for minutes, 2 for seconds, 3 for subseconds. */
  bool is_negative;
  char plus_char;
  char minus_char;
  bool is_two_line;
  bool center_label;
  bool finished;
};

/****************************************************
 *
 * Burnham_Format_Input_View
 *
 * Burnham gives the coordinates in this format:
 * 22115s2119 ==> RA 22h 11.5m  Dec -21(deg) 19(min).
 * 06078n4844 ==> RA  6h 07.8m  Dec +48(deg) 44(min).
 * 
 * The get methods return the input values.  For pointing purposes,
 * if the values actually are those of Burham's handbook, precession from
 * 1950 to the present epoch should be applied.
 ***************************************************/

class Burnham_Format_Input_View: public Character_Reciever{
 public:
  Burnham_Format_Input_View();
  ~Burnham_Format_Input_View();
  std::unique_ptr < CharLCD_STM32F > write_first_line(std::unique_ptr <
						      CharLCD_STM32F >);
  std::unique_ptr < CharLCD_STM32F > write_second_line(std::unique_ptr <
						       CharLCD_STM32F >);
  void put_char( char );

  inline bool is_finished() {
    return finished;
  }
  double get_RA();
  double get_Declination();
  CAA2DCoordinate get_RA_and_Dec();
 private:
  std::string label_;
  int32_t width_;
  void put_digit( char );
  char cursor_char;
  char plus_char;
  char minus_char;
  void toggle_plus_minus();
  void make_positive();
  void make_negative();
  bool finished;
  char char_buffer[11];
  uint32_t position;
  void increment_position();
  void decrement_position();
  Character_Reciever* saved_cr;
};




/* 
 * After the user has identified an object for a sight alignment this view 
 * prompts the user to point the telescope and press #.  When # is pressed
 * both a timestamp and a set of encoder values are recorded.
 * 
 **/
class Alignment_Timestamp_Prompt:public Character_Reciever {
 public:
  Alignment_Timestamp_Prompt( Simple_Altazimuth_Scope*, std::string object);
  ~Alignment_Timestamp_Prompt();
  std::unique_ptr < CharLCD_STM32F > write_first_line(std::unique_ptr <
						      CharLCD_STM32F >);
  std::unique_ptr < CharLCD_STM32F > write_second_line(std::unique_ptr <
						       CharLCD_STM32F >);
  std::unique_ptr < CharLCD_STM32F > write_third_line(std::unique_ptr <
						      CharLCD_STM32F >);
  std::unique_ptr < CharLCD_STM32F > write_fourth_line(std::unique_ptr <
						       CharLCD_STM32F >);
  void put_char(char);
  void grab_timestamp_data();
  double get_timestamp();
  Alt_Azi_Snapshot_t get_encoder_data();
  inline bool is_finished() {
    return finished;
  }
  bool has_data();
  std::string get_object_name();
 private:
  double timestamp;      /* Initialized to zero.  Set at timestamp. */
  Simple_Altazimuth_Scope* telescope;
  int32_t _width;
  Character_Reciever* saved_cr;
  Alt_Azi_Snapshot_t encoder_data;  /* Set at timestamp. */
  std::string prompt_text_1;
  std::string object_name;
  std::string prompt_text_3;
  std::string prompt_text_4;
  bool _has_data;
  bool finished;
};

#endif				/*  INPUT_VIEWS_H  */
