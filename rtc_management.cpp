#include "rtc_management.h"
#include "specificities.h"
#include "stm32f4xx_rtc.h"
#include "build_date.h"
#include "AADate.h"


__IO uint32_t AsynchPrediv = 0;
__IO uint32_t SynchPrediv = 0;

uint32_t errorindex = 0;
uint32_t i = 0;

/* Eliminate? */
uint32_t BKPDataReg[RTC_BKP_DR_NUMBER] = {
  RTC_BKP_DR0, RTC_BKP_DR1, RTC_BKP_DR2,
  RTC_BKP_DR3, RTC_BKP_DR4, RTC_BKP_DR5,
  RTC_BKP_DR6, RTC_BKP_DR7, RTC_BKP_DR8,
  RTC_BKP_DR9, RTC_BKP_DR10, RTC_BKP_DR11,
  RTC_BKP_DR12, RTC_BKP_DR13, RTC_BKP_DR14,
  RTC_BKP_DR15, RTC_BKP_DR16, RTC_BKP_DR17,
  RTC_BKP_DR18, RTC_BKP_DR19
};

/* Jan 31 2014 made these static */
static RTC_TimeTypeDef RTC_TimeStructure;
static RTC_DateTypeDef RTC_DateStructure;
static RTC_InitTypeDef RTC_InitStructure;

/* In the Stopwatch example this code was in the main() body. */
void RTC_Enable_Init(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;

  /* Enable the PWR APB1 Clock Interface */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);
  /* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  /* Enable the RTC Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  /* EXTI configuration   */
  EXTI_ClearITPendingBit(EXTI_Line22);
  EXTI_InitStructure.EXTI_Line = EXTI_Line22;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  /* BKPDataReg[1] will maintain a count of startups since RTC_Cold_Config() */
  uint32_t restart_count = RTC_ReadBackupRegister(BKPDataReg[1]);
  RTC_WriteBackupRegister(BKPDataReg[1], restart_count + 1);
}

/**
 * @brief  This function handles RTC Wakeup Timer Handler.
 * @param  None
 * @retval None
 */
void RTC_WKUP_IRQHandler(void)
{
  if (RTC_GetITStatus(RTC_IT_WUT) != RESET) {
    RTC_ClearITPendingBit(RTC_IT_WUT);
    EXTI_ClearITPendingBit(EXTI_Line22);
  }
}

/*
 * This sets up the RTC (Real Time Clock) if it has not been set
 * up in previous operation of the board or if the backup battery
 * has been disconnected.
 *
 * The build time and date are used to set the clock.  These are recorded
 * and retrieved in the code found in build_date.c
 *
 * Code pertinant to wake up and time stamp is not really used.  
 * Much of the RTC code is copied from an ST stopwatch example.
 *
 **/
