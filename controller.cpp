#include "controller.h"

#include "main.h"
#include "microsecond_delay.h"
#include "rtc_management.h"
#include "solar_system.h"
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

#include "ngc_list_access.h"

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

