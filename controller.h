#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include <string>
#include <vector>
#include "character_reciever.h"

/* 
 * This should be a singleton? (Defer doing so.)
 * 
 * Should the keystroke input buffer live in this class? (I like this idea.)
 *
 */

/** \addtogroup os */
/** \namespace dsc_controller */
/** Containing a stack for input characters and a stack of strings for input commands. */
namespace dsc_controller {


  extern std::vector < std::string > cmd_buffer;
  extern std::vector < char >char_buffer;

  void set_character_reciever(Character_Reciever*);
  Character_Reciever* get_character_reciever();

  std::string pop_cmd_buffer();

  
  void put_char(char c);

  void push_cmd_buffer(std::string);

  void do_execute_command(std::string & cmd);

}
#endif  // CONTROLLER_H_
