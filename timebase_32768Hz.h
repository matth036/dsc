#ifndef TIMEBASE_32768HZ_H
#define TIMEBASE_32768HZ_H

#include <stm32f4xx.h>

#define TIMEBASE_32768HZ_TIMER_TO_USE 13

#ifdef __cplusplus
namespace timebase_32768Hz {

  void timebase_32768Hz_configure(void);

#if TIMEBASE_32768HZ_TIMER_TO_USE == 13
  TIM_TypeDef *const timebase_32768Hz_timer = TIM13;
  const uint32_t timebase_32768Hz_timer_rcc_bus = RCC_APB1Periph_TIM13;
  const IRQn_Type timebase_32768Hz_IRQn = TIM8_UP_TIM13_IRQn;
#elif  TIMEBASE_32768HZ_TIMER_TO_USE == 3
  TIM_TypeDef *const timebase_32768Hz_timer = TIM3;
  const uint32_t timebase_32768Hz_timer_rcc_bus = RCC_APB1Periph_TIM3;
  const IRQn_Type timebase_32768Hz_IRQn = TIM3_IRQn;
#endif

}				/* end of namespace. */
#endif				/* end of ifdef __cplusplus */
#endif				/* eof TIMEBASE_32768HZ_H */
