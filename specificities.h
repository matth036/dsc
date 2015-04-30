#ifndef _SPECIFICITIES_H
#define _SPECIFICITIES_H

#include "stm32f4xx_gpio.h"
/* 
 * Will also include hardware (wire connections, pins and ports  to the cpu board) here.
 * 
 * 
 * Stuff (parameters) in this file
 * should be end user set-able in a finished product. 
 */
namespace specificities{
  constexpr uint32_t azimuth_ticks_per_revolution = 4000;
  constexpr uint32_t altitude_ticks_per_revolution = 4000;

  constexpr GPIO_TypeDef* azimuth_decoder_ch_A_port = GPIOD;
  constexpr uint16_t azimuth_decoder_ch_A_pin = GPIO_Pin_13;
  constexpr GPIO_TypeDef* azimuth_decoder_ch_B_port = GPIOD;
  constexpr uint16_t azimuth_decoder_ch_B_pin = GPIO_Pin_12;

  constexpr GPIO_TypeDef* altitude_decoder_ch_A_port = GPIOE;
  constexpr uint16_t altitude_decoder_ch_A_pin = GPIO_Pin_9;
  constexpr GPIO_TypeDef* altitude_decoder_ch_B_port = GPIOE;
  constexpr uint16_t altitude_decoder_ch_B_pin = GPIO_Pin_11;
  /* 
   * Ultimately, the startup count should not matter.
   * For the purpose of debugging and trouble shooting I arrange
   * for the azimuth count to be zero near north and the altitude count to be 
   * zero near the horizon.  
   * 
   * Working near 45 degress north latitude, these values ensure this if
   * the microcontroller is turned on while the telescope is pointed at Polaris.
   */
  constexpr bool latitude_is_north = true;
  constexpr bool longitude_is_west = true;
  constexpr uint32_t my_latitude = 45;
  constexpr uint32_t altitude_startup_count = (altitude_ticks_per_revolution*my_latitude)/360;
  constexpr uint32_t azimuth_startup_count = 0;

  /* Declared here, these are defined in specificities.cpp */
  extern std::initializer_list<uint16_t>       char_lcd_pins;
  extern std::initializer_list<GPIO_TypeDef*>  char_lcd_ports;

  extern std::initializer_list<uint16_t>       matrix_keypad_row_pins;
  extern std::initializer_list<GPIO_TypeDef*>  matrix_keypad_row_ports;
  extern std::initializer_list<uint16_t>       matrix_keypad_col_pins;
  extern std::initializer_list<GPIO_TypeDef*>  matrix_keypad_col_ports;
}

#endif   /*  _SPECIFICITIES_H  */
