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
  constexpr uint32_t altitude_ticks_per_revolution = 4000;
  constexpr uint32_t azimuth_ticks_per_revolution = 4000;

  constexpr bool latitude_is_north = true;
  constexpr bool longitude_is_west = true;

  /* 
   * Ultimately, the startup count should not matter.
   * For the purpose of debugging and trouble shooting I arrange
   * for the azimuth count to be zero near north and the altitude count to be 
   * zero near the horizon.  
   * 
   * Working near 45 degress north latitude, these values ensure this if
   * the microcontroller is turned on while the telescope is pointed at Polaris.
   */
  constexpr uint32_t my_latitude = 45;
  constexpr uint32_t altitude_startup_count = (altitude_ticks_per_revolution*my_latitude)/360;
  constexpr uint32_t azimuth_startup_count = 0;

  /* MACROS such as GPIO_Pin_7 are #defined in stm32f4xx_gpio.h */







  constexpr uint16_t char_lcd_RS_pin  = GPIO_Pin_0;        /* RS  */
  constexpr uint16_t char_lcd_RW_pin  = GPIO_Pin_2;        /* R/W */
  constexpr uint16_t char_lcd_E_pin   = GPIO_Pin_4;        /* E  (Enable) */
  constexpr uint16_t char_lcd_DB0_pin = GPIO_Pin_6;        /* DB0 */
  constexpr uint16_t char_lcd_DB1_pin = GPIO_Pin_8;        /* DB1 */
  constexpr uint16_t char_lcd_DB2_pin = GPIO_Pin_10;       /* DB2 */
  constexpr uint16_t char_lcd_DB3_pin = GPIO_Pin_14;       /* DB3 */
  constexpr uint16_t char_lcd_DB4_pin = GPIO_Pin_7;        /* DB4 */
  constexpr uint16_t char_lcd_DB5_pin = GPIO_Pin_9;        /* DB5 */
  constexpr uint16_t char_lcd_DB6_pin = GPIO_Pin_11;       /* DB6 */
  constexpr uint16_t char_lcd_DB7_pin = GPIO_Pin_15;       /* DB7 */

  // A pointer to a struct. 
  // using port = GPIO_TypeDef*; 

  constexpr GPIO_TypeDef* char_lcd_RS_port  = GPIOD;        /* RS  */
  constexpr GPIO_TypeDef* char_lcd_RW_port  = GPIOD;        /* R/W */
  constexpr GPIO_TypeDef* char_lcd_E_port   = GPIOD;        /* E  (Enable) */
  constexpr GPIO_TypeDef* char_lcd_DB0_port = GPIOD;        /* DB0 */
  constexpr GPIO_TypeDef* char_lcd_DB1_port = GPIOD;        /* DB1 */
  constexpr GPIO_TypeDef* char_lcd_DB2_port = GPIOD;        /* DB2 */
  constexpr GPIO_TypeDef* char_lcd_DB3_port = GPIOD;        /* DB3 */
  constexpr GPIO_TypeDef* char_lcd_DB4_port = GPIOD;        /* DB4 */
  constexpr GPIO_TypeDef* char_lcd_DB5_port = GPIOD;        /* DB5 */
  constexpr GPIO_TypeDef* char_lcd_DB6_port = GPIOD;        /* DB6 */
  constexpr GPIO_TypeDef* char_lcd_DB7_port = GPIOD;        /* DB7 */





}

#endif   /*  _SPECIFICITIES_H  */
