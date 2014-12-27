#include "controller.h"

#include "main.h"
#include "microsecond_delay.h"
#include "rtc_management.h"
#include "solar_system.h"
// #include <regex>
#include <cctype>
#include <cmath>
#include "matrix_keypad.h"

#include "AAElliptical.h"
#include "AA3DCoordinate.h"
#include "AA2DCoordinate.h"
#include "AAPrecession.h"
#include "AADynamicalTime.h"
#include "sexagesimal.h"
#include "extra_solar_transforms.h"
#include "ngc_objects.h"
#include "ngc_list.h"
#include "input_views.h"
#include "output_views.h"
#include "menu_views.h"
#include "binary_tidbits.h"
#include "navigation_star.h"
#include "AADate.h"
#include "build_date.h"
#include "alignment_sight_info.h"
#include "linear_algebra_interface.h"
#include "navigation_star.h"
#include "telescope_model.h"

/* forward declarations */

void navigation_star_menu_test();
void navigation_star_menu();
void solar_system_menu();


bool is_two_digits(std::string & cmd)
{
  if (cmd.size() != 2) {
    return false;
  }
  if (!isdigit(cmd[0]))
    return false;
  if (!isdigit(cmd[1]))
    return false;
  return true;
}

bool is_four_digits(std::string & cmd)
{
  if (cmd.size() != 4) {
    return false;
  }
  if (!isdigit(cmd[0]))
    return false;
  if (!isdigit(cmd[1]))
    return false;
  if (!isdigit(cmd[2]))
    return false;
  if (!isdigit(cmd[3]))
    return false;
  return true;
}

void debug_print(std::string & msg)
{
  auto lcd = check_out_main_lcd();
  lcd->clear();
  lcd->home();
  MicroSecondDelay::millisecond_delay(1);
  lcd->setCursor(0, 0);
  int n = lcd->print("[");
  n += lcd->print(msg);
  n += lcd->print("]");
  while (n < 20) {
    n += lcd->print("]");
  }
  MicroSecondDelay::millisecond_delay(9000);
  lcd->clear();
  MicroSecondDelay::millisecond_delay(1);
  check_in_main_lcd(std::move(lcd));
}

void nav_star_command(int num)
{
  if( num<0 || num > 57 ){
    return;
  }
  double JD = JD_timestamp_pretty_good_000();
  CAA2DCoordinate RA_Dec = navigation_star::nav_star_RA_Dec(num, JD);
  int n;
  auto lcd = check_out_main_lcd();
  lcd->clear();
  lcd->home();
  MicroSecondDelay::millisecond_delay(1);
  lcd->setCursor(0, 0);
  n = lcd->print("Navigation Star: ");
  n += lcd->print(num);
  while (n < 20) {
    n += lcd->print(" ");
  }
  lcd->setCursor(0, 1);
  n = lcd->print(" RA: ");
  n += lcd->print(sexagesimal::Sexagesimal(RA_Dec.X).to_string());
  while (n < 20) {
    n += lcd->print(" ");
  }
  lcd->setCursor(0, 2);
  n = lcd->print("DEC: ");
  n += lcd->print(sexagesimal::Sexagesimal(RA_Dec.Y).to_string());
  while (n < 20) {
    n += lcd->print(" ");
  }
  lcd->setCursor(0, 3);
  n = lcd->print( "* "  );
  n += lcd->print(  navigation_star::get_navigation_star_name(num)  );
  n += lcd->print( " *"  );
  while (n < 20) {
    n += lcd->print(" ");
  }

  MicroSecondDelay::millisecond_delay(9000);
  lcd->clear();
  MicroSecondDelay::millisecond_delay(1);
  check_in_main_lcd(std::move(lcd));
}

#if 0
void ngc_object_command(int num)
{
  if( num <= 0 || num > 7840 ){
    return;
  }
  // double JD  = JD_timestamp_pretty_good_000();
  int index = ngc_objects::get_index( num );
  CAA2DCoordinate RA_Dec;
  sexagesimal::Sexagesimal RA;
  sexagesimal::Sexagesimal Dec;
  RA.set_binary_data(ngc_list[index].RA_data);
  Dec.set_binary_data(ngc_list[index].DEC_data);
  RA_Dec.X = RA.to_double();
  RA_Dec.Y = Dec.to_double();

  int n;
  auto lcd = check_out_main_lcd();
  lcd->clear();
  lcd->home();
  MicroSecondDelay::millisecond_delay(1);
  lcd->setCursor(0, 0);
  n = lcd->print("NGC Object: ");
  n += lcd->print(num);
  while (n < 20) {
    n += lcd->print(" ");
  }
  lcd->setCursor(0, 1);
  n = lcd->print(" RA: ");
  n += lcd->print(sexagesimal::Sexagesimal(RA_Dec.X).to_string());
  while (n < 20) {
    n += lcd->print(" ");
  }
  lcd->setCursor(0, 2);
  n = lcd->print("DEC: ");
  n += lcd->print(sexagesimal::Sexagesimal(RA_Dec.Y).to_string());
  while (n < 20) {
    n += lcd->print(" ");
  }
  lcd->setCursor(0, 3);
  n = lcd->print( "Magnitue = "  );
  n += lcd->print(  ngc_list[index].Vmag  );
  while (n < 20) {
    n += lcd->print(" ");
  }
  MicroSecondDelay::millisecond_delay(9000);
  lcd->clear();
  MicroSecondDelay::millisecond_delay(1);
  check_in_main_lcd(std::move(lcd));
}
#endif


