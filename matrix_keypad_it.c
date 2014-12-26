#include "matrix_keypad.h"
#include <stm32f4xx_tim.h>
#include "main.h"

/**
 *  TIM12 will handle keypad write and read events.
 *
 *  Matrix_Keypad defined in matrix_keypad.h is now a C++ class.  
 *  Think about how to handle timer(s) for more than one keypad. 
**/
#if KEYPAD_TIMER_TO_USE == 12

static uint16_t capture;
const uint16_t TIM12_CCR1_Val = 250; 
void TIM8_BRK_TIM12_IRQHandler()
{

  if (TIM_GetITStatus(TIM12, TIM_IT_CC1) != RESET){
    TIM_ClearITPendingBit(TIM12, TIM_IT_CC1);
    main_keypad_time_tick_action();
    capture = TIM_GetCapture1(TIM12);
    TIM_SetCompare1(TIM12, capture + TIM12_CCR1_Val);
  }

}

#endif
