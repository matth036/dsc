#ifndef BUILD_DATE_H_
#define BUILD_DATE_H_


/** \defgroup os Operating System
  * 
  *
  * This module provides operating system like funtionality.
  * 
  * These functions return information about the build time 
  * of the application.
  * They provide a reasonably current time value in the case
  * of an un-set real time clock.
  *
  */

#ifdef __cplusplus
extern "C" {
#endif

  /** \ingroup os */
  /*@{*/
  /** Returns the build time hour. */  
int build_hour();
  /** Returns the build time minute. */
int build_minute();
  /** Returns the build time second. */
int build_second();
  /** Returns the build month numerically in range [1..12]. */
int build_month();
  /** Returns the build day of the month */
int build_day();
  /** Returns the build year. */
int build_year();
  /** Returns the month numerically where the C style string argument \
      Month is "Jan","Feb", ... Used by \code int build_month(). */
int build_month_num(const char* Month);
  /*@}*/
#ifdef __cplusplus
}
#endif

#endif  // BUILD_DATE_H_
