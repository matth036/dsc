#ifndef BUILD_DATE_H
#define BUILD_DATE_H

/* These functions return information about the build time of the application.
 * They provide a reasonably current time value in the case of an un-set real time clock.
 *
 * @TODO try to implement this information as a set of C++11 constepr integers.
 */


#ifdef __cplusplus
extern "C" {
#endif

int build_hour();
int build_minute();
int build_second();
int build_month();
int build_day();
int build_year();

int build_month_num( const char* Month );

#ifdef __cplusplus
}
#endif

#endif
