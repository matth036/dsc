#include <initializer_list>
#include "stm32f4xx_gpio.h"
#include "specificities.h"

/* MACROS such as GPIO_Pin_7 are #defined in stm32f4xx_gpio.h */


std::initializer_list<uint16_t> specificities::char_lcd_pins
{
  GPIO_Pin_0,          /* RS  */
    GPIO_Pin_2,        /* R/W */
    GPIO_Pin_4,        /* E  (Enable) */
    GPIO_Pin_6,        /* DB0 */
    GPIO_Pin_8,        /* DB1 */
    GPIO_Pin_10,       /* DB2 */
    GPIO_Pin_14,       /* DB3 */
    GPIO_Pin_7,        /* DB4 */
    GPIO_Pin_9,        /* DB5 */
    GPIO_Pin_11,       /* DB6 */
    GPIO_Pin_15,       /* DB7 */
};

/* MACROS such as GPIOD are #defined in stm32f4xx_gpio.h                */
/* GPIO_TypeDef* is a pointer to a struct typedef found in stm32f4xx.h  */
std::initializer_list<GPIO_TypeDef*>  specificities::char_lcd_ports
{
  GPIOD,         /* RS  */
    GPIOD,       /* R/W */
    GPIOD,       /* E  (Enable) */
    GPIOD,       /* DB0 */
    GPIOD,       /* DB1 */
    GPIOD,       /* DB2 */
    GPIOD,       /* DB3 */
    GPIOD,       /* DB4 */
    GPIOD,       /* DB5 */
    GPIOD,       /* DB6 */
    GPIOD,       /* DB7 */
};

