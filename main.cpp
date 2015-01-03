#include "main.h"
#include "stm32_e407_utilities.h"
#include "microsecond_delay.h"
#include "timebase_32768Hz.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_rtc.h"
#include "char_lcd_stm32f4.h"
#include "sexagesimal.h"
#include "rtc_management.h"
#include "matrix_keypad.h"
#include "quadrature_decoder.h"
#include "telescope_model.h"

#include "controller.h"
#include "flex_lexer_declare.h"
#include <memory>

///////////////////////////////////////

__IO uint32_t Secondfraction = 0;
__IO uint32_t TimeDisplay = 0;

////////////////////////////////////////


void Delay(uint32_t nTime);
uint32_t loop_counter = 0;
using std::unique_ptr;


static std::unique_ptr < Matrix_Keypad > main_keypad_ptr;

/* Called periodically by the interupt for keypad scanning. */
void main_keypad_time_tick_action()
{
  if (main_keypad_ptr) {
    main_keypad_ptr->time_tick_action();
  }
}

/* 
 * This project uses a unique_ptr for access control to the main LCD.
 * Objects needing the main display should check it out when needed
 * and check it in when no longer needed.  Failure to check it in
 * renders the LCD unuseable.
 *
 * Presently the main LCD is the only one.
 **/
static std::unique_ptr < CharLCD_STM32F > main_lcd_ptr;

std::unique_ptr < CharLCD_STM32F > check_out_main_lcd()
{
  return std::move(main_lcd_ptr);
}

void check_in_main_lcd(std::unique_ptr < CharLCD_STM32F > &&returned_ptr)
{
  main_lcd_ptr = std::move(returned_ptr);
}

static Alignment_Data_Set* main_sight_data = nullptr;

Alignment_Data_Set* get_main_sight_data(){
  return main_sight_data;
}

/* Needs to be set in main(). */
static Simple_Altazimuth_Scope* main_simple_telescope = nullptr; 

Simple_Altazimuth_Scope* get_main_simple_telescope(){
  return main_simple_telescope;
}


