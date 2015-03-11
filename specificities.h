#ifndef _SPECIFICITIES_H
#define _SPECIFICITIES_H

#include "stm32f4xx_gpio.h"
/* 
 * Will also include hardware (wire connections to the cpu board) here.
 * 
 * 
 * Stuff (parameters) in this file
 * should be end user set-able in a finished product. 
 */
namespace specificities{
  constexpr uint32_t altitude_ticks_per_revolution = 4000;
  constexpr uint32_t azimuth_ticks_per_revolution = 4000;

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

  /* MACROS such as GPIO_Pin_7 are defined in stm32f4xx_gpio.h */

  constexpr uint16_t char_lcd_E_pin   = GPIO_Pin_4;        /* E  (Enable) */
  constexpr uint16_t char_lcd_DB0_pin = GPIO_Pin_6;        /* DB0 */


  constexpr uint16_t char_lcd_DB4_pin = GPIO_Pin_7;        /* DB4 */
  constexpr uint16_t char_lcd_DB5_pin = GPIO_Pin_9;        /* DB5 */
}

#endif   /*  _SPECIFICITIES_H  */