void RTC_Cold_Config(void)
{
  /* Enable the PWR clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
  /* Allow access to RTC */
  PWR_BackupAccessCmd(ENABLE);
  /* Enable the LSE OSC */
  RCC_LSEConfig(RCC_LSE_ON);
  /* Wait till LSE is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET) {

  }
  /* Select the RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
  /* These are the values from the Stopwatch Example. */
  SynchPrediv = 0x3FF;
  AsynchPrediv = 0x1F;

  /* Enable the RTC Clock */
  RCC_RTCCLKCmd(ENABLE);

  /* Wait for RTC APB registers synchronisation */
  RTC_WaitForSynchro();

  /* 
   * Enable The TimeStamp.  This project never used this feature.
   *  Pins PC13 and PI18 can evidently be used to trigger a timestamp.
   *  See stm32f4xx_rtc.h L608,L609. 
   *  Consider taking this out so I do not inadvertently start triggering
   *  timestamps.
   */
  RTC_TimeStampCmd(RTC_TimeStampEdge_Falling, ENABLE);

  /* Write to the first RTC Backup Data Register */
  RTC_WriteBackupRegister(RTC_BKP_DR0, FIRST_DATA);
  /* As long as we have the Astronomical Algoritms date class, use it. */
  /* Central Standard Time is GMT - 6
   * Central Daylight Savings Time is GMT - 5
   */
  float GMT_offset_4_timzone = -5.0;
  CAADate date(build_year(), build_month(), build_day(), build_hour() - GMT_offset_4_timzone,
	       build_minute(), build_second(), true);

  /* Set the Time */
  RTC_TimeStructure.RTC_Hours = date.Hour();
  RTC_TimeStructure.RTC_Minutes = date.Minute();
  RTC_TimeStructure.RTC_Seconds = date.Second();

  /* Set the Date */
  RTC_DateStructure.RTC_Month = date.Month();
  RTC_DateStructure.RTC_Date = date.Day();
  RTC_DateStructure.RTC_Year = date.Year() % 100;
  /*
   * Reconcile these two day of week numbering conventions:
   * Sunday == 0, Monday == 1, ...  in AADate.h (enumeration).
   * Sunday == 7, Monday == 1, ...  in stm32f4xx_rtc.h (#define).
   */
  uint8_t day_of_week = date.DayOfWeek();
  if (day_of_week == 0) {
    day_of_week = 7;
  }
  RTC_DateStructure.RTC_WeekDay = day_of_week;

  /* Calendar Configuration */
  RTC_InitStructure.RTC_AsynchPrediv = AsynchPrediv;
  RTC_InitStructure.RTC_SynchPrediv = SynchPrediv;
  RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
  RTC_Init(&RTC_InitStructure);

  /* Set Current Time and Date */
  RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure);
  RTC_SetDate(RTC_Format_BIN, &RTC_DateStructure);

  /* Configure the RTC Wakeup Clock source and Counter (Wakeup event each 1 second) */
  RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div16);
  RTC_SetWakeUpCounter(0x7FF);

  /* Enable the Wakeup Interrupt */
  RTC_ITConfig(RTC_IT_WUT, ENABLE);

  /* Enable Wakeup Counter */
  RTC_WakeUpCmd(ENABLE);

  /*  Backup SRAM ************************************************************** */
  /* Enable BKPRAM Clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_BKPSRAM, ENABLE);
  /* Enable the Backup SRAM low power Regulator to retain it's content in VBAT mode */
  PWR_BackupRegulatorCmd(ENABLE);
  /* Wait until the Backup SRAM low power Regulator is ready */
  while (PWR_GetFlagStatus(PWR_FLAG_BRR) == RESET) {
  }
  /* RTC Backup Data Registers ************************************************* */
  initial_write_to_backup_reg();
}

/**
 * @brief  Writes data to all Backup data registers.
 * @param  FirstBackupData: data to write to first backup data register.
 * @retval None
 */
void initial_write_to_backup_reg()
{
  uint32_t index = 0;
  // First, zero everything.
  for (index = 0; index < RTC_BKP_DR_NUMBER; index++) {
    RTC_WriteBackupRegister(BKPDataReg[index], 0x0);
  }
  /* 
   * Intentionally not setting ticks_per_revolution.
   *
   * Let main() 
   * get_backup_domain_altitude_ticks_per_rev()
   * and if the value is zero, pop up a menu to prompt for the value.
   * The menu should have default value = specificities::altitude_ticks_per_revolution.
   * then
   * save_backup_domain_altitude_ticks_per_rev( );
   * then construct the encoder reader object.
   *
   */
}

/* Sample code from Timestamp example. */
void RTC_Timestamp_Config(void)
{
  /* Enable the PWR clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
  /* Allow access to RTC */
  PWR_BackupAccessCmd(ENABLE);
  /* Enable the LSE OSC */
  RCC_LSEConfig(RCC_LSE_ON);
  /* Wait till LSE is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET) {
    /* Just waiting... */
  }
  /* Select the RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
  /* Enable the RTC Clock */
  RCC_RTCCLKCmd(ENABLE);
  /* Wait for RTC APB registers synchronisation */
  RTC_WaitForSynchro();
  /* Enable The TimeStamp */
  RTC_TimeStampCmd(RTC_TimeStampEdge_Falling, ENABLE);
}

/* This provides a fast enough value with subsecond accuaracy.
 * Suitable for rapidly updated output views. 
 * Need to verify that successive calls are non-decreasing. 
 * 
 * Timestamp functions for the project need more research and experimentation.
 * */
double JD_timestamp_pretty_good_000(){
  uint32_t year_100s = 100 * (static_cast < uint32_t > (build_year()) / 100);

  uint32_t Secondfraction = RTC_GetSubSecond();
  RTC_TimeTypeDef RTC_TimeStructure;
  RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
  RTC_DateTypeDef RTC_TimeStampDateStructure;
  RTC_GetDate(RTC_Format_BIN, &RTC_TimeStampDateStructure );

  Secondfraction =
    1000 -
    ((uint32_t) ((uint32_t) Secondfraction * 1000) / (uint32_t) 0x3FF);

  uint32_t year = year_100s + RTC_TimeStampDateStructure.RTC_Year;
  double seconds = Secondfraction;
  seconds *= .001;
  seconds += RTC_TimeStructure.RTC_Seconds;

  CAADate timestamp_date(year, RTC_TimeStampDateStructure.RTC_Month,
			 RTC_TimeStampDateStructure.RTC_Date,
			 RTC_TimeStructure.RTC_Hours,
			 RTC_TimeStructure.RTC_Minutes,
			 seconds, true);

  double JD = timestamp_date.Julian();
  return JD;
}


