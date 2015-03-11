#ifndef MENU_VIEWS_H
#define MENU_VIEWS_H

#include <string>
#include <vector>
#include <memory>
class CharLCD_STM32F;
#include "character_reciever.h"

constexpr uint32_t MENU_VIEW_DEFAULT_WIDTH = 20;

struct numbered_list_menu_item {
  int32_t number;
   std::string text;
};

class Numbered_List_Menu:Character_Reciever {
 public:
  Numbered_List_Menu(std::string header_text);
  ~Numbered_List_Menu();

  void put_char(char c);
  void put_digit(char d);
  void pop_digit();
  void sort_by_number();
  void sort_by_name();
   std::unique_ptr < CharLCD_STM32F > write_first_line(std::unique_ptr <
						       CharLCD_STM32F >);
   std::unique_ptr < CharLCD_STM32F > write_second_line(std::unique_ptr <
							CharLCD_STM32F >);
   std::unique_ptr < CharLCD_STM32F > write_third_line(std::unique_ptr <
						       CharLCD_STM32F >);
   std::unique_ptr < CharLCD_STM32F > write_fourth_line(std::unique_ptr <
							CharLCD_STM32F >);

   std::unique_ptr < CharLCD_STM32F > write_line(std::unique_ptr <
						      CharLCD_STM32F >, int32_t );


   std::unique_ptr < CharLCD_STM32F > write_blank_line(std::unique_ptr <
							CharLCD_STM32F >);


  void set_selected_char(char);
  void set_deselected_char(char);
  void select();
  void return_char_action();
  void insert_menu_item(int n, std::string text);
  void scroll_up();
  void scroll_down();
  size_t size();
  inline bool is_finished() {
    return finished;
  } 
  int32_t get_selection();
  std::string get_selection_text();
 private:
  Character_Reciever* saved_cr;
  std::vector < numbered_list_menu_item > selections;
  int32_t digits_value;
  int32_t selection;
  int32_t viewpoint;
  int32_t _width;
  uint32_t _num_width;
  std::string header_text;
  char selected_char;
  char deselected_char;
  bool has_digits;
  bool has_selection;
  bool finished;
};

#endif				/* MENU_VIEWS_H */
