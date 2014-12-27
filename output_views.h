#ifndef OUTPUT_VIEWS_H
#define OUTPUT_VIEWS_H

/* 
 * An output view will need character input for actions like 
 * "dismiss", "scroll_down", scroll_up".
 *
 */
#include "character_reciever.h"
#include <string>
#include <memory>
//#include <vector>
#include "sexagesimal.h"
#include "alignment_sight_info.h"
#include "AADate.h"

// #include "CharLCD_STM32F.h"
class CharLCD_STM32F;



class Information_View:Character_Reciever {
 public:
  Information_View( );
  ~Information_View( );
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
  void set_text_1( std::string );
  void set_text_2( std::string );
  void set_text_3( std::string );
  void set_text_4( std::string );
 private:
  std::string text_1;
  std::string text_2;
  std::string text_3;
  std::string text_4;
  Character_Reciever* saved_cr;
  int32_t width_;
  bool center_text;
  bool finished;
};

class Current_Time_View:Character_Reciever {
 public:
  Current_Time_View( );
  ~Current_Time_View( );
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
  static std::string month_string( int month );
 private:
  // double julian_date;
  CAADate JD;
  Character_Reciever* saved_cr;
  int32_t width_;
  bool finished;
};


class Angular_Coordinates_View:Character_Reciever {
 public:
  Angular_Coordinates_View( Simple_Altazimuth_Scope* );
  ~Angular_Coordinates_View( );
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
 private:
  CAA2Dfloat RA_and_Dec;
  std::string RA_str;
  std::string Dec_str;
  CAA2Dfloat Azi_and_Alt;
  //std::string Azi_str;
  //std::string Alt_str;

  Simple_Altazimuth_Scope* telescope;
  Character_Reciever* saved_cr;
  int32_t width_;
  bool finished;
};

class Pushto_Output_View: public Character_Reciever {
 public:  
  /* May want the constructor to hold the timestamp of when it was constructed. */
  Pushto_Output_View(   CAA2DCoordinate RA_and_Dec );
  ~Pushto_Output_View();
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
  void put_char(char);
  void set_label_1(std::string);
  void set_label_2(std::string);
 private:
  CAA2DCoordinate RA_Dec;
  Alignment_Data_Set* data_set;
  Simple_Altazimuth_Scope* telescope;
  Alt_Azi_Snapshot_t target;
  Alt_Azi_Snapshot_t current;
  Character_Reciever* saved_cr;
  float pressure;
  float temperature;
  int32_t width_;
  float degrees_up;   /* Negative if down  */
  float degrees_left; /* Negative if right */
  float epsilon_degrees;
  std::string label_1;
  std::string label_2;
  bool finished;
};

class Countdown_View:Character_Reciever {
 public:
  Countdown_View();
  ~Countdown_View();
  std::unique_ptr < CharLCD_STM32F > write_first_line(std::unique_ptr <
						      CharLCD_STM32F >);
  std::unique_ptr < CharLCD_STM32F > write_second_line(std::unique_ptr <
						       CharLCD_STM32F >);
  std::unique_ptr < CharLCD_STM32F > write_third_line(std::unique_ptr <
						      CharLCD_STM32F >);
  std::unique_ptr < CharLCD_STM32F > write_fourth_line(std::unique_ptr <
						       CharLCD_STM32F >);
  void set_label_1(std::string);
  void set_label_2(std::string);
  void set_countdown_moment( double );
  void update();
  void put_char(char);
  inline bool is_finished() {
    return finished;
  } 

 private:
  Character_Reciever* saved_cr;
  int32_t days;
  int32_t hours;
  uint8_t minutes;
  uint8_t seconds;
  uint16_t xxx;
  uint8_t d_space = 5;
  uint8_t h_space = 3;
  uint8_t m_space = 3;
  double countdown_moment; /* The Julian date we count down to... */
  std::string label_1;
  std::string label_2;
  int32_t width_;
  bool finished;
};


class Alignment_Sights_View:public Character_Reciever{
 public:  
  Alignment_Sights_View( Alignment_Data_Set* data );
  ~Alignment_Sights_View( );

  std::unique_ptr < CharLCD_STM32F > write_first_line(std::unique_ptr <
						      CharLCD_STM32F >);
  std::unique_ptr < CharLCD_STM32F > write_second_line(std::unique_ptr <
						       CharLCD_STM32F >);
  std::unique_ptr < CharLCD_STM32F > write_third_line(std::unique_ptr <
						      CharLCD_STM32F >);
  std::unique_ptr < CharLCD_STM32F > write_fourth_line(std::unique_ptr <
						       CharLCD_STM32F >);
  
  size_t size();
  void put_char(char);
  inline bool is_finished() {
    return finished;
  } 
  inline bool prompt_for_new_star_sight(){
    return _prompt_for_new_star_sight;
  }
  inline bool prompt_for_new_planet_sight(){
    return _prompt_for_new_planet_sight;
  }
  inline bool delete_item_with_confirm(){
    return _delete_item_with_confirm;
  }
  void clear_prompts();
  void trim_position();
  uint32_t get_position();
 private:
   std::unique_ptr < CharLCD_STM32F > write_date_line(std::unique_ptr <
						    CharLCD_STM32F >, double jd);

   std::unique_ptr < CharLCD_STM32F > write_text_line(std::unique_ptr < 
						   CharLCD_STM32F > lcd, std::string text );

   std::unique_ptr < CharLCD_STM32F > write_blank_line(std::unique_ptr <
							CharLCD_STM32F >);
   void scroll_up();
   void scroll_down();
   void delete_with_confirm();
   void delete_without_confirm();
   void select_char_action();
   void return_char_action();
   void asterix_char_action();
   Character_Reciever* saved_cr;
   uint32_t position;
   int32_t width_;
   Alignment_Data_Set* data_set;
   bool finished;
   bool _prompt_for_new_star_sight;
   bool _prompt_for_new_planet_sight;
   bool _delete_item_with_confirm;
};



class Planetary_Details_View:Character_Reciever {
 public:
  Planetary_Details_View( int planet );
  ~Planetary_Details_View( );
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
 private:
  int planet_num_;
  double JD;
  std::string body_name;
  CAA2DCoordinate RA_Dec;
  double distance;
  //  Simple_Altazimuth_Scope* telescope;
  Character_Reciever* saved_cr;
  int32_t width_;
  bool finished;
};


class BSC_Details_View:Character_Reciever {
 public:
  BSC_Details_View( );
  ~BSC_Details_View( );
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
  void set_index(int i);
 private:
  int index;
  double JD;
  CAA2DCoordinate RA_Dec;
  Character_Reciever* saved_cr;
  int32_t width_;
  bool finished;
};

class NGC_Details_View:Character_Reciever {
 public:
  NGC_Details_View( int ngc_num );
  ~NGC_Details_View( );
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
 private:
  int ngc_num_;
  int index;
  float magnitude;
  void setup( int ngc_num );
  double JD;
  CAA2DCoordinate RA_Dec;
  float dimension_a;
  float dimension_b;
  //   double distance;
  //  Simple_Altazimuth_Scope* telescope;
  Character_Reciever* saved_cr;
  int32_t width_;
  bool finished;
};



#endif				/*  OUTPUT_VIEWS_H  */
