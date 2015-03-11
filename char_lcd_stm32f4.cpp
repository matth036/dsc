#include "char_lcd_stm32f4.h"
#include "stm32f4xx.h"
#include "microsecond_delay.h"
#include <stdio.h>

#include<string>
#include <inttypes.h>
// #include "Arduino.h"
#include <vector>
// void microsecond_delay( uint16_t );
using MicroSecondDelay::microsecond_delay;
using std::vector;

// When the display powers up, it is configured as follows:
//
// 1. Display clear
// 2. Function set: 
//    DL = 1; 8-bit interface data 
//    N = 0; 1-line display 
//    F = 0; 5x8 dot character font 
// 3. Display on/off control: 
//    D = 0; Display off 
//    C = 0; Cursor off 
//    B = 0; Blinking off 
// 4. Entry mode set: 
//    I/D = 1; Increment by 1 
//    S = 0; No shift 
//
// Note, however, that resetting the Arduino doesn't reset the LCD, so we
// can't assume that its in that state when a sketch starts (and the
// LiquidCrystal constructor is called).

  /* The size() of pins is 
     [4 or 8](data pins) + [2 or 3](rw,enable and optionally rw). 
     = [4|8] + [2|3]
     ==> {6,7,10,11}  
     If ports.size() < pins.size() use ports.end() for the rest of the pins. */
CharLCD_STM32F::CharLCD_STM32F(initializer_list < pin > pins,
			       initializer_list < port > ports)
{
  const uint32_t npins = pins.size();
  const uint32_t nports = ports.size();
  assert_param( nports >= 1 );
  if (!(npins == 6 || npins == 7 || npins == 10 || npins == 11)) {
    assert_param( 0 );
    return;			// Error conditions npins.
  }
  const uint8_t use_rw = (npins == 7 || npins == 11);
  const uint8_t fourbitmode = (npins == 6 || npins == 7);
  /* Use the last port listed as a default. */
  const auto fallback_port = ports.begin()[nports - 1];
  for (auto it = ports.begin(); it != ports.end(); ++it) {
    /* I have a magic formula for this in the Quadrature encoder source.
     * I will need this type of code again so it may be well to  put it
     * in a communally accessable file.  
     */ 
    if (*it == GPIOA) {
      RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    } else if (*it == GPIOB) {
      RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    } else if (*it == GPIOC) {
      RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    } else if (*it == GPIOD) { /* This case has been tested. */
      RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    } else if (*it == GPIOE) { /* This case has been tested. */
      RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    } else if (*it == GPIOF) {
      RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
    } else if (*it == GPIOG) {
      RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
    } else if (*it == GPIOH) {
      RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE);
    } else if (*it == GPIOI) {
      RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI, ENABLE);
    } else {
      assert_param(0);
    }
  }
  auto pin2use = pins.begin();
  auto port2use = ports.begin();

  /* RS, R/W, E, DB0, DB1, DB2, DB3, DB4, DB5, DB6, DB7 */
  _rs_pin = *pin2use;
  _rs_port = *port2use;
  ++pin2use, ++port2use;
  if (use_rw) {
    _rw_pin = *pin2use;
    _rw_port = port2use ? fallback_port : *port2use;
    ++pin2use, ++port2use;
  } else {
    _rw_pin = 255;		/* Sentinal value used in Arduino. */
    _rw_port = nullptr;
  }
  _enable_pin = *pin2use;
  _enable_port = port2use ? fallback_port : *port2use;
  ++pin2use, ++port2use;
  for (uint32_t i = 0; i < 8; ++i) {
    _data_pins[i] = *pin2use;
    _data_ports[i] = port2use ? fallback_port : *port2use;
    ++pin2use, ++port2use;
  }
  if (fourbitmode) {		/* copy pin info in 0123 to 4567 where it belongs. */
    for (uint32_t i = 0; i < 4; ++i) {
      _data_pins[i + 4] = _data_pins[i];
      _data_ports[i + 4] = _data_ports[i];
    }
  }
  gpio_init_comands();
  if (fourbitmode) {
    _displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
    gpio_init_msb();
  } else {
    _displayfunction = LCD_8BITMODE | LCD_1LINE | LCD_5x8DOTS;
    gpio_init_lsb();
    gpio_init_msb();
  }
  blocked_for_PWM = false;
  begin(16, 1);

}