double JD_timestamp_old_busted(void)
{
  RTC_TimeTypeDef RTC_TimeStampStructure;
  RTC_DateTypeDef RTC_TimeStampDateStructure;
  /* The funtion RTC_GetTimeStamp(...) Always sets RTC_Year to zero. */
  RTC_DateTypeDef RTC_DateStruct;
  RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);
  uint32_t year = RTC_DateStruct.RTC_Year;
  /* The RTC doesn't count centuries.  This fixes that. IS THIS BUGGY ON NEW YEARS EVE? */
  year += 100 * (static_cast < uint32_t > (build_year()) / 100);

  RTC_TimeStampCmd(RTC_TimeStampEdge_Falling, ENABLE);

  EXTI_GenerateSWInterrupt(KEY_BUTTON_EXTI_LINE);
  EXTI_GenerateSWInterrupt(TAMPER_BUTTON_EXTI_LINE);
  EXTI_GenerateSWInterrupt(WAKEUP_BUTTON_EXTI_LINE);

  RTC_GetTimeStamp(RTC_Format_BIN, &RTC_TimeStampStructure,
		   &RTC_TimeStampDateStructure);
  CAADate timestamp_date(year, RTC_TimeStampDateStructure.RTC_Month,
			 RTC_TimeStampDateStructure.RTC_Date,
			 RTC_TimeStampStructure.RTC_Hours,
			 RTC_TimeStampStructure.RTC_Minutes,
			 RTC_TimeStampStructure.RTC_Seconds, true);

  //return (double)RTC_TimeStampDateStructure.RTC_Year;
  //return (double)RTC_TimeStampDateStructure.RTC_Month;
  //return (double)RTC_TimeStampStructure.RTC_Hours;
  return timestamp_date.Julian();
}

void RTC_add_seconds( double dt ){
  /* Enable the PWR clock. Should already be Enabled. */
  // RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
  /* Allow access to RTC.  Should already be Enabled. */
  // PWR_BackupAccessCmd(ENABLE);
  dt /= 86400.0;
  double JD = JD_timestamp_pretty_good_000();
  JD += dt;
  CAADate date{JD,true};
  /* Set the Time */
  RTC_TimeStructure.RTC_Hours = date.Hour();
  RTC_TimeStructure.RTC_Minutes = date.Minute();
  RTC_TimeStructure.RTC_Seconds = date.Second();
  /* Set the Date */
  RTC_DateStructure.RTC_Month = date.Month();
  RTC_DateStructure.RTC_Date = date.Day();
  RTC_DateStructure.RTC_Year = date.Year() % 100;
  /*
   * Reconcile these two day of week numbering conventions:
   * Sunday == 0, Monday == 1, ...  in AADate.h (enumeration).
   * Sunday == 7, Monday == 1, ...  in stm32f4xx_rtc.h (#define).
   */
  uint8_t day_of_week = date.DayOfWeek();
  if (day_of_week == 0) {
    day_of_week = 7;
  }
  RTC_DateStructure.RTC_WeekDay = day_of_week;
  /* Set Current Time and Date */
  RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure);
  RTC_SetDate(RTC_Format_BIN, &RTC_DateStructure);
}

void RTC_add_years( double dt ){
  dt /= 365.0;  /* divide by 365.25 might be more logical. */
  double JD = JD_timestamp_pretty_good_000();
  JD += dt;
  CAADate date{JD,true};
  /* Set the Time */
  RTC_TimeStructure.RTC_Hours = date.Hour();
  RTC_TimeStructure.RTC_Minutes = date.Minute();
  RTC_TimeStructure.RTC_Seconds = date.Second();
  /* Set the Date */
  RTC_DateStructure.RTC_Month = date.Month();
  RTC_DateStructure.RTC_Date = date.Day();
  RTC_DateStructure.RTC_Year = date.Year() % 100;
  /*
   * In AADate.h (enumeration).
   * Sunday == 0, Monday == 1, ... but,   
   * in stm32f4xx_rtc.h (#define),
   * Sunday == 7, Monday == 1, ...  
   */
  uint8_t day_of_week = date.DayOfWeek();
  if (day_of_week == 0) {
    day_of_week = 7;
  }
  RTC_DateStructure.RTC_WeekDay = day_of_week;
  /* Set Current Time and Date */
  RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure);
  RTC_SetDate(RTC_Format_BIN, &RTC_DateStructure);
}

