#include "timebase_32768Hz.h"

#include <misc.h>
#include <stm32f4xx.h>
#include <stm32f4xx_rcc.h>

void timebase_32768Hz::timebase_32768Hz_configure(void)
{
  /* Power up the timer */
  RCC_APB1PeriphClockCmd(timebase_32768Hz::timebase_32768Hz_timer_rcc_bus,
			 ENABLE);
  TIM_Cmd(timebase_32768Hz::timebase_32768Hz_timer, DISABLE);
  /* Enable the TIMx gloabal Interrupt */
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = timebase_32768Hz::timebase_32768Hz_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  /* At this point initialize any gpio pins needed. (These will be outputs.)
   * This example will only use the onboard LED.
   * This code would very appropriatly belong in it's own function. 
   * Typically these pins will be toggled in the interupt handler.
   * (So there is configuration information that needs to be shared with this 
   * file and the file implementing the IRQ handler.)
   */
  /* GPIOC Periph clock enable */
  if (0) {
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    /*  Configure PC13 in output pushpull mode.  This is connected to the green user LED. */
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    /* Set the pin to some definite state. On or off. */
    GPIOC->BSRRL = GPIO_Pin_13;	// On 
  }
  // uint16_t PrescalerValue = 10666;   /* Slow down 32Khz to 3Hz for debug only, Set to 0 for showtime. */
  uint16_t PrescalerValue = 0;
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
  TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;	// zero.
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(timebase_32768Hz::timebase_32768Hz_timer,
		   &TIM_TimeBaseStructure);
  /* Another damned thing. */
  TIM_PrescalerConfig(timebase_32768Hz::timebase_32768Hz_timer, PrescalerValue,
		      TIM_PSCReloadMode_Immediate);
  /* Output Compare Timing Mode configuration: Channel1 */
  TIM_OCInitTypeDef TIM_OCInitStructure;
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 650;	/* What is this and would would be a sane default ? */
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

  TIM_OC1Init(timebase_32768Hz::timebase_32768Hz_timer, &TIM_OCInitStructure);
  TIM_OC1PreloadConfig(timebase_32768Hz::timebase_32768Hz_timer,
		       TIM_OCPreload_Disable);
  /* TIM Interrupts enable */
  TIM_ITConfig(timebase_32768Hz::timebase_32768Hz_timer, TIM_IT_CC1, ENABLE);
  /* Start the timer. */
  TIM_Cmd(timebase_32768Hz::timebase_32768Hz_timer, ENABLE);
}
