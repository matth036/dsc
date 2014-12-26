#ifndef _CONTROLLER_H
#define _CONTROLLER_H

#include <string>
#include <vector>
#include "character_reciever.h"

/* 
 * This should be a singleton? (Defer doing so.)
 * 
 * Should the keystroke input buffer live in this class? (I like this idea.)
 *
 *
 *
 */


namespace dsc_controller {


  extern std::vector < std::string > cmd_buffer;
  extern std::vector < char >char_buffer;

  void set_character_reciever(Character_Reciever*);
  Character_Reciever* get_character_reciever();

  std::string pop_cmd_buffer();

  
  void put_char(char c);

  void  push_cmd_buffer(std::string);

  void A_command(std::string &);
  void AA_command(std::string &);
  void AAA_command(std::string &);
  void B_command(std::string &);
  void BA_command(std::string &);
  void BB_command(std::string &);
  void BC_command(std::string &);
  void C_command(std::string &);
  void CA_command(std::string &);
  void CB_command(std::string &);
  void CC_command(std::string &);



  void do_execute_command(std::string & cmd);
  void execute_early_experimental_command_implementations(std::string & cmd);


}
#endif				/*  _CONTROLLER_H */
