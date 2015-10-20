#ifndef CHARLCD_STM32F_H_
#define CHARLCD_STM32F_H_

#include "stm32f4xx_gpio.h"
#include "stm32f4xx.h"
#include <inttypes.h>
#include "Print.h"
#include <initializer_list>

/**\defgroup char_lcd ''Character Liquid Crystal Display'' */


/**\addtogroup char_lcd 
 *@{
 **/
/** Commands, de-macro-ized from Arduino code. */
static constexpr uint8_t LCD_CLEARDISPLAY = 0x01;
static constexpr uint8_t LCD_RETURNHOME = 0x02;
static constexpr uint8_t LCD_ENTRYMODESET = 0x04;
static constexpr uint8_t LCD_DISPLAYCONTROL = 0x08;
static constexpr uint8_t LCD_CURSORSHIFT = 0x10;
static constexpr uint8_t LCD_FUNCTIONSET = 0x20;
static constexpr uint8_t LCD_SETCGRAMADDR = 0x40;
static constexpr uint8_t LCD_SETDDRAMADDR = 0x80;

// flags for display entry mode
static constexpr uint8_t LCD_ENTRYRIGHT = 0x00;
static constexpr uint8_t LCD_ENTRYLEFT = 0x02;
static constexpr uint8_t LCD_ENTRYSHIFTINCREMENT = 0x01;
static constexpr uint8_t LCD_ENTRYSHIFTDECREMENT = 0x00;

// flags for display on/off control
static constexpr uint8_t LCD_DISPLAYON = 0x04;
static constexpr uint8_t LCD_DISPLAYOFF = 0x00;
static constexpr uint8_t LCD_CURSORON = 0x02;
static constexpr uint8_t LCD_CURSOROFF = 0x00;
static constexpr uint8_t LCD_BLINKON = 0x01;
static constexpr uint8_t LCD_BLINKOFF = 0x00;

// flags for display/cursor shift
static constexpr uint8_t LCD_DISPLAYMOVE = 0x08;
static constexpr uint8_t LCD_CURSORMOVE = 0x00;
static constexpr uint8_t LCD_MOVERIGHT = 0x04;
static constexpr uint8_t LCD_MOVELEFT = 0x00;

// flags for function set
static constexpr uint8_t LCD_8BITMODE = 0x10;
static constexpr uint8_t LCD_4BITMODE = 0x00;
static constexpr uint8_t LCD_2LINE = 0x08;
static constexpr uint8_t LCD_1LINE = 0x00;
static constexpr uint8_t LCD_5x10DOTS = 0x04;
static constexpr uint8_t LCD_5x8DOTS = 0x00;
/*@}*/

/* Pin numbers are bit masks, e.g. GPIO_Pin_9 == 0x0200 == 1<<9. 
 *  The macro GPIO_Pin_9 is #define-d in stm32f4xx_gpio.h as ((uint16_t)0x0200)
 */

/* Some style guides oppose using directive in .h files. 
 * Note: I have decided this is OK for when using is functionally 
 * only a typedef for a single (native or derived) type.
 */
using pin = uint16_t; 
using port = GPIO_TypeDef*;  // A pointer to a struct. 
using std::initializer_list;

class CharLCD_STM32F : public Print {
public:
  /*! pins.size() determines the configuration.
   *  - (pins.size()) == 6   -->  4 bit mode, rw grounded.
   *  - (pins.size()) == 7   -->  4 bit mode, rw connected to uC.
   *  - (pins.size()) == 10  -->  8 bit mode, rw grounded.
   *  - (pins.size()) == 11  -->  8 bit mode, rw connected to uC.
   * 
   *  If ports.size() > pins.size() The last element of ports[]
   *  is used for the remaining pins.
   */
  CharLCD_STM32F(initializer_list<pin>pins,
		 initializer_list<port>ports);

  void init(uint8_t fourbitmode, uint8_t rs, uint8_t rw, uint8_t enable,
	    uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
	    uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7, GPIO_TypeDef* port );
    
  void init(uint8_t fourbitmode, 
	    uint16_t rs, uint16_t rw, uint16_t enable,
	    uint16_t d0, uint16_t d1, uint16_t d2, uint16_t d3,
	    uint16_t d4, uint16_t d5, uint16_t d6, uint16_t d7, 
	    GPIO_TypeDef* port_rs, GPIO_TypeDef* port_rw, GPIO_TypeDef* port_enable,
	    GPIO_TypeDef* port_d0,GPIO_TypeDef* port_d1,GPIO_TypeDef* port_d2,GPIO_TypeDef* port_d3,
	    GPIO_TypeDef* port_d4,GPIO_TypeDef* port_d5,GPIO_TypeDef* port_d6,GPIO_TypeDef* port_d7 );

  void stm32f4_init( uint8_t fourbitmode );
  void gpio_init_comands( );
  void gpio_init_lsb( );
  void gpio_init_msb( );

  void begin(uint8_t cols, uint8_t rows, uint8_t charsize = LCD_5x8DOTS);

  void clear();
  void home();

  void noDisplay();
  void display();
  void noBlink();
  void blink();
  void noCursor();
  void cursor();
  void scrollDisplayLeft();
  void scrollDisplayRight();
  void leftToRight();
  void rightToLeft();
  void autoscroll(); /*!< */
  void noAutoscroll();
  void createChar(uint8_t, uint8_t[]);
  void setCursor(uint8_t, uint8_t); 
  virtual size_t write(uint8_t);
  void command(uint8_t);
  void VARIOUS_HACKS();
  using Print::write;
private:
  void send(uint8_t, uint8_t);
  void write4bits(uint8_t);
  void write8bits(uint8_t);
  uint8_t read8bits();
  void pulseEnable();
  void ready_wait();

  void rs_on();
  void rw_on();
  void enable_on();
  void rs_off();
  void rw_off();
  void enable_off();

  /* Typically all pins wil be on the same port.  We allow for the possiblity that they aren't. */
  uint16_t       _rs_pin; // LOW: command.  HIGH: character.
  GPIO_TypeDef* _rs_port;
  uint16_t       _rw_pin; // LOW: write to LCD.  HIGH: read from LCD.
  GPIO_TypeDef* _rw_port;
  uint16_t       _enable_pin; // activated by a HIGH pulse.
  GPIO_TypeDef* _enable_port;
  uint16_t       _data_pins[8];
  GPIO_TypeDef* _data_ports[8];
  /* Check if these values apply to displays with other than four rows. */
  uint8_t row_offsets[4] = { 0x00, 0x40, 0x14, 0x54 };
  uint8_t _displayfunction;
  uint8_t _displaycontrol;
  uint8_t _displaymode;
  uint8_t _initialized;
  uint8_t _numlines;
  uint8_t _currline;
};

#endif  // CHARLCD_STM32F_H_
