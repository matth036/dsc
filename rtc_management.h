#ifndef RTC_MANAGEMENT_H_
#define RTC_MANAGEMENT_H_

/*
 *   Expecting to have only one RTC, we will
 *   not make a class for this task. Just use 
 *   use regular non-member funtions.
 *  
 */
#include <inttypes.h>
#ifdef __cplusplus
#include <sexagesimal.h>
// class sexagesimal::Sexagesimal;
#endif
// #include <sexagesimal.h>

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
  void RTC_add_seconds(double);
  void RTC_add_days(double);
  void RTC_add_years(double);


  void save_backup_domain_altitude_ticks_per_rev(int alt_ticks);
  void save_backup_domain_azimuth_ticks_per_rev(int azi_ticks);
  int get_backup_domain_altitude_ticks_per_rev();
  int get_backup_domain_azimuth_ticks_per_rev();

  //  float get_backup_domain_longitude();
  //  void save_backup_domain_longitude(float);

  //  float get_backup_domain_altitude();
  //  void save_backup_domain_altitude(float);

  float get_backup_domain_altitude();
  void save_backup_domain_altitude(float);


  /* 
   * Plan: Code as if temperature and pressure
   * were periodicly sensed and recorded with
   * save_backup_domain_temperature();
   * save_backup_domain_pressure();
   *  
   * */
  float get_backup_domain_temperature();
  void save_backup_domain_temperature(float);

  float get_backup_domain_pressure();
  void save_backup_domain_pressure(float);


#ifdef __cplusplus
}
#endif


/* Functions using or returning C++ classes that are unsuitable for C. */
#ifdef __cplusplus
sexagesimal::Sexagesimal get_backup_domain_longitude();
sexagesimal::Sexagesimal get_backup_domain_latitude();
void save_backup_domain_longitude(sexagesimal::Sexagesimal longitude);
void save_backup_domain_latitude(sexagesimal::Sexagesimal latitude);
bool have_backup_domain_longitude();
bool have_backup_domain_latitude();
#endif

#endif  // RTC_MANAGMENT_H_
