#include "menu_views.h"

#include "matrix_keypad.h"
#include "char_lcd_stm32f4.h"
#include "controller.h"
#include "sexagesimal.h"

Numbered_List_Menu::Numbered_List_Menu(std::string text)
{
  header_text = text;
  has_selection = false;
  has_digits = false;
  finished = false;
  viewpoint = 0;
  _width = MENU_VIEW_DEFAULT_WIDTH;
  selected_char = '+';
  deselected_char = ' ';
  _num_width = 2;
  saved_cr = dsc_controller::get_character_reciever();
  dsc_controller::set_character_reciever(this);
}

Numbered_List_Menu::~Numbered_List_Menu()
{
  dsc_controller::set_character_reciever(saved_cr);
}

void Numbered_List_Menu::set_selected_char(char c)
{
  selected_char = c;
}

void Numbered_List_Menu::set_deselected_char(char c)
{
  deselected_char = c;
}

void Numbered_List_Menu::put_char(char c)
{
 switch( c ){
 case scroll_up_char:
   scroll_up();
   return;
 case scroll_down_char:
   scroll_down();
   return;
 case select_char:
   select();
   return;
 case keypad_return_char:
   return_char_action();
   return;
 case '0':
 case '1':
 case '2':
 case '3':
 case '4':
 case '5':
 case '6':
 case '7':
 case '8':
 case '9':
   put_digit( c );
   return;
 case keypad_backspace_char:
   pop_digit();
   return;
 }
 
}

void Numbered_List_Menu::return_char_action()
{
  // dsc_controller::set_character_reciever(nullptr);
  finished = true;
}

void Numbered_List_Menu::scroll_up()
{
  if ( viewpoint > 0 ) {
    --viewpoint;
  }
}

void Numbered_List_Menu::scroll_down()
{
  if( viewpoint < 0 ){
    viewpoint = 0;
    return;
  }
  if ( static_cast<uint32_t>(viewpoint) < size() - 1 ) {
    ++viewpoint;
  }
}

size_t Numbered_List_Menu::size(){
  return selections.size();
}

void Numbered_List_Menu::select()
{
  if( has_selection ){
    if( selection == viewpoint ){
      /* Toggle from selected to un-selected. */
      selection = 0;
      has_selection = false;
    }else{
      selection = viewpoint;
      has_selection = true;
    }
  }else{
    selection = viewpoint;
    has_selection = true;
  }
}


std::string Numbered_List_Menu::get_selection_text(){
  if( has_selection ){
    return selections[selection].text;
  }else{
    return "";
  }
}

int32_t Numbered_List_Menu::get_selection()
{
  if( has_selection ){
    return selection;
  }
  if( has_digits ){
    for( auto it = selections.begin(); it != selections.end(); ++it ){
      if( digits_value == it->number ){
	return digits_value;
      }
    }
  }
  return -1;
}

void Numbered_List_Menu::put_digit(char d){
  digits_value = 10 * digits_value + (d - '0');
  has_digits = true;
}

void Numbered_List_Menu::pop_digit(){
  digits_value /= 10;
}

void Numbered_List_Menu::insert_menu_item(int n, std::string item_text){
  numbered_list_menu_item item;
  item.number = n;
  item.text = item_text;
  selections.push_back( item );
}

std::unique_ptr < CharLCD_STM32F >
    Numbered_List_Menu::write_first_line(std::unique_ptr < CharLCD_STM32F > lcd)
{
  /* Assumes the lcd position is at the start of the line. */
  int space = _width - header_text.size();
  int n = 0;
  while (n < space / 2) {
    n += lcd->print(' ');
  }
  n += lcd->print(header_text);
  while (n < _width) {
    n += lcd->print(' ');
  }
  return std::move(lcd);
}

/*  The lines appear thus:
 *
 *   1    |        HEADER      |
 *   2    |  viewpoint - 1     |  (blank if out of range)
 *   3    | viewpoint          |
 *   4    |  viewpoint + 1     |  (blank if out of range)
 *
 */

std::unique_ptr < CharLCD_STM32F >
    Numbered_List_Menu::write_second_line(std::unique_ptr < CharLCD_STM32F > lcd)
{
  uint32_t index = viewpoint - 1;
  if( index >= 0 && index < selections.size() ){
    lcd = write_line( std::move(lcd), index );
  }else{
    lcd = write_blank_line( std::move(lcd) );
  }
  return std::move(lcd);
}

std::unique_ptr < CharLCD_STM32F >
    Numbered_List_Menu::write_third_line(std::unique_ptr < CharLCD_STM32F > lcd)
{
  uint32_t index = viewpoint;
  if( index >= 0 && index < selections.size() ){
    lcd = write_line( std::move(lcd), index );
  }else{
    lcd = write_blank_line( std::move(lcd) );
  }
  return std::move(lcd);
}

std::unique_ptr < CharLCD_STM32F >
    Numbered_List_Menu::write_fourth_line(std::unique_ptr < CharLCD_STM32F > lcd)
{
  uint32_t index = viewpoint + 1;
  if( index >= 0 && index < selections.size() ){
    lcd = write_line( std::move(lcd), index );
  }else{
    lcd = write_blank_line( std::move(lcd) );
  }
  return std::move(lcd);
}

std::unique_ptr < CharLCD_STM32F >
Numbered_List_Menu::write_line(std::unique_ptr < CharLCD_STM32F > lcd, int32_t index)
{
  int n = 0;
  if( has_selection && index == selection ){
    n += lcd->print( selected_char );
  }else{
    n += lcd->print( deselected_char );
  }
  numbered_list_menu_item item = selections[index];
  std::string number_string = sexagesimal::to_string_hack( static_cast<uint32_t>(item.number) );
  while( number_string.size() < _num_width ){ 
    number_string = ' ' + number_string; /* Left pad with spaces. */
  }


  n += lcd->print( number_string );
  n += lcd->print( ' ' );
  n += lcd->print( item.text );

  if( index == viewpoint ){
    while (n < _width - 1) {
      n += lcd->print(' ');
    }  
    n += lcd->print('<');
  }else{
    while (n < _width) {
      n += lcd->print(' ');
    }    
  }
  return std::move(lcd);
}

std::unique_ptr < CharLCD_STM32F >
Numbered_List_Menu::write_blank_line(std::unique_ptr < CharLCD_STM32F > lcd )
{
  int n = 0; 
  while (n < _width) {
    n += lcd->print(' ');
  }
  return std::move(lcd);
}
