#include "rtc_management.h"

#include <stm32f4xx_exti.h>

#define WAKEUP_BUTTON_EXTI_LINE          EXTI_Line0
#define TAMPER_BUTTON_EXTI_LINE          EXTI_Line13
#define KEY_BUTTON_EXTI_LINE             EXTI_Line15


/**
 * @brief  This function handles External line 0 interrupt request.
 * @param  None
 * @retval None
 */
void EXTI0_IRQHandler(void)
{
  /* Clear the TimeStamp registers */
  if(EXTI_GetITStatus(WAKEUP_BUTTON_EXTI_LINE) != RESET)
    {
      /* Turn LED1 ON and LED2 OFF */
      // STM_EVAL_LEDOn(LED1);
      // STM_EVAL_LEDOff(LED2);

      /* Clear The TSF Flag (Clear TimeStamp Registers) */
      RTC_ClearFlag(RTC_FLAG_TSF);
      // printf("\r\n******************** TimeStamp Event Cleared ********************");
      /* Clear the Wakeup Button EXTI line pending bit */
      EXTI_ClearITPendingBit(WAKEUP_BUTTON_EXTI_LINE);
    }
}


void EXTI15_10_IRQHandler(void)
{
  /* TimeStamp Event detected */
  if(EXTI_GetITStatus(TAMPER_BUTTON_EXTI_LINE) != RESET)
    {
      /* Turn LED4 ON */
      // STM_EVAL_LEDOn(LED4);
      // printf("\r\n******************** TimeStamp Event Occurred ********************");
      /* Clear the TAMPER Button EXTI line pending bit */
      EXTI_ClearITPendingBit(TAMPER_BUTTON_EXTI_LINE);
    }

  /* To display Current RTC Time and Date Calendar */
  if(EXTI_GetITStatus(KEY_BUTTON_EXTI_LINE) != RESET)
    {
      /* Turn LED4 ON */
      // STM_EVAL_LEDOn(LED4);

      /* Clear the KEY Button EXTI line pending bit */
      EXTI_ClearITPendingBit(KEY_BUTTON_EXTI_LINE);

      /* Display the Time */
      // RTC_TimeShow();
      /* Display the date */
      // RTC_DateShow();
      /* Display the TimeStamp */
      // RTC_TimeStampShow();
    }
}

