#ifndef QUADRATURE_DECODER_H
#define QUADRATURE_DECODER_H

/****************************  
DM00042534.pdf describes timers & counters
(There is another ST document I want to find.)

Two advanced controll timers 
TIM1 and TIM8 
support quadrature.

Four general purpose timers
TIM2 (32bit)
TIM3
TIM4
TIM5 (32bit)
support quadrature.

All of these timers have four capture compare channels.
I have assumed _CH1 and _CH2 are to be used for encoder interaction.
Is this necessary? Could _CH3 and _CH4 be used?
Attempt to use _CH3 and _CH4 failed.
****************************/

//   Libraries/CMSIS/ST/STM32F4xx/Include/stm32f4xx.h
#include "stm32f4xx.h"
#include <utility>
#include <memory>

using std::pair;
using port_pin_t = pair < GPIO_TypeDef *, uint16_t >;
using upp_ptr = std::unique_ptr < port_pin_t >;


class Decoder_Interface{
 public:
  ~Decoder_Interface(){}
  virtual const uint32_t get_count()=0;
  virtual const uint32_t get_ticks_per_revolution()=0;
  virtual void set_count( uint32_t )=0;
};


class Quadrature_Decoder : public Decoder_Interface {

 public:

  Quadrature_Decoder(GPIO_TypeDef * A_port, uint16_t A_pin,
		     GPIO_TypeDef * B_port, uint16_t B_pin,
		     TIM_TypeDef * TIMx, uint32_t ticks);
  void swapChannels();
  void zeroCounter();
  const uint32_t get_count();
  const uint32_t get_ticks_per_revolution();
  void set_count( uint32_t );
  inline bool hasIndex() {
    return (port_pin_I->first && port_pin_I->second);
  }
  uint8_t getTimerNumber();
  void counterEnable();
  void counterDisable();
  static uint8_t count_ones(uint16_t n);
  bool is_viable();
  bool is_direct_viable();
  bool is_swaped_viable();
 private:
  void init();
  void debug_init();
  void harshReset();
  void pin_initiate(uint16_t pin, GPIO_TypeDef * port);
  void pin_configure_alternate_function(uint16_t pin, GPIO_TypeDef * port);
  /* Consider a better name. */
  void rccPeriphClockEnable(GPIO_TypeDef *);

  void counterPeriphClockEnable();
  void TIM_configure();
  bool is_viable_1();
  bool is_viable_2();
  bool is_viable_3();
  bool is_viable_4();
  bool is_viable_5();
  bool is_viable_8();

  void configTimer();
  uint8_t pmask2pnum(uint16_t);
  uint8_t stm32f4xx_timer2gpioAF(TIM_TypeDef * TIMx);
  /* Class members */
  //  Experimenting:
  std::unique_ptr < TIM_TypeDef * >_TIMx_up;;
  TIM_TypeDef *_TIMx;
  uint32_t ticks_per_revolution;
  upp_ptr port_pin_A;
  upp_ptr port_pin_B;
  std::pair < GPIO_TypeDef *, uint16_t > *port_pin_I;

};


/* For experimentation. */
class Fake_Decoder : public Decoder_Interface {
 public:
  Fake_Decoder();
  Fake_Decoder( Fake_Decoder& );
  ~Fake_Decoder();

  const uint32_t get_count();
  const uint32_t get_ticks_per_revolution();
  void set_count( uint32_t );
 private:
  uint32_t count;
  uint32_t ticks_per_revolution;
};


#endif	/*** eof QUADRATURE_DECODER_H */
