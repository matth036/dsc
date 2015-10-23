#include "controller.h"

#include <string>
#include <vector>
#include "matrix_keypad.h"

/** \addtogroup os 
  * @{
  */
/** @brief Setable, getable storage location for the currently active 
 *  Character_Reciever.
 */
static Character_Reciever *current_character_reciever = nullptr;

/** 
 * This may be called by any funtion with arguemnt a pointer to any class implementing
 * the Character_Reciever interface.  That is, at any time the 
 * stream of input characters may be usurped.  This is not expected to 
 * be a problem.  If this becomes a problem the plan is to deal with it 
 * by implementing a subscriber pattern.
 * 
 * AN EFFECTIVE PRACTICE is this.  When writing a class implementing
 * Character_Reciever:
 * -# get_character_reciever() and save the value in the constructor.
 * -# set_character_reciever( this ) in the constructor.
 * -# In the destructor, restore the saved value.
 *
 * This lax treatment stands in contrast to the controlled 
 * treatment of the main LCD display where move semanitics
 * transfer a unique_ptr.
 **/
void dsc_controller::set_character_reciever(Character_Reciever * cr)
{
  current_character_reciever = cr;
}

/** Returns a pointer to the currently active Character_Reciever. */
Character_Reciever *dsc_controller::get_character_reciever()
{
  return current_character_reciever;
}

/** The default treatment of input characters is to take them as forming command language words. */
void default_put_char(char c)
{
  if (c == keypad_backspace_char) {     /* behave like a Backspace. */
    if (dsc_controller::char_buffer.size() > 0) {
      dsc_controller::char_buffer.pop_back();
    }
    return;
  } else if (c == keypad_return_char) { /* behave like a [Enter|Return] */
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

/** Recieves and dispatches an input character. */
void dsc_controller::put_char(char c)
{
  if (current_character_reciever) {
    current_character_reciever->put_char(c);
  } else {
    default_put_char(c);
  }
}

/** 
 *  If not dispatched to an object implementing Character_Reciever,
 *  input characters are pushed into this vector. 
*/
std::vector < char >dsc_controller::char_buffer {24};
/** 
 *  @brief Input commands are pushed into this stack.
 *
 *  A command is pushed when '#' is entered into the keypad.
 *
 *  When a running task is finished executing, control returns
 *  to the super loop at the bottom of main().  Inside the super loop,
 *  if cmd_buffer is not empty, a command is popped and executed
 *  as the next task if
 *  it is a valid command. 
 *  
 */
std::vector < std::string > dsc_controller::cmd_buffer {8};

std::string dsc_controller::pop_cmd_buffer()
{
  std::string item = cmd_buffer[0];
  /* 
   * Simulate FIFO  action  [at cost of O(n)]. 
   * 
   * An attempt to make cmd_buffer a std::deque<std::string> did not work well.
   * The application halted displaying an empty command.
   **/
  for (auto p = cmd_buffer.begin(); (p + 1) != cmd_buffer.end(); ++p) {
    *p = *(p + 1);
  }
  cmd_buffer.pop_back();
  return item;
}

void dsc_controller::push_cmd_buffer(std::string cmd)
{
  dsc_controller::cmd_buffer.push_back(cmd);
}




/** @} */
// eof
