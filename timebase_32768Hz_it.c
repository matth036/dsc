#include "timebase_32768Hz.h"

  /*
   *   To create a 32786 Hz clock we need toggle at 65536 Hz
   *   We operate TIM13 at it's maximum frequency 42 MHz.
   *   We want to toggle every 640.869 clock ticks. (42000000.0 / 65536.0)
   *   The conditional in the interupt achieves this on the average.
   */

static uint32_t tim13_it_count = 0;
static uint16_t capture = 0;

#if TIMEBASE_32768HZ_TIMER_TO_USE == 13
//TIM_TypeDef *const timebase_32768Hz_timer = TIM13;
//const uint32_t timebase_32768Hz_timer_rcc_bus = RCC_APB1Periph_TIM13;
//const IRQn_Type timebase_32768Hz_IRQn = TIM8_UP_TIM13_IRQn;
void TIM8_UP_TIM13_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM13, TIM_IT_CC1) != RESET) {
    TIM_ClearITPendingBit(TIM13, TIM_IT_CC1);
    capture = TIM_GetCapture1(TIM13);
    ++tim13_it_count;
    if ((tim13_it_count & 0x01FF) < 67) {
      TIM_SetCompare1(TIM13, capture + 640);	/* do this  67 of 512 times */
    } else {
      TIM_SetCompare1(TIM13, capture + 641);	/* do this 455 of 512 times */
    }
    if ((tim13_it_count & 0x0003FFFF) == 0) {
      /* Toggles some LED */
      GPIOC->ODR ^= GPIO_Pin_13;
    }

  }
}

#elif  TIMEBASE_32768HZ_TIMER_TO_USE == 3
//  TIM_TypeDef *const timebase_32768Hz_timer = TIM3;
//  const uint32_t timebase_32768Hz_timer_rcc_bus = RCC_APB1Periph_TIM3;
//  const IRQn_Type timebase_32768Hz_IRQn = TIM3_IRQn;
#endif