/*! Enables all relevant pins to output mode. */
void CharLCD_STM32F::stm32f4_init(uint8_t fourbitmode)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  //
  GPIO_InitStructure.GPIO_Pin = _rs_pin;
  GPIO_Init(_rs_port, &GPIO_InitStructure);
  // we can save 1 pin by not using RW. Indicate by passing 255 instead of pin#
  if (_rw_pin != 255) {
    GPIO_InitStructure.GPIO_Pin = _rw_pin;
    GPIO_Init(_rw_port, &GPIO_InitStructure);
  }
  GPIO_InitStructure.GPIO_Pin = _enable_pin;
  GPIO_Init(_enable_port, &GPIO_InitStructure);
  //
  int end = fourbitmode ? 4 : 8;
  for (int i = 0; i < end; ++i) {
    GPIO_InitStructure.GPIO_Pin = _data_pins[i];
    GPIO_Init(_data_ports[i], &GPIO_InitStructure);
  }
}


void CharLCD_STM32F::gpio_init_comands()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = _rs_pin;
  GPIO_Init(_rs_port, &GPIO_InitStructure);
  // Arduino convention: if _rw_pin == 255, The pin in the lcd [is|should_be] grounded.
  if (_rw_pin != 255) {
    GPIO_InitStructure.GPIO_Pin = _rw_pin;
    GPIO_Init(_rw_port, &GPIO_InitStructure);
  }
  GPIO_InitStructure.GPIO_Pin = _enable_pin;
  GPIO_Init(_enable_port, &GPIO_InitStructure);
}

void CharLCD_STM32F::gpio_init_lsb()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  for (int i = 0; i < 4; ++i) {
    GPIO_InitStructure.GPIO_Pin = _data_pins[i];
    GPIO_Init(_data_ports[i], &GPIO_InitStructure);
  }
}

void CharLCD_STM32F::gpio_init_msb()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  for (int i = 4; i < 8; ++i) {
    GPIO_InitStructure.GPIO_Pin = _data_pins[i];
    GPIO_Init(_data_ports[i], &GPIO_InitStructure);
  }
}



void CharLCD_STM32F::begin(uint8_t cols, uint8_t lines, uint8_t dotsize)
{
  if (lines > 1) {
    _displayfunction |= LCD_2LINE;
  }
  _numlines = lines;
  _currline = 0;

  // for some 1 line displays you can select a 10 pixel high font
  if ((dotsize != 0) && (lines == 1)) {
    _displayfunction |= LCD_5x10DOTS;
  }
  // SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
  // according to datasheet, we need at least 40ms after power rises above 2.7V
  // before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
  microsecond_delay(0xFFFF);
  microsecond_delay(0xFFFF);

  // Now we pull both RS and R/W low to begin commands
  rs_off();
  enable_off();
  rw_off();
  //put the LCD into 4 bit or 8 bit mode
  if (!(_displayfunction & LCD_8BITMODE)) {
    // this is according to the hitachi HD44780 datasheet
    // figure 24, pg 46

    // we start in 8bit mode, try to set 4 bit mode
    write4bits(0x03);
    microsecond_delay(4500);	// wait min 4.1ms

    // second try
    write4bits(0x03);
    microsecond_delay(4500);	// wait min 4.1ms

    // third go!
    write4bits(0x03);
    microsecond_delay(150);

    // finally, set to 4-bit interface
    write4bits(0x02);
  } else {
    // this is according to the hitachi HD44780 datasheet
    // page 45 figure 23

    // Send function set command sequence
    command(LCD_FUNCTIONSET | _displayfunction);
    microsecond_delay(4500);	// wait more than 4.1ms

    // second try
    command(LCD_FUNCTIONSET | _displayfunction);
    microsecond_delay(150);

    // third go
    command(LCD_FUNCTIONSET | _displayfunction);
  }


  // finally, set # lines, font size, etc.
  command(LCD_FUNCTIONSET | _displayfunction);

  // turn the display on with no cursor or blinking default
  _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
  display();

  // clear it off
  clear();

  // Initialize to default text direction (for romance languages)
  _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
  // set the entry mode
  command(LCD_ENTRYMODESET | _displaymode);
}

