#include "quadrature_decoder.h"
#include "rtc_management.h"
#include "microsecond_delay.h"
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_rcc.h>
#include "stm32f4xx_conf.h"

/*! 
 * On the stm32f4discovery board avoid these pins:
 * TIM1   should avoid    PA9  used by USB. (green led which indicates VBUS is present.) 
 * TIM2   should avoid    PB3  used by SWD  (Can be disconnected by opening a solder bridge.)
 * TIM4   should avoid    PD11 used by user LED 4, green.   
 * TIM4   should avoid    PD13 used by user LED 3, orange.
*/

Quadrature_Decoder::Quadrature_Decoder(GPIO_TypeDef * A_port, uint16_t A_pin,
				       GPIO_TypeDef * B_port, uint16_t B_pin,
				       TIM_TypeDef * TIMx, uint32_t ticks)
{
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));
  assert_param(IS_GPIO_ALL_PERIPH(A_port));
  assert_param(IS_GPIO_ALL_PERIPH(B_port));
  assert_param(1 == count_ones(A_pin));
  assert_param(1 == count_ones(B_pin));
  if (A_port == B_port) {
    assert_param(A_pin != B_pin);
  }
#if( 0 )
  port_pin_A->first = A_port;
  port_pin_A->second = A_pin;
  port_pin_B->first = B_port;
  port_pin_B->second = B_pin;
  port_pin_I->first = nullptr;	// No Index pin, set it to null.
  port_pin_I->second = 0;	// pin descriptions are singleton bitmasks. 0 is not valid.
#else
  port_pin_A = std::unique_ptr < port_pin_t > {
    new port_pin_t {
  A_port, A_pin}};
  port_pin_B = std::unique_ptr < port_pin_t > {
    new port_pin_t {
  B_port, B_pin}};
#endif
  _TIMx = TIMx;
  // _TIMx_up = std::unique_ptr<TIM_TypeDef *>( _TIMx );
  ticks_per_revolution = ticks;

  assert_param(is_viable());
  init();
  TIM_Cmd(_TIMx, DISABLE);
  TIM_SetCounter(_TIMx, ticks / 2);
  TIM_Cmd(_TIMx, ENABLE);
}

void Quadrature_Decoder::debug_init()
{
  assert_param(0);
}

void Quadrature_Decoder::init()
{
  rccPeriphClockEnable(port_pin_A->first);
  rccPeriphClockEnable(port_pin_B->first);
  counterPeriphClockEnable();

  pin_configure_alternate_function(port_pin_A->second, port_pin_A->first);
  pin_configure_alternate_function(port_pin_B->second, port_pin_B->first);

  pin_initiate(port_pin_A->second, port_pin_A->first);
  pin_initiate(port_pin_B->second, port_pin_B->first);

  TIM_configure();

  TIM_EncoderInterfaceConfig(_TIMx, TIM_EncoderMode_TI12,
			     TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);

  TIM_SetAutoreload(_TIMx, (ticks_per_revolution - 1));
}

void Quadrature_Decoder::pin_initiate(uint16_t pin, GPIO_TypeDef * port)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  //   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = pin;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  // probably irrelevant. Valid values are GPIO_OType_OD, GPIO_OType_PP
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;

  GPIO_Init(port, &GPIO_InitStructure);
}

void Quadrature_Decoder::TIM_configure()
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
  TIM_TimeBaseInitStruct.TIM_Prescaler = 0;
  TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
  if (_TIMx == TIM2 || _TIMx == TIM5) {	/* 32 bit counters */
    TIM_TimeBaseInitStruct.TIM_Period = 0xFFFFFFFF;
  } else {			/* 16 bit counters */
    TIM_TimeBaseInitStruct.TIM_Period = 0x0000FFFF;
  }
  TIM_TimeBaseInit(_TIMx, &TIM_TimeBaseInitStruct);
}

void Quadrature_Decoder::pin_configure_alternate_function(uint16_t pin,
							  GPIO_TypeDef * port)
{
  uint8_t pnum = pmask2pnum(pin);
  uint8_t GPIO_AF_TIMxxxx = stm32f4xx_timer2gpioAF(_TIMx);
  GPIO_PinAFConfig(port, pnum, GPIO_AF_TIMxxxx);
}

