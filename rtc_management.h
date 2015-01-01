#ifndef _RTC_MANAGEMENT_H
#define _RTC_MANAGEMENT_H

/*
 *   Expecting to have only one RTC, we will
 *   not make a class for this task. Just use 
 *   use regular non-member funtions.
 *  
 */
#include <inttypes.h>

#include <sexagesimal.h>

#define WAKEUP_BUTTON_EXTI_LINE          EXTI_Line0
#define TAMPER_BUTTON_EXTI_LINE          EXTI_Line13
#define KEY_BUTTON_EXTI_LINE             EXTI_Line15

/* Cant use constepr in regular c. */
const uint32_t FIRST_DATA = 0xD9A2D6E6;

/* failed de-macroization.  Why? */
// const uint32_t RTC_BKP_DR_NUMBER = 20;
// static constexpr uint8_t LCD_CLEARDISPLAY = 0x01;

#define RTC_BKP_DR_NUMBER   20

#ifdef __cplusplus
extern "C" {
#endif
/* Functions usable in either C of C++. */
  void RTC_Enable_Init(void);
  void RTC_Cold_Config(void);
  void RTC_Config(void);
  void initial_write_to_backup_reg(void);

  double JD_timestamp_pretty_good_000();
  double JD_timestamp(void);
  /* @TODO sanitize this. */
  // double JD_timestamp_truly_fucked_hack(void);
  void RTC_add_seconds(double);
  void RTC_add_days(double);
  void RTC_add_years(double);


  void save_backup_domain_altitude_tick_per_rev(int alt_ticks);
  void save_backup_domain_azimuth_ticks_per_rev(int azi_ticks);
  int get_backup_domain_altitude_ticks_per_rev();
  int get_backup_domain_azimuth_ticks_per_rev();

#ifdef __cplusplus
}
#endif
/* Functions using classes that are unsuitable for C. */
#ifdef __cplusplus

sexagesimal::Sexagesimal get_backup_domain_longitude();
sexagesimal::Sexagesimal get_backup_domain_latitude();
void save_backup_domain_longitude(sexagesimal::Sexagesimal longitude);
void save_backup_domain_latitude(sexagesimal::Sexagesimal latitude);
#endif

#endif				/*  RTC_MANAGMENT_H */