/********** high level commands, for the user! */
void CharLCD_STM32F::clear()
{
  command(LCD_CLEARDISPLAY);	// clear display, set cursor position to zero
  microsecond_delay(2000);	// this command takes a long time!
  microsecond_delay(0xFFFF);    // NEWHAVEN_OLED_MOD
}

void CharLCD_STM32F::home()
{
  command(LCD_RETURNHOME);	// set cursor position to zero
  /* For Newhaven OLED displays, this extra delay is not needed.  
   * Keep it in for compatibility with Hitachi compatable LCDs.
   */
  //microsecond_delay(2000);	// this command takes a long time!
  microsecond_delay(600);	// this command takes a long time!
}

void CharLCD_STM32F::VARIOUS_HACKS(){

}

void CharLCD_STM32F::row_switcheroo(){
  swap_rows(0,1);
  swap_rows(2,3);
}

void CharLCD_STM32F::swap_rows(int a, int b){
  if( a<0 || b<0 ){
    return;
  }
  if( a>3 || b>3 ){
    return;
  }
  int temp;
  temp = row_offsets[a];
  row_offsets[a] = row_offsets[b];
  row_offsets[b] = temp;
}

void CharLCD_STM32F::setCursor(uint8_t col, uint8_t row)
{
  /* HACK ALERT:  I HAVE NO IDEA WHY THESE ARE SHUFFLED ON MY EXAMPLE. 
   * Documentation indicates the traditional layout is correct. 
   *
   * I have made row_offsets[] a class member and provided a means to
   * permute the entries. See ::swap_rows(int,int);
   * 
   */ 
  // int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
  // int row_offsets[] = { 0x40, 0x00, 0x54, 0x14 };
  if (row >= _numlines) {
    row = _numlines - 1;	// we count rows starting w/0
  }
  command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

// Turn the display on/off (quickly)
void CharLCD_STM32F::noDisplay()
{
  _displaycontrol &= ~LCD_DISPLAYON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void CharLCD_STM32F::display()
{
  _displaycontrol |= LCD_DISPLAYON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turns the underline cursor on/off
void CharLCD_STM32F::noCursor()
{
  _displaycontrol &= ~LCD_CURSORON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void CharLCD_STM32F::cursor()
{
  _displaycontrol |= LCD_CURSORON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turn on and off the blinking cursor
void CharLCD_STM32F::noBlink()
{
  _displaycontrol &= ~LCD_BLINKON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void CharLCD_STM32F::blink()
{
  _displaycontrol |= LCD_BLINKON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void CharLCD_STM32F::scrollDisplayLeft(void)
{
  command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

void CharLCD_STM32F::scrollDisplayRight(void)
{
  command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void CharLCD_STM32F::leftToRight(void)
{
  _displaymode |= LCD_ENTRYLEFT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This is for text that flows Right to Left
void CharLCD_STM32F::rightToLeft(void)
{
  _displaymode &= ~LCD_ENTRYLEFT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'right justify' text from the cursor
void CharLCD_STM32F::autoscroll(void)
{
  _displaymode |= LCD_ENTRYSHIFTINCREMENT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'left justify' text from the cursor
void CharLCD_STM32F::noAutoscroll(void)
{
  _displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void CharLCD_STM32F::createChar(uint8_t location, uint8_t charmap[])
{
  location &= 0x7;		// we only have 8 locations 0-7
  command(LCD_SETCGRAMADDR | (location << 3));
  for (int i = 0; i < 8; i++) {
    write(charmap[i]);
  }
}

void CharLCD_STM32F::rs_on()
{
  GPIO_SetBits(_rs_port, _rs_pin);
}

void CharLCD_STM32F::rw_on()
{
  if (_rw_pin != 255) {
    GPIO_SetBits(_rw_port, _rw_pin);
  }
}

void CharLCD_STM32F::enable_on()
{
  GPIO_SetBits(_enable_port, _enable_pin);
}

void CharLCD_STM32F::rs_off()
{
  GPIO_ResetBits(_rs_port, _rs_pin);
}

void CharLCD_STM32F::rw_off()
{
  if (_rw_pin != 255) {
    GPIO_ResetBits(_rw_port, _rw_pin);
  }
}

void CharLCD_STM32F::enable_off()
{
  GPIO_ResetBits(_enable_port, _enable_pin);
}

/*********** mid level commands, for sending data/cmds */

inline void CharLCD_STM32F::command(uint8_t value)
{
  send(value, 0x0); /* In the Arduino code 0x0 was the macro LOW. */
}

/* n.b. 
 * size_t write(uint8_t)
 * is a purely virtual function in super class Print 
 *
 * There also is a write( uint8_t, size_t) 
 **/
inline size_t CharLCD_STM32F::write(uint8_t value)
{
  send(value, 0x1); /* In the Arduino code 0x1 was the macro HIGH. */
  return 1;			// assume sucess
}

/************ low level data pushing commands **********/

// write either command or data, with automatic 4/8-bit selection
void CharLCD_STM32F::send(uint8_t value, uint8_t mode)
{
  while( blocked_for_PWM ){
    /* All non PWM commands and writes wait. */
  }
  if (mode) {
    rs_on();
  } else {
    rs_off();
  }
  // if there is a RW pin indicated, set it low to Write
  rw_off();

  if (_displayfunction & LCD_8BITMODE) {
    write8bits(value);
  } else {
    write4bits(value >> 4);
    write4bits(value);
  }
}

void CharLCD_STM32F::pwm_flip_on(){
  if( blocked_for_PWM ){
    return;  
  }
  blocked_for_PWM = true;
  _displaycontrol |= LCD_DISPLAYON;
  uint8_t value = LCD_DISPLAYCONTROL | _displaycontrol;
  rs_off();
  rw_off();
  if (_displayfunction & LCD_8BITMODE) {
    write8bits(value);
  } else {
    write4bits(value >> 4);
    write4bits(value);
  }
  blocked_for_PWM = false;
}

void CharLCD_STM32F::pwm_flip_off(){
  if( blocked_for_PWM ){
    return;  
  }
  blocked_for_PWM = true;
  rs_off();
  rw_off();
  constexpr uint8_t value = LCD_DISPLAYOFF;
  if (_displayfunction & LCD_8BITMODE) {
    write8bits(value);
  } else {
    write4bits(value >> 4);
    write4bits(value);
  }
  blocked_for_PWM = false;
}

void CharLCD_STM32F::pulseEnable(void)
{
  // digitalWrite(_enable_pin, LOW);
  enable_off();
  microsecond_delay(1);
  //digitalWrite(_enable_pin, HIGH);
  enable_on();
  microsecond_delay(1);		// enable pulse must be >450ns
  //  digitalWrite(_enable_pin, LOW);
  enable_off();
  /* For Hitachi LCDs this next line was:
   * microsecond_delay(100);   // commands need > 37us to settle
   * This delay has been increased to comply with the delay indicated on 
   * Newhaven Displays OLED displays.
   */
  microsecond_delay(600); // NEWHAVE_OLED_MOD was 100 in Arduino, 37 in Hitachi spec.
}

void CharLCD_STM32F::write4bits(uint8_t value)
{
  if( _rw_pin != 255 ){
      enable_off();
      microsecond_delay(1);
  }
    /* 
       NOTICE THAT IF THIS WORKS, IT IS AN ACCIDENT.
       In four bit mode, data is written to the high order bits [d4,d5,d6,d7].
       I just happen to have a copy of those pins and ports in the low order bits.
       Change this code when there is time to check the change.
     */

  for (int i = 0; i < 4; i++) {
    // The arduino code sets the pin to output mode first. 
    // I don't see the point. And it is complicated with GPIO_Init used by STM32Fxx
    if ((value >> i) & 0x01) {
      GPIO_SetBits(_data_ports[i], _data_pins[i]);
    } else {
      GPIO_ResetBits(_data_ports[i], _data_pins[i]);
    }
  }
  if( _rw_pin == 255 ){
    pulseEnable();
  }else{
    enable_on();
    microsecond_delay(1);
    ready_wait();
  }

}

void CharLCD_STM32F::write8bits(uint8_t value)
{
  if( _rw_pin != 255 ){
    enable_off();
    microsecond_delay(1);
  }
  for (int i = 0; i < 8; i++) {
    // The arduino code sets the pin to output mode first. 
    // I don't see the point. And is is complicated with GPIO_Init used by STM32Fxx
    if ((value >> i) & 0x01) {
      GPIO_SetBits(_data_ports[i], _data_pins[i]);
    } else {
      GPIO_ResetBits(_data_ports[i], _data_pins[i]);
    }
  }
  if( _rw_pin == 255 ){
    pulseEnable();
  }else{
    enable_on();
    microsecond_delay(1);
    ready_wait();
  }
}

uint8_t CharLCD_STM32F::read8bits(){
  if( _rw_pin == 255 ){
    return 0;
  }
  enable_off();
  rs_on();
  rw_on();
  uint8_t data = 0;
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  for( int i=0; i<8; ++i ){
    GPIO_InitStructure.GPIO_Pin = _data_pins[i];
    GPIO_Init(_data_ports[i], &GPIO_InitStructure);
  }
  enable_on(); // Low to high transition triggers data write.
  microsecond_delay(600);
  for( int i=0;i<8; ++i ){
    uint8_t bit = GPIO_ReadInputDataBit(_data_ports[i],_data_pins[i]);
    if( bit ){
      data |= (0x01 << i);
    }
  }

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  for( int i=0;i<8; ++i ){
    GPIO_InitStructure.GPIO_Pin = _data_pins[i];
    GPIO_Init(_data_ports[i], &GPIO_InitStructure);
  }
  rw_off();
  return data;
}

/************************
Program received signal SIGINT, Interrupt.
0x08044f82 in TIM_GetFlagStatus (TIMx=0x40002000, TIM_FLAG=2)
    at /home/dan/stm32f4_discovery/STM32F4-Discovery_FW_V1.1.0//Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_tim.c:2437
2437      assert_param(IS_TIM_ALL_PERIPH(TIMx));
(gdb) bt
#0  0x08044f82 in TIM_GetFlagStatus (TIMx=0x40002000, TIM_FLAG=2)
    at /home/dan/stm32f4_discovery/STM32F4-Discovery_FW_V1.1.0//Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_tim.c:2437
#1  0x08043cc8 in MicroSecondDelay::microsecond_delay (value=10) at microsecond_delay.cpp:54
#2  0x080416ae in CharLCD_STM32F::ready_wait (this=0x200014f8) at CharLCD_STM32F.cpp:658
#3  0x08041642 in CharLCD_STM32F::write8bits (this=0x200014f8, value=48 '0') at CharLCD_STM32F.cpp:599
#4  0x080414ac in CharLCD_STM32F::send (this=0x200014f8, value=48 '0', mode=0 '\000') at CharLCD_STM32F.cpp:487
#5  0x08041434 in CharLCD_STM32F::command(unsigned char) [clone .local.2411] (this=0x200014f8, value=48 '0')
    at CharLCD_STM32F.cpp:455
#6  0x08041138 in CharLCD_STM32F::begin (this=0x200014f8, cols=16 '\020', lines=1 '\001', dotsize=0 '\000')
    at CharLCD_STM32F.cpp:243
#7  0x08040f2c in CharLCD_STM32F::__base_ctor (this=0x200014f8, pins=..., ports=...) at CharLCD_STM32F.cpp:118
#8  0x0804246a in main () at main.cpp:118
telescope->set_align_error_max( static_cast<float>(CAACoordinateTransformation::RadiansToDegrees(max_error) ) );
  //telescope->set_align_error_mean( static_cast<float>(CAACoordinateTransformation::RadiansToDegrees( sum_error/count ) ) );***************************/
/* This works, but, if the Display (or the wiring) is broken, 
 * The whole System stops here.  Need a safety valve.
 * 
 * The incident that inspired this comment was a loose wire 
 * on the display R/W pin.
*/ 
static uint32_t debug_ready_wait;
void CharLCD_STM32F::ready_wait(){
  debug_ready_wait = 0;
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  /* NOPULL works but is slow. 
   * DOWN works.
   * UP leaves the screen blank.
   */
  //GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Pin = _data_pins[7];
  GPIO_Init(_data_ports[7], &GPIO_InitStructure);
  uint8_t busy = 1;
  rs_on();
  rw_on();
  do{
    ++debug_ready_wait;
    microsecond_delay(10);
    busy = GPIO_ReadInputDataBit(_data_ports[7],_data_pins[7]);
    assert_param(  _rw_pin != 255  );
  }while( busy );
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(_data_ports[7], &GPIO_InitStructure);
  rw_off();
}