/*******************************
Example 
port = GPIOH
pin  = GPIO_Pin_7 = ((uint16_t)0x0080)  =  0000 0000 1000 0000b
The class member _TIMx (x=1,2,3,4,5, or 8) is an implicit argument.
_TIMx = TIM3

We calculate:
npin = 7  The number of zeros to the right of the one.
And make the funtion call
GPIO_PinAFConfig( port, 7, GPIO_AF_TIM3);

So, for the general case we need a map:
TIM1 -> GPIO_AF_TIM1 = 0x01
TIM2 -> GPIO_AF_TIM2 = 0x01
TIM3 -> GPIO_AF_TIM3 = 0x02
TIM4 -> GPIO_AF_TIM4 = 0x02
TIM5 -> GPIO_AF_TIM5 = 0x02
TIM8 -> GPIO_AF_TIM8 = 0x03

The key is (TIM_TypeDef *) and the value is (uint8_t).
THIS IS VERY HARDWARE SPECIFIC.
*******************/


uint8_t Quadrature_Decoder::stm32f4xx_timer2gpioAF(TIM_TypeDef * TIMx)
{
  if (TIMx == TIM1 || TIMx == TIM2) {
    const uint8_t rvalue = 0x01;
    static_assert(GPIO_AF_TIM1 == rvalue, "These are said to be equal");
    static_assert(GPIO_AF_TIM2 == rvalue, "This is the putative value");
    return rvalue;
  }
  if (TIMx == TIM3 || TIMx == TIM4 || TIMx == TIM5) {
    static_assert(GPIO_AF_TIM3 == GPIO_AF_TIM4, "These are said to be equal");
    static_assert(GPIO_AF_TIM3 == GPIO_AF_TIM5, "These are said to be equal");
    static_assert(GPIO_AF_TIM3 == 0x02, "This is the putative value.");
    return 0x02;
  }
  if (TIMx == TIM8) {
    static_assert(GPIO_AF_TIM8 == 0x03, "This is the putative value.");
    return 0x03;
  }
  assert_param(0);
  return 0xFF;
}

uint8_t Quadrature_Decoder::count_ones(uint16_t n)
{
  uint8_t s = 0;
  while (n) {
    if (n & 0x0001) {
      ++s;
    }
    n = n >> 1;
  }
  return s;
}

uint8_t Quadrature_Decoder::pmask2pnum(uint16_t pin)
{
  uint8_t c1 = 0;		// count ones.
  uint8_t c0 = 0;		// count zeros.
  do {
    if (pin & 0x01) {
      ++c1;
    } else {
      ++c0;
    }
    pin = pin >> 1;
  } while (pin);

  if (c1 == 1) {
    return c0;			// the number of zeros to the right of the only one.
  } else {
    for (;;) {			/* Error Condition */
    }
  }
  return 255;			// never get here.
}

    /* 
       A timer in the context of a quadrature follower 
       is a counter.
     */
void Quadrature_Decoder::counterPeriphClockEnable()
{
  // Note that TIM1 and TIM8 call a different function.
  if (_TIMx == TIM1) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
  } else if (_TIMx == TIM2) {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  } else if (_TIMx == TIM3) {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  } else if (_TIMx == TIM4) {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
  } else if (_TIMx == TIM5) {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
  } else if (_TIMx == TIM8) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
  }

}

void Quadrature_Decoder::counterEnable()
{
  TIM_Cmd(_TIMx, ENABLE);
}

void Quadrature_Decoder::counterDisable()
{
  TIM_Cmd(_TIMx, DISABLE);
}

void Quadrature_Decoder::zeroCounter()
{
  TIM_Cmd(_TIMx, DISABLE);
  TIM_SetCounter(_TIMx, 0);
  TIM_Cmd(_TIMx, ENABLE);
}

void Quadrature_Decoder::harshReset()
{
  assert_param(false);
  __disable_irq();
  TIM_Cmd(_TIMx, DISABLE);
  TIM_SetCounter(_TIMx, ticks_per_revolution / 2);
  TIM_Cmd(_TIMx, ENABLE);
  __enable_irq();
}

uint8_t Quadrature_Decoder::getTimerNumber()
{
  if (_TIMx == TIM1) {
    return 1;
  } else if (_TIMx == TIM2) {
    return 2;
  } else if (_TIMx == TIM3) {
    return 3;
  } else if (_TIMx == TIM4) {
    return 4;
  } else if (_TIMx == TIM5) {
    return 5;
  } else if (_TIMx == TIM8) {
    return 8;
  } else {
    while (1) {
      // Never return;
    }
  }
  return 0x00;
}

const uint32_t Quadrature_Decoder::get_count()
{
  return TIM_GetCounter(_TIMx);
}

void Quadrature_Decoder::set_count(uint32_t value){
  TIM_SetCounter(_TIMx, value);
}

