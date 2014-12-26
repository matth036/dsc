#include "microsecond_delay.h"

/*   STM32F4-Discovery_FW_V1.1.0/Libraries/STM32F4xx_StdPeriph_Driver/inc/misc.h */
#include <misc.h>
/*  STM32F4-Discovery_FW_V1.1.0/Libraries/CMSIS/ST/STM32F4xx/Include/stm32f4xx.h */
#include <stm32f4xx.h>
/*  STM32F4-Discovery_FW_V1.1.0/Libraries/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_rcc.h */
#include <stm32f4xx_rcc.h>

/*  A TIMx typedef as defined in the ST source files e.g. TIM13  */
using MicroSecondDelay::TIMmicrosecond_delay_timer;

void MicroSecondDelay::microsecond_delay_configure(void)
{

  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_OCInitTypeDef TIM_OCInitStructure;
  RCC_APB1PeriphClockCmd(MicroSecondDelay::microsecond_delay_timer_rcc_bus,
			 ENABLE);
  /* Time base configuration.  
   *  We will use a timer that only counts up.  
   *  We will use a timer with only 16 bits   */
  TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
  /* This prescaler value is correct for TIM13 and TIM14.  Fix if another is used. */
  TIM_TimeBaseStructure.TIM_Prescaler =
      (uint16_t) ((SystemCoreClock / 2) / 1000000) - 1;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIMmicrosecond_delay_timer, &TIM_TimeBaseStructure);

  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 0;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

  TIM_OC1Init(TIMmicrosecond_delay_timer, &TIM_OCInitStructure);

  TIM_SelectOnePulseMode(TIMmicrosecond_delay_timer, TIM_OPMode_Single);
  /*  Leave the counter disabled between calls to microsecond_dalay( uint16_t ); */
  TIM_Cmd(TIMmicrosecond_delay_timer, DISABLE);
}

/*! Delay for at least the specified number of microseconds.  
 *  (1 microsecond) == (1.0e-6 second).
 */
void MicroSecondDelay::microsecond_delay(uint16_t value)
{
  if (value < 1) {
    return;
  }
  TIMmicrosecond_delay_timer->CCR1 = value;
  TIMmicrosecond_delay_timer->CNT = 0;
  TIM_Cmd(TIMmicrosecond_delay_timer, ENABLE);
  while (TIM_GetFlagStatus(TIMmicrosecond_delay_timer, TIM_FLAG_CC1) == RESET) {
    /* This is the delay.  Because this loop polls TIM_FLAG_CC1 
     * there is no interrupt or interrupt handler.
     */
  }
  TIM_Cmd(TIMmicrosecond_delay_timer, DISABLE);
  TIM_ClearFlag(TIMmicrosecond_delay_timer, TIM_FLAG_CC1);
}

void MicroSecondDelay::millisecond_delay(uint32_t value)
{
  while (value) {
    MicroSecondDelay::microsecond_delay(1000);
    value--;
  }
}
