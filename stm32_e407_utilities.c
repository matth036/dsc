#include "stm32_e407_utilities.h"
#include <stdlib.h>
#include <stm32f4xx_gpio.h>
void STM32_E407_button_setup()
{
  /* GPIOA Periph clock enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  /* Configure user push button. */
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  // UNTESTED. Probably OK.
}

void STM32_E407_led_setup()
{
  /* GPIOC Periph clock enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  /* Configure PC13 a in output pushpull mode */
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/* 
 * After the button has been setup, this will read the state of the push button. 
 * The output is suitable to be treated as a boolean.  
 */
uint8_t button_pushed()
{
  return GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0);
}

/* 
 * See the code for 
 * void GPIO_SetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
 * and 
 * void GPIO_ResetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
 * in 
 * stm32f4xx_gpio.c
 * 
 * Note that the toggle doesn't come with the atomic write safety 
 * promise that: 
 *  ... there is no risk of an IRQ occurring between
 * the read and the modify access.
 * 
 * So, prefer on and off over toggle for code that claims to be reentrant. 
 * 
 * Note March 2014.  @TODO Read the comments in stm32f4xx_gpio.c and investigate the
 * possibility of implementing a mutex (or semaphore) at the cost of one IO pin.
 * 
 */

void STM32_E407_led_on(void)
{
  GPIOC->BSRRL = GPIO_Pin_13;
}

void STM32_E407_led_off(void)
{
  GPIOC->BSRRH = GPIO_Pin_13;
}

/* Prefer on off over toggle for code that claims to be reentrant. */
void STM32_E407_led_toggle(void)
{
  GPIOC->ODR ^= GPIO_Pin_13;
}

#define BLOCK_SIZE 8
uint32_t sram_stack_heap_probulate(uint32_t depth)
{
  static uint32_t max_heap = 0x00000000;
  static uint32_t min_heap = 0xFFFFFFFF;
  static uint32_t max_stack = 0x00000000;
  static uint32_t min_stack = 0xFFFFFFFF;
  uint32_t max_heap_local;
  uint32_t min_heap_local;
  uint32_t max_stack_local;
  uint32_t min_stack_local;
  uint32_t stack_item = 0xDEADBEEF;
  uint32_t stack_address;
  uint32_t heap_address;
  uint32_t *some_data;
  int i;
  stack_address = (uint32_t) & stack_item;
  if (stack_address > max_stack) {
    max_stack = stack_address;
  }
  if (stack_address < min_stack) {
    min_stack = stack_address;
  }
  some_data = (uint32_t *) malloc(BLOCK_SIZE * sizeof(uint32_t));
  if (!some_data) {
    /* Because * don't know how to examine the static versions in the debugger. */
    max_heap_local = max_heap;
    min_heap_local = min_heap;
    max_stack_local = max_stack;
    min_stack_local = min_stack;
    /*****************
     * In the debugger, break at this return and do this:
     * (gdb) print (max_stack_local - min_heap_local)/1024
     * $10 = 122
     * (gdb) print (min_stack_local - max_heap_local)/1024
     * $11 = 1
    ******************/
    return max_stack - min_heap;
  } else {
    for (i = 0; i < BLOCK_SIZE; ++i) {
      some_data[i] = 0x0000ABBA;
    }
    heap_address = (uint32_t) (some_data);
    if (heap_address > max_heap) {
      max_heap = heap_address;
    }
    if (heap_address < min_heap) {
      min_heap = heap_address;
    }

    if (depth > 0) {
      sram_stack_heap_probulate(depth - 1);
    }
    /* Because * don't know how to examine the static versions in the debugger. */
    max_heap_local = max_heap;
    min_heap_local = min_heap;
    max_stack_local = max_stack;
    min_stack_local = min_stack;
    some_data[0] = max_heap_local;
    some_data[1] = min_heap_local;
    some_data[2] = max_stack_local;
    some_data[3] = min_stack_local;
    some_data[4] = 777777;
    free( some_data );

    
    return max_stack - min_heap;
  }


}