const uint32_t Quadrature_Decoder::get_ticks_per_revolution(){
  return ticks_per_revolution;
}

void Quadrature_Decoder::swapChannels()
{
  std::swap(port_pin_A, port_pin_B);
}

    /************************************************************************************ 
Some reverse engineering to get from (say) GPIOD to RCC_AHB1Periph_GPIOD
This allows us to calculate the first argument in
RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOD   , ENABLE);
and the like.

RCC_AHB1PeriphClockCmd is 
grep RCC_AHB1Periph_GPIO[A-J] ./Libraries/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_rcc.h
#define RCC_AHB1Periph_GPIOA             ((uint32_t)0x00000001)
#define RCC_AHB1Periph_GPIOB             ((uint32_t)0x00000002)
#define RCC_AHB1Periph_GPIOC             ((uint32_t)0x00000004)
#define RCC_AHB1Periph_GPIOD             ((uint32_t)0x00000008)
#define RCC_AHB1Periph_GPIOE             ((uint32_t)0x00000010)
#define RCC_AHB1Periph_GPIOF             ((uint32_t)0x00000020)
#define RCC_AHB1Periph_GPIOG             ((uint32_t)0x00000040)
#define RCC_AHB1Periph_GPIOH             ((uint32_t)0x00000080)
#define RCC_AHB1Periph_GPIOI             ((uint32_t)0x00000100)

....STM32F4-Discovery_FW_V1.1.0$ grep -Rh GPIO[A-J]_BASE .
#define GPIOA_BASE            (AHB1PERIPH_BASE + 0x0000)
#define GPIOB_BASE            (AHB1PERIPH_BASE + 0x0400)
#define GPIOC_BASE            (AHB1PERIPH_BASE + 0x0800)
#define GPIOD_BASE            (AHB1PERIPH_BASE + 0x0C00)
#define GPIOE_BASE            (AHB1PERIPH_BASE + 0x1000)
#define GPIOF_BASE            (AHB1PERIPH_BASE + 0x1400)
#define GPIOG_BASE            (AHB1PERIPH_BASE + 0x1800)
#define GPIOH_BASE            (AHB1PERIPH_BASE + 0x1C00)
#define GPIOI_BASE            (AHB1PERIPH_BASE + 0x2000)
#define GPIOA               ((GPIO_TypeDef *) GPIOA_BASE)
#define GPIOB               ((GPIO_TypeDef *) GPIOB_BASE)
#define GPIOC               ((GPIO_TypeDef *) GPIOC_BASE)
#define GPIOD               ((GPIO_TypeDef *) GPIOD_BASE)
#define GPIOE               ((GPIO_TypeDef *) GPIOE_BASE)
#define GPIOF               ((GPIO_TypeDef *) GPIOF_BASE)
#define GPIOG               ((GPIO_TypeDef *) GPIOG_BASE)
#define GPIOH               ((GPIO_TypeDef *) GPIOH_BASE)
#define GPIOI               ((GPIO_TypeDef *) GPIOI_BASE)
    **************************************************************/
void Quadrature_Decoder::rccPeriphClockEnable(GPIO_TypeDef * GPIOx)
{
  uint32_t offset = (uint32_t) GPIOx - (uint32_t) GPIOA;	// x-A 
  uint32_t pitch = (uint32_t) GPIOB - (uint32_t) GPIOA;	// Constant. In fact 0x0400.
  uint32_t portnum = offset / pitch;	// A->0, B->1, C->2 ...
  uint32_t magic = (uint32_t) 1 << portnum;
  // 1000 0001 1000 1011 1110 1110 0000 0000
  // This assertion employs the macro at stm32f4xx_rcc.h : 275
  assert_param(IS_RCC_AHB1_CLOCK_PERIPH(magic));
  RCC_AHB1PeriphClockCmd(magic, ENABLE);
}

bool Quadrature_Decoder::is_viable()
{
  if (is_direct_viable()) {
    return true;
  } else if (is_swaped_viable()) {
    return true;
  }
  return false;
}

bool Quadrature_Decoder::is_swaped_viable()
{
  swapChannels();
  bool rvalue = is_direct_viable();
  swapChannels();
  return rvalue;
}

bool Quadrature_Decoder::is_direct_viable()
{
  if (_TIMx == TIM1) {
    return is_viable_1();
  } else if (_TIMx == TIM2) {
    return is_viable_2();
  } else if (_TIMx == TIM3) {
    return is_viable_3();
  } else if (_TIMx == TIM4) {
    return is_viable_4();
  } else if (_TIMx == TIM5) {
    return is_viable_5();
  } else if (_TIMx == TIM8) {
    return is_viable_8();
  }
  return false;
}

