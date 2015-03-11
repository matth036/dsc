#include "build_date.h"

#include <stdio.h>
#include <string.h>
// #include <iostream>

int build_month_num( const char* Month ){
  int m_num = 1;  /* Sane default */
  if( 0 == strncmp( Month, "Jan",3 ) ){
      m_num = 1;
    }
    else if( 0 == strncmp( Month, "Feb",3 ) ){
      m_num = 2;
    } 
    else if( 0 ==  strncmp( Month, "Mar",3 ) ){
      m_num = 3;
    } 
    else if( 0 == strncmp( Month, "Apr",3 ) ){
      m_num = 4;
    } 
    else if( 0 == strncmp( Month, "May",3 ) ){
      m_num = 5;
    } 
    else if( 0 == strncmp( Month, "Jun",3 ) ){
      m_num = 6;
    } 
    else if( 0 == strncmp( Month, "Jul",3 ) ){
      m_num = 7;
    } 
    else if( 0 == strncmp( Month, "Aug",3 ) ){
      m_num = 8;
    } 
    else if( 0 == strncmp( Month, "Sep",3 ) ){
      m_num = 9;
    } 
    else if( 0 == strncmp( Month, "Oct",3 ) ){
      m_num = 10;
    } 
    else if( 0 == strncmp( Month, "Nov",3 ) ){
      m_num = 11;
    } 
    else if( 0 == strncmp( Month, "Dec",3 ) ){
      m_num = 12;
    } 
  return m_num;
}

int build_hour(){
  int h; 
  sscanf(  __TIME__, "%d:%*d:%*d", &h );
  return h;
}

int build_minute(){
  int m; 
  sscanf(  __TIME__, "%*d:%d:%*d", &m );
  return m;
}

int build_second(){
  int s; 
  sscanf(  __TIME__, "%*d:%*d:%d", &s );
  return s;
}

int build_month(){
  char monthname[20];
  sscanf( __DATE__, "%s %*d %*d", monthname );
  return build_month_num( monthname );
}

int build_day(){
  int day;
  sscanf( __DATE__, "%*s %d %*d", &day );
  return day;
}

int build_year(){
  int year;
  sscanf( __DATE__, "%*s %*d %d", &year );
  return year;
}

