#ifndef MICROSECOND_DELAY_H
#define MICROSECOND_DELAY_H


/*! 9-15-2013 Changed from TIM13 to TIM14 to avoid conflict with keypad. */
// #include <stdint.h>
#include <stm32f4xx.h>

#define MICROSECOND_DELAY_TIMER_TO_USE 14

#ifdef __cplusplus
namespace MicroSecondDelay {
  void microsecond_delay(uint16_t value);
  void millisecond_delay(uint32_t value);
  void microsecond_delay_configure(void);
  /* We may not alter the pointer TIMmicrosecond_delay_timer but we may modify members of 
   * the structure pointed to. Stroustrup 4th Ed. p.187 */
#if MICROSECOND_DELAY_TIMER_TO_USE == 13
  TIM_TypeDef *const TIMmicrosecond_delay_timer = TIM13;
  const uint32_t microsecond_delay_timer_rcc_bus = RCC_APB1Periph_TIM13;
#elif MICROSECOND_DELAY_TIMER_TO_USE == 14
  TIM_TypeDef *const TIMmicrosecond_delay_timer = TIM14;
  const uint32_t microsecond_delay_timer_rcc_bus = RCC_APB1Periph_TIM14;
#endif
}
#endif /* ifdef __cplusplus */


#endif				/* MICROSECOND_DELAY_H */
