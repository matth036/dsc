#ifndef STM32_E407_UTILITIES_H
#define STM32_E407_UTILITIES_H

#define SRAM_STACK_HEAP_PROBULATE 1


#ifdef __cplusplus
extern "C" {
#endif

#include <stm32f4xx.h>

  uint32_t sram_stack_probulate( uint32_t depth );

  void STM32_E407_led_setup(void);

  void STM32_E407_button_setup(void);

  uint8_t button_pushed();

  void STM32_E407_led_on(void);

  void STM32_E407_led_off(void);
  /* n.b. Prefer on off over toggle for code that claims to be reentrant. */
  void STM32_E407_led_toggle(void);
#if SRAM_STACK_HEAP_PROBULATE
  /* This is usefull but seems to occupy a lot of flash memory.
   * Likely C++ heap allocation functions (new & delete & new[] & delete[]) are duplicated by 
   * the C allocation functions malloc() and free();
   *
   */
  uint32_t sram_stack_heap_probulate(uint32_t depth);
#endif

#ifdef __cplusplus
}
#endif
#endif