bool Quadrature_Decoder::is_viable_1()
{
  // PA8 or PE9
  bool A_ok = (*port_pin_A == std::make_pair(GPIOA, GPIO_Pin_8)) ||
      (*port_pin_A == std::make_pair(GPIOE, GPIO_Pin_9));
  // PA9 or PE11
  bool B_ok = (*port_pin_B == std::make_pair(GPIOA, GPIO_Pin_9)) ||
      (*port_pin_B == std::make_pair(GPIOE, GPIO_Pin_11));
  return A_ok && B_ok;
}

bool Quadrature_Decoder::is_viable_2()
{
  // PA0 or PA5 or PA15
  bool A_ok = (*port_pin_A == std::make_pair(GPIOA, GPIO_Pin_0)) ||
      (*port_pin_A == std::make_pair(GPIOA, GPIO_Pin_5)) ||
      (*port_pin_A == std::make_pair(GPIOA, GPIO_Pin_15));
  // PA1 or PB3
  bool B_ok = (*port_pin_B == std::make_pair(GPIOA, GPIO_Pin_1)) ||
      (*port_pin_B == std::make_pair(GPIOB, GPIO_Pin_3));
  return A_ok && B_ok;
}

bool Quadrature_Decoder::is_viable_3()
{
  // PA6 or PB4 or PC6
  bool A_ok = (*port_pin_A == std::make_pair(GPIOA, GPIO_Pin_6)) ||
      (*port_pin_A == std::make_pair(GPIOB, GPIO_Pin_4)) ||
      (*port_pin_A == std::make_pair(GPIOC, GPIO_Pin_6));
  // PA7 or PB6 or PC7
  bool B_ok = (*port_pin_B == std::make_pair(GPIOA, GPIO_Pin_7)) ||
      (*port_pin_B == std::make_pair(GPIOB, GPIO_Pin_6)) ||
      (*port_pin_B == std::make_pair(GPIOC, GPIO_Pin_7));
  return A_ok && B_ok;
}

bool Quadrature_Decoder::is_viable_4()
{
  // PB6 or PD12
  bool A_ok = (*port_pin_A == std::make_pair(GPIOB, GPIO_Pin_6)) ||
      (*port_pin_A == std::make_pair(GPIOD, GPIO_Pin_12));
  // PB7 or PD13
  bool B_ok = (*port_pin_B == std::make_pair(GPIOB, GPIO_Pin_7)) ||
      (*port_pin_B == std::make_pair(GPIOD, GPIO_Pin_13));
  return A_ok && B_ok;
}

bool Quadrature_Decoder::is_viable_5()
{
  // PA0 or PH10
  bool A_ok = (*port_pin_A == std::make_pair(GPIOA, GPIO_Pin_0)) ||
      (*port_pin_A == std::make_pair(GPIOH, GPIO_Pin_10));
  // PA1 or PH11
  bool B_ok = (*port_pin_B == std::make_pair(GPIOA, GPIO_Pin_1)) ||
      (*port_pin_B == std::make_pair(GPIOH, GPIO_Pin_11));
  return A_ok && B_ok;
}

    /* This seems to overlap a TIM3  set of pins.  How can that be?  */
bool Quadrature_Decoder::is_viable_8()
{
  // PC6 or PI5
  bool A_ok = (*port_pin_A == std::make_pair(GPIOC, GPIO_Pin_6)) ||
      (*port_pin_A == std::make_pair(GPIOI, GPIO_Pin_5));
  // PC7 or PI6
  bool B_ok = (*port_pin_B == std::make_pair(GPIOC, GPIO_Pin_7)) ||
      (*port_pin_B == std::make_pair(GPIOI, GPIO_Pin_6));
  return A_ok && B_ok;
}

/*****************************************
TIM_EncoderInterfaceConfig(...)  invoces  stm32f4xx_tim.c
assert_param(IS_TIM_LIST2_PERIPH(TIMx));
LIST2 includes the Quadrature capable timers plus TIM9 and TIM12
There are other encoding schemes. E.g. Ch.2 = diretion, Ch.1 = counts ticks.

TIM9 and TIM12  only count up.
 *****************************************/


const uint32_t Fake_Decoder::get_count(){
  return count;
}

const uint32_t Fake_Decoder::get_ticks_per_revolution(){
  return ticks_per_revolution;
}

void Fake_Decoder::set_count( uint32_t value ){
  ticks_per_revolution = value; 
}

// EOF