void RTC_add_days( double dt ){
  double JD = JD_timestamp_pretty_good_000();
  JD += dt;
  CAADate date{JD,true};
  /* Set the Time */
  RTC_TimeStructure.RTC_Hours = date.Hour();
  RTC_TimeStructure.RTC_Minutes = date.Minute();
  RTC_TimeStructure.RTC_Seconds = date.Second();
  /* Set the Date */
  RTC_DateStructure.RTC_Month = date.Month();
  RTC_DateStructure.RTC_Date = date.Day();
  RTC_DateStructure.RTC_Year = date.Year() % 100;
  /*
   * In AADate.h (enumeration).
   * Sunday == 0, Monday == 1, ... but,   
   * in stm32f4xx_rtc.h (#define),
   * Sunday == 7, Monday == 1, ...  
   */
  uint8_t day_of_week = date.DayOfWeek();
  if (day_of_week == 0) {
    day_of_week = 7;
  }
  RTC_DateStructure.RTC_WeekDay = day_of_week;
  /* Set Current Time and Date */
  RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure);
  RTC_SetDate(RTC_Format_BIN, &RTC_DateStructure);
}

void rtc_subsecond_experiment(){
  /*********************
  Running this code (in gdb) verifies that the subsecond timer counts down, not up 
  (gdb) print {forward , tie , backward }
  $7 = {0, 9997, 3}
  (gdb) continue
   *********************/
  uint32_t forward = 0;
  uint32_t tie = 0;
  uint32_t backward = 0;
  for( uint32_t i=0; i<10000; ++i ){
    auto get_0 = RTC_GetSubSecond();
    auto get_1 = RTC_GetSubSecond();
    if( get_1 > get_0 ){
      ++forward;
    }else if( get_1 < get_0 ){
      ++backward;
    }else if( get_0 == get_1 ){
      ++tie;
    }else{
      assert_param( false );
    }
  }
}


/******** Backup domain set funtions. *************/

//  RTC_BKP_DR0 is an arbitrary integer indicating backkup domain has been initialized.
//  RTC_BKP_DR1 counts the number of starts since cold start initialization.
//  RTC_BKP_DR2 longitude
//  RTC_BKP_DR3 latitude
/* Others are planned but not yet used. */
void save_backup_domain_longitude( sexagesimal::Sexagesimal longitude ){
  RTC_WriteBackupRegister(RTC_BKP_DR2, longitude.get_data());
}

void save_backup_domain_latitude( sexagesimal::Sexagesimal latitude ){
  RTC_WriteBackupRegister(RTC_BKP_DR3, latitude.get_data());
}

void save_backup_domain_altitude_ticks_per_rev( int alt_ticks ){
    RTC_WriteBackupRegister(RTC_BKP_DR4, alt_ticks);
}

void save_backup_domain_azimuth_ticks_per_rev( int azi_ticks ){
    RTC_WriteBackupRegister(RTC_BKP_DR5, azi_ticks);
}

void save_backup_domain_altitude_home( int alt_home ){
    RTC_WriteBackupRegister(RTC_BKP_DR6, alt_home);
}

void save_backup_domain_azimuth_home( int azi_home ){
    RTC_WriteBackupRegister(RTC_BKP_DR7, azi_home );
}



/******** Backup domain get funtions. *************/
sexagesimal::Sexagesimal get_backup_domain_longitude(){
  sexagesimal::Sexagesimal longitude;
  longitude.set_binary_data( RTC_ReadBackupRegister(RTC_BKP_DR2) );
  return longitude;
}

sexagesimal::Sexagesimal get_backup_domain_latitude(){
  sexagesimal::Sexagesimal latitude;
  latitude.set_binary_data( RTC_ReadBackupRegister(RTC_BKP_DR3) );
  return latitude;
}

int get_backup_domain_altitude_ticks_per_rev(){
  return  RTC_ReadBackupRegister(RTC_BKP_DR4);
}

int get_backup_domain_azimuth_ticks_per_rev(){
  return  RTC_ReadBackupRegister(RTC_BKP_DR5);
}

int get_backup_domain_altitude_home(){
  return  RTC_ReadBackupRegister(RTC_BKP_DR6);
}

int get_backup_domain_azimuth_home(){
  return  RTC_ReadBackupRegister(RTC_BKP_DR7);
}



/* 12 Registers left. */