int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
   *  this is done through SystemInit() function which is called from startup
   *  file (startup_stm32f4xx.s) before to branch to application main.
   *  To reconfigure the default setting of SystemInit() function, refer to
   *  system_stm32f4xx.c file
   */
  //  STM32_E407_led_setup();
  // timebase_5::timebase_5_configure();


  MicroSecondDelay::microsecond_delay_configure();
  /* This mysterious command turns on the hardware floating point unit. */
  SCB->CPACR |= ((3UL << 10 * 2) | (3UL << 11 * 2));
  main_keypad_ptr = unique_ptr < Matrix_Keypad > (new Matrix_Keypad {
						  {
						  GPIO_Pin_1, GPIO_Pin_3,
						  GPIO_Pin_5, GPIO_Pin_7}, {
						  GPIOF}, {
						  GPIO_Pin_0, GPIO_Pin_2,
						  GPIO_Pin_4, GPIO_Pin_6}, {
						  GPIOF}}
  );

  config_keypad_timer();

  /* RS, R/W, E, DB0, DB1, DB2, DB3, DB4, DB5, DB6, DB7 */
  main_lcd_ptr = unique_ptr < CharLCD_STM32F > ( new CharLCD_STM32F { {
	GPIO_Pin_0,  /* RS  */
	  GPIO_Pin_2, /* R/W */
	  GPIO_Pin_4, /* E */
	  GPIO_Pin_6, /* DB0 */
	  GPIO_Pin_8, /* DB1 */
	  GPIO_Pin_10, /* DB2 */
	  GPIO_Pin_14, /* DB3 */
	  GPIO_Pin_7,  /* DB4 */
	  GPIO_Pin_9,  /* DB5 */
	  GPIO_Pin_11, /* DB6 */
	  GPIO_Pin_15, /* DB7 */
	  }, 
	{GPIOD,
	    GPIOD,
	    GPIOD,
	    GPIOD,
	    GPIOD,
	    GPIOD,
	    GPIOD,
	    GPIOD,
	    GPIOD,
	    GPIOD,
	    GPIOD,
	    }}
  );

  main_lcd_ptr->begin(4, 20);

  unique_ptr < CharLCD_STM32F > lcd = check_out_main_lcd();
  assert_param(lcd != nullptr);
  /* Don't know why, but turning the display off then on 
   * fixes a shuffling of lines 0123 -> 1032   */
  lcd->noDisplay();
  MicroSecondDelay::millisecond_delay(1);
  lcd->display();
  /* Shaft decoders, hardware on telescope mount. */

 Quadrature_Decoder decoder_0{GPIOD,GPIO_Pin_13,
      GPIOD,GPIO_Pin_12,
      AZIMUTH_DECODER_TIMER_TO_USE,
      4000};

  Quadrature_Decoder decoder_1{GPIOE,GPIO_Pin_9,
      GPIOE,GPIO_Pin_11,
      ALTITUDE_DECODER_TIMER_TO_USE,
      4000};

  decoder_0.set_count( 0 );   // Azimuth
  decoder_1.set_count( 500 ); // Altitude

  main_simple_telescope = new Simple_Altazimuth_Scope{ std::unique_ptr<Quadrature_Decoder>(&decoder_0), 
      std::unique_ptr<Quadrature_Decoder>(&decoder_1) };

  main_sight_data = new Alignment_Data_Set( main_simple_telescope );
  /* Set up read backup register RTC_BKP_DR0,
   * Configure RTC if it is not the value set
   * on the first run.
   */
  if (RTC_ReadBackupRegister(RTC_BKP_DR0) != FIRST_DATA) {
    lcd->setCursor(0, 0);
    lcd->print("FIRST_DATA not read.");
    /* RTC Configuration */
    RTC_Cold_Config();
    lcd->setCursor(0, 1);
    lcd->print(" RTC_Cold_Config();");
  } else {
    lcd->setCursor(0, 0);
    lcd->print("FIRST_DATA read");
    RTC_Enable_Init();
    lcd->setCursor(0, 1);
    lcd->print(" RTC_Enable_Init(); ");
  }
  MicroSecondDelay::millisecond_delay(3000);
  /////////////////////////////////////////////////
  lcd->home();
  lcd->clear();
  lcd->setCursor(0, 0);
  lcd->print("DR0: ");
  lcd->print(RTC_ReadBackupRegister(RTC_BKP_DR0), 16);
  MicroSecondDelay::millisecond_delay(3000);
  lcd->setCursor(0, 1);
  lcd->print("DR1: ");
  lcd->print(RTC_ReadBackupRegister(RTC_BKP_DR1), 16);
  lcd->setCursor(0, 2);
  lcd->print("DR2: ");
  lcd->print(RTC_ReadBackupRegister(RTC_BKP_DR2), 16);
  lcd->setCursor(0, 3);
  lcd->print("DR3: ");
  lcd->print(RTC_ReadBackupRegister(RTC_BKP_DR3), 16);
  lcd->setCursor(0, 0);
  lcd->print("DR0: ");
  lcd->print(RTC_ReadBackupRegister(RTC_BKP_DR0), 16);
  MicroSecondDelay::millisecond_delay(4000);
  loop_counter = 0;
  lcd->clear();
  lcd->setCursor(0, 0);
  while (1) {
    ++loop_counter;
    int n; /* Counts the characters printed to lcd. */
    if (dsc_controller::cmd_buffer.size() > 0) {
      /* If there is a command to execute do so. */
      std::string command = dsc_controller::pop_cmd_buffer();
      lcd->clear();
      //lcd->setCursor(0,0);      
      lcd->home();
      MicroSecondDelay::millisecond_delay(1);
      n = lcd->print(" COMMAND: ");
      if (command == "") {
	command == "null";
      }
      n += lcd->print(command);
      while (n < 20) {
	n += lcd->print(' ');
      }
      MicroSecondDelay::millisecond_delay(3000);
      lcd->clear();
      /* Relinquish the main LCD along with the flow of control. */
      check_in_main_lcd(std::move(lcd));
      
      /* New flex_lexer.cpp */ 
      scan_for_action(  command );


      dsc_controller::do_execute_command(command);

      lcd = check_out_main_lcd();
    }
    /* This is what the main loop does if there is no command to execute. 
     * Display the time and the two encoder values.
     */
    lcd->home();
    RTC_TimeTypeDef RTC_TimeStructure;
    Secondfraction =
	1000 -
	((uint32_t) ((uint32_t) RTC_GetSubSecond() * 1000) / (uint32_t) 0x3FF);
    RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
    sexagesimal::Sexagesimal rtc_time {
    RTC_TimeStructure.RTC_Hours,
	  RTC_TimeStructure.RTC_Minutes,
	  RTC_TimeStructure.RTC_Seconds,
	  static_cast < uint16_t > (Secondfraction)};
    n = lcd->print(rtc_time.to_string());
    while (n < 20) {
      n += lcd->print(' ');
    }

    n = lcd->print(dsc_controller::char_buffer);
    while (n < 20) {
      n += lcd->print(' ');
    }

    Alt_Azi_Snapshot_t data = main_simple_telescope->get_snapshot();
    lcd->setCursor( 0, 1);
    n = 0;
    n += lcd->print( data.alt_value );
    while (n < 10) {  // TODO USE 
      n += lcd->print(' ');
    }
    n += lcd->print( data.azi_value );
    while (n < 20) {
      n += lcd->print(' ');
    }
    
    sexagesimal::Sexagesimal longitude = get_backup_domain_longitude();
    sexagesimal::Sexagesimal latitude = get_backup_domain_latitude();
    n=0;
    n += lcd->print( longitude.to_longitude_string() ); 
    while (n < 10) {
      n += lcd->print(' ');
    }
    n += lcd->print( latitude.to_latitude_string() ); 
    while (n < 20) {
      n += lcd->print(' ');
    }


  }
}

/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in 10 ms.
  * @retval None
  */
void Delay(uint32_t nTime)
{
  while (nTime--) ;
}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t * file, uint32_t line)
{
  /* Loop forever blinking the LED in a desperate plea for attention. */
  while (1) {
    uint32_t i;
    for (i = 0; i < 300000; ++i) {
      STM32_E407_led_on();
    }
    for (i = 0; i < 300000; ++i) {
      STM32_E407_led_off();
    }
  }
}

#endif				/* USE_FULL_ASSERT */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