void two_digit_command(int value)
{
  if (value >= 0 && value <= 57) {
    nav_star_command(value);
    return;
  } else {
    auto lcd = check_out_main_lcd();
    lcd->clear();
    lcd->home();
    MicroSecondDelay::millisecond_delay(1);
    lcd->setCursor(0, 0);
    lcd->print("TWO DIGIT ");
    lcd->print(value);
    lcd->print(";   ");
    MicroSecondDelay::millisecond_delay(9000);
    lcd->clear();
    MicroSecondDelay::millisecond_delay(1);
    check_in_main_lcd(std::move(lcd));
  }
}

void four_digit_command(int value)
{
  if (value >= 0 && value <= 7840) {
    //  ngc_object_command(value);
    return;
  } else {
    auto lcd = check_out_main_lcd();
    lcd->clear();
    lcd->home();
    MicroSecondDelay::millisecond_delay(1);
    lcd->setCursor(0, 0);
    lcd->print("FOUR DIGIT ");
    lcd->print(value);
    lcd->print(";   ");
    MicroSecondDelay::millisecond_delay(9000);
    lcd->clear();
    MicroSecondDelay::millisecond_delay(1);
    check_in_main_lcd(std::move(lcd));
  }
}

void dsc_controller::execute_early_experimental_command_implementations(std::string & cmd){

}






void dsc_controller::AAA_command(std::string & cmd){

}

void dsc_controller::AA_command(std::string & cmd){
}

void dsc_controller::BA_command(std::string &cmd){}
void dsc_controller::BB_command(std::string &cmd){}
void dsc_controller::BC_command(std::string &cmd){}


void dsc_controller::CA_command(std::string &cmd){}
void dsc_controller::CB_command(std::string &cmd){}
void dsc_controller::CC_command(std::string &cmd){}





void dsc_controller::A_command(std::string & cmd){
}


/* All dead code now, remove */
void dsc_controller::B_command(std::string & cmd){

}

/* All dead code now, remove */
void dsc_controller::C_command(std::string & cmd){

}

void dsc_controller::do_execute_command(std::string & cmd)
{
  if( cmd.size() < 1 ){
    return;
  }
  switch( cmd[0] ){
  case 'A':
    dsc_controller::A_command(cmd);
    return;
  case 'B':
    dsc_controller::B_command(cmd);
    return;
  case 'C':
    dsc_controller::C_command(cmd);
    return;

  default:
    dsc_controller::execute_early_experimental_command_implementations(cmd);
  }
} 

static Character_Reciever* current_character_reciever = nullptr;
/* This may be called by any funtion with arguemnt a pointer to any class implementing
 * the Character_Reciever interface.  That is, at any time the 
 * stream of input characters may be usurped.  This is not expected to 
 * be a problem.  If this becomes a problem the plan is to deal with it 
 * by implementing a subscriber pattern.
 * 
 * This lax treatment stands in contrast to the controlled 
 * treatment of the main LCD display where conflicting print() calls 
 * would be chaotic.
 **/
void dsc_controller::set_character_reciever(Character_Reciever* cr){
  current_character_reciever = cr;
}

Character_Reciever* dsc_controller::get_character_reciever(){
  return current_character_reciever;
}


/* The default treatment of input characters is to take them as forming command language words. */
void default_put_char(char c){
  if (c == keypad_backspace_char) {		/* behave like a Backspace. */
    if (dsc_controller::char_buffer.size() > 0) {
      dsc_controller::char_buffer.pop_back();
    }
    return;
  } else if (c == keypad_return_char) {      /* behave like a [Enter|Return] */
    int length = dsc_controller::char_buffer.size();
    std::string buffer_copy = "";
    for (int i = 0; i < length; ++i) {
      buffer_copy += dsc_controller::char_buffer[i];
    }
    dsc_controller::char_buffer.clear();
    dsc_controller::cmd_buffer.push_back(buffer_copy);
    return;
  } else {
    if (dsc_controller::char_buffer.size() < 20) {
      dsc_controller::char_buffer.push_back(c);
    } 
  }
}

/* Functions in namespace dsc_controller. */
void dsc_controller::put_char(char c)
{
  if( current_character_reciever ){
    current_character_reciever->put_char(c);
  }else{
    default_put_char(c);
  }
}

std::vector < char >dsc_controller::char_buffer {24};
std::vector < std::string >dsc_controller::cmd_buffer {8};

std::string dsc_controller::pop_cmd_buffer(){
  std::string item = cmd_buffer[0];
  /* Simulate FIFO  action  [at cost of O(n)]. 
   * @TODO see if there is a proper FIFO buffer that compiles for uC.  
   **/
  for (auto p = cmd_buffer.begin(); (p + 1) != cmd_buffer.end(); ++p) {
    *p = *(p + 1);
  }
  cmd_buffer.pop_back();
  return item;
}

