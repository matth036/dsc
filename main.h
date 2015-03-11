#ifndef __MAIN_H
#define __MAIN_H

#include "stm32f4xx.h"
#define AZIMUTH_DECODER_TIMER_TO_USE TIM4
#define ALTITUDE_DECODER_TIMER_TO_USE TIM1


#ifdef __cplusplus
/* includes and fuctions that only make sense in a C++ context. */
#include <memory>
#include "char_lcd_stm32f4.h"
#include "alignment_sight_info.h"

class Telescope_Model;
class Square_Altazimuth_Scope;
std::unique_ptr<CharLCD_STM32F> check_out_main_lcd();
void check_in_main_lcd(  std::unique_ptr<CharLCD_STM32F>&& returned_ptr );

Alignment_Data_Set* get_main_sight_data();
Simple_Altazimuth_Scope* get_main_simple_telescope();

/* Failed attempts at greater generality.  */
//Square_Altazimuth_Scope* get_main_alt_az_telescope();
//Telescope_Model* get_main_telescope();
#endif


#ifdef __cplusplus
extern "C" {
#endif

  int main(void);  /* Need I declare this? */
  /* These functions may be called from either C or C++ code if this header file is #included  */
  // void put_char( char c );
  void TimingDelay_Decrement(void);
  void Delay(__IO uint32_t nTime);
  void main_keypad_time_tick_action();
  void main_lcd_pwm_flip_on();
  void main_lcd_pwm_flip_off();


#ifdef  USE_FULL_ASSERT
  void assert_failed(uint8_t * file, uint32_t line);
#endif 

#ifdef __cplusplus
}
#endif


#endif				/* __MAIN_H */
