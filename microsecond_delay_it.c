#include "microsecond_delay.h"

/* 
 * microsecond_delay(uint16_t) does not use interupts.
 * Instead, it polls
 * TIM_GetFlagStatus(TIMx, TIM_FLAG_CC1)
 * inside a delay loop.
 * 
 * So, why these functions? Because their presence 
 * causes a link time error if the IRQHandler() is
 * defined once more elsewhere. This indicatates a resource
 * conflict, likely the use of the same timer for two purposes.
 *
 */
#if MICROSECOND_DELAY_TIMER_TO_USE == 13
void TIM8_UP_TIM13_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM13, TIM_IT_CC1) != RESET) {
    TIM_ClearITPendingBit(TIM13, TIM_IT_CC1);
  }
}
#elif  MICROSECOND_DELAY_TIMER_TO_USE == 14
void TIM8_TRG_COM_TIM14_IRQHandler(void){
  if (TIM_GetITStatus(TIM14, TIM_IT_CC1) != RESET) {
    TIM_ClearITPendingBit(TIM14, TIM_IT_CC1);
  }
}
#endif
