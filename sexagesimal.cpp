#include "sexagesimal.h"
#include <cmath>
#define _GLIBCXX_USE_WCHAR_T 1
#include <string>
#include <bits/basic_string.h>
#include "binary_tidbits.h"

/*
 * 32 bits in uint32 used as:
 * 1 bit   sign
 * 9 bits  0-360 (which may only be 0-24)
 * 6 bits  0-60
 * 6 bits  0-60
 * 10 bits 0-999
 * ddd.mm.ss.xxx
 * hh:mm:ss.xxx
 * */

sexagesimal::Sexagesimal::Sexagesimal(int32_t deg, uint8_t min, uint8_t sec,
				      uint16_t xxx)
{
  setData(deg, min, sec, xxx);
}

sexagesimal::Sexagesimal::Sexagesimal(double d)
{
  setData(d);
}

void sexagesimal::Sexagesimal::setData(int32_t deg, uint32_t min, uint32_t sec,
				       uint32_t xxx)
{
  if (deg < 0) {		/* indicate the sign */
    data.ui = 1 << 31;
    deg = -deg;
  } else {
    data.ui = 0;
  }
  deg = deg % 360;
  /* Carry up overflow in less significant data */
  while (xxx >= 1000) {
    xxx -= 1000;
    ++sec;
  }
  while (sec >= 60) {
    sec -= 60;
    ++min;
  }
  while (min >= 60) {
    min -= 60;
    ++deg;
  }
  deg = deg % 360;
  data.ui |= 0x7FC00000 & (static_cast < uint32_t > (deg) << 22);
  data.ui |= 0x003F0000 & (static_cast < uint32_t > (min) << (22 - 6));
  data.ui |= 0x0000FC00 & (static_cast < uint32_t > (sec) << (22 - 6 - 6));
  data.ui |= 0x000003FF & (static_cast < uint32_t > (xxx) << (22 - 6 - 6 - 10));
}

void sexagesimal::Sexagesimal::setData(double d)
{
  bool neg = false;
  if (d < 0) {
    neg = true;
    d = -d;
  }
  int deg = floor(d);
  d = d - deg;
  d *= 60;
  uint8_t min = static_cast < uint8_t > (floor(d));
  d = d - min;
  d *= 60;
  uint8_t sec = static_cast < uint8_t > (floor(d));
  d = d - sec;
  d *= 1000;

  uint16_t millis = static_cast < uint16_t > (round(d));
  if (neg) {
    deg = -deg;
  }
  setData(deg, min, sec, millis);
}

const uint32_t sexagesimal::Sexagesimal::millis()
{
  return data.ui & 0x03FF;
}

const uint32_t sexagesimal::Sexagesimal::seconds()
{
  return ((0x0000FC00 & data.ui) >> 10);
}

const uint32_t sexagesimal::Sexagesimal::minutes()
{
  return ((0x003F0000 & data.ui) >> 16);
}

const uint32_t sexagesimal::Sexagesimal::msb()
{
  return ((0x7FC00000 & data.ui) >> 22);
}

const double sexagesimal::Sexagesimal::to_double()
{
  double value = 0.0;
  value += millis();
  value /= 1000.0;
  value += seconds();
  value /= 60.0;
  value += minutes();
  value /= 60;
  value += msb();
  if (data.ui & 1 << 31) {
    return -value;
  } else {
    return value;
  }
}

const uint32_t sexagesimal::Sexagesimal::get_data()
{
  return data.ui;
}

/* Non member function (but in the namespace.) */
double sexagesimal::data_to_double(uint32_t data)
{
  sexagesimal::Sexagesimal temp;
  temp.set_binary_data(data);
  return temp.to_double();
}

std::string sexagesimal::to_string_hack(int32_t val){
  uint32_t mag;
  if( val>=0 ){
    mag = val;
    return "+" + sexagesimal::to_string_hack(mag); 
  }else{
    mag = -val;
    return "-" + sexagesimal::to_string_hack(mag); 
  }
}

/* 
 * This is the sort of low level stuff I should not need to implement myself. 
 * 
 * The correct call is std::to_string( arg );
 * This works for native compile to build machine archetecture.
 * Doesn't work in my cross compile environment.
 */
std::string sexagesimal::to_string_hack(uint32_t val)
{
#if 0
  bool hack_needed = false;
  std::string lets_try_doing_it_right = "No Sucsess with gcc 4.8.3 and newlib 2.1";
  lets_try_doing_it_right = to_string( val );
  if( !hack_needed ){
    return lets_try_doing_it_right;
  }
  if (val == 0) {
    return "0";
  }
#endif
  std::string str;
  while (val) {
    uint32_t digit = val % 10;
    val /= 10;
    switch (digit) {
    case 0:
      str = '0' + str;
      break;
    case 1:
      str = '1' + str;
      break;
    case 2:
      str = '2' + str;
      break;
    case 3:
      str = '3' + str;
      break;
    case 4:
      str = '4' + str;
      break;
    case 5:
      str = '5' + str;
      break;
    case 6:
      str = '6' + str;
      break;
    case 7:
      str = '7' + str;
      break;
    case 8:
      str = '8' + str;
      break;
    case 9:
      str = '9' + str;
      break;
    default:
      break;
    }
  }
  return str;
}

/* Come up with a better name for this. */
template < typename N >
    sexagesimal::Sexagesimal sexagesimal::toSexagesimal(N realnum)
{
  return sexagesimal::double2sexagesimal(static_cast < double >(realnum));
}

sexagesimal::Sexagesimal sexagesimal::double2sexagesimal(double d)
{
  bool neg = false;
  if (d < 0) {
    neg = true;
    d = -d;
  }
  int deg = floor(d);
  d = d - deg;
  d *= 60;
  uint8_t min = static_cast < uint8_t > (floor(d));
  d = d - min;
  d *= 60;
  uint8_t sec = static_cast < uint8_t > (floor(d));
  d = d - sec;
  d *= 1000;
  uint16_t millis = static_cast < uint16_t > (round(d));
  if (neg) {
    deg = -deg;
  }
  return sexagesimal::Sexagesimal {
  deg, min, sec, millis};
}

/* In refining this and related methods, heed the instruction in 
 * Meeus "Astronomical Algorithms" p.6 
*/
const std::string sexagesimal::Sexagesimal::to_string()
{
  std::string str;
  std::string addon;
  if (data.ui & 1 << 31) {
    str = '-';
  } else {
    str = ' ';
  }
  /* The conditional is a test bed for getting a sane to_string( uint32_t ) */
#if 0
  str += std::to_string( msb() );
#else
  str += sexagesimal::to_string_hack(msb());	//std::to_string( msb() );
#endif
  str += ':';
  addon = sexagesimal::to_string_hack(minutes());	//std::to_string( minutes() );
  while (addon.size() < 2) {	// zero padding
    addon = '0' + addon;
  }
  str += addon;
  str += ':';
  addon = sexagesimal::to_string_hack(seconds());	//std::to_string( seconds() );
  while (addon.size() < 2) {	// zero padding
    addon = '0' + addon;
  }
  str += addon;
  str += '.';
  addon = sexagesimal::to_string_hack(millis());	// std::to_string( millis() );
  while (addon.size() < 3) {	// zero padding
    addon = '0' + addon;
  }
  str += addon;
  return str;
}

/* Usefull for comparison with Nautical Almanac. */
const std::string sexagesimal::Sexagesimal::to_hmxxxx_string()
{
  std::string str;
  std::string addon;
  if (data.ui & 1 << 31) {
    str = '-';
  } else {
    str = ' ';
  }
  str += sexagesimal::to_string_hack(msb());	//std::to_string( msb() );
  str += ':';
  addon = sexagesimal::to_string_hack(minutes());	// std::to_string( minutes() );
  while (addon.size() < 2) {	// zero padding
    addon = '0' + addon;
  }
  str += addon;
  str += '.';
  // finally add on 4 digits of decimal minutes.
  uint32_t milli_seconds = 1000 * seconds() + millis();	// 1 second = 1000 milliseconds.
  uint32_t temp = milli_seconds * 10;	// 10^(-4) seconds
  temp = temp / 60;		// Four digits of decimal minutes.
  addon = sexagesimal::to_string_hack(temp);	//std::to_string( temp );
  while (addon.size() < 4) {	// zero padding
    addon = '0' + addon;
  }
  str += addon;
  return str;
}

/* Usefull for comparison with Nautical Almanac. */
const std::string sexagesimal::Sexagesimal::to_dddmmxxxx_string()
{
  std::string str;
  std::string addon;
  if (data.ui & 1 << 31) {
    str = '-';
  } else {
    str = ' ';
  }
  str += sexagesimal::to_string_hack(msb());	// std::to_string( msb() );
  while (str.size() < 4) {	// space padding
    str = ' ' + str;
  }
  str += ':';
  addon = sexagesimal::to_string_hack(minutes());	// std::to_string( minutes() );
  while (addon.size() < 2) {	// zero padding
    addon = '0' + addon;
  }
  str += addon;
  str += '.';
  // finally add on 4 digits of decimal minutes.
  uint32_t milli_seconds = 1000 * seconds() + millis();	// 1 second = 1000 milliseconds.
  uint32_t temp = milli_seconds * 10;	// 10^(-4) seconds
  temp = temp / 60;		// Four digits of decimal minutes.
  addon = sexagesimal::to_string_hack(temp);	// std::to_string( temp );
  while (addon.size() < 4) {	// zero padding
    addon = '0' + addon;
  }
  str += addon;
  return str;
}

/* Usefull for comparison with Astronimical Almanac. */
const std::string sexagesimal::Sexagesimal::to_dms_string()
{
  std::string str;
  std::string addon;
  if (data.ui & 1 << 31) {
    str = '-';
  } else {
    str = ' ';
  }
  str += sexagesimal::to_string_hack(msb());	// std::to_string( msb() );
  while (str.size() < 4) {	// space padding
    str = ' ' + str;
  }
  str += ':';
  addon = sexagesimal::to_string_hack(minutes());	// std::to_string( minutes() );
  while (addon.size() < 2) {	// zero padding
    addon = '0' + addon;
  }
  str += addon;
  str += ':';
  addon = sexagesimal::to_string_hack(seconds());	// std::to_string( seconds() );
  while (addon.size() < 2) {	// zero padding
    addon = '0' + addon;
  }
  str += addon;
  str += '.';
  addon = sexagesimal::to_string_hack(millis());	// std::to_string( temp );
  while (addon.size() < 3) {	// zero padding
    addon = '0' + addon;
  }
  str += addon;
  return str;
}

/* Usefull for comparison with Astronimical Almanac. */
const std::string sexagesimal::Sexagesimal::to_hms_string()
{
  std::string str;
  std::string addon;
  if (data.ui & 1 << 31) {
    str = '-';
  } else {
    str = ' ';
  }
  str += sexagesimal::to_string_hack(msb());	// std::to_string( msb() );
  while (str.size() < 4) {	// space padding
    str = ' ' + str;
  }
  str += 'h';
  addon = sexagesimal::to_string_hack(minutes());	// std::to_string( minutes() );
  while (addon.size() < 2) {	// zero padding
    addon = '0' + addon;
  }
  str += addon;
  str += 'm';
  addon = sexagesimal::to_string_hack(seconds());	// std::to_string( seconds() );
  while (addon.size() < 2) {	// zero padding
    addon = '0' + addon;
  }
  str += addon;
  str += 's';
  str += '.';
  addon = sexagesimal::to_string_hack(millis());	// std::to_string( temp );
  while (addon.size() < 3) {	// zero padding
    addon = '0' + addon;
  }
  str += addon;
  return str;
}


/* Usefull for comparison with Nautical Almanac. */
const std::string sexagesimal::Sexagesimal::to_latitude_string()
{
  std::string str;
  std::string addon;
  if (data.ui & 1 << 31) {
    str = 'S';
  } else {
    str = 'N';
  }
  str += sexagesimal::to_string_hack(msb());	// std::to_string( msb() );
  while (str.size() < 4) {	// space padding
    str = ' ' + str;
  }
  str += ':';
  addon = sexagesimal::to_string_hack(minutes());	// std::to_string( minutes() );
  while (addon.size() < 2) {	// zero padding
    addon = '0' + addon;
  }
  str += addon;
  str += ':';
  addon = sexagesimal::to_string_hack(seconds());	// std::to_string( minutes() );
  while (addon.size() < 2) {	// zero padding
    addon = '0' + addon;
  }
  str += addon;
  /************************************
  str += '.';
  // finally add on 4 digits of decimal minutes.
  uint32_t milli_seconds = 1000 * seconds() + millis();	// 1 second = 1000 milliseconds.
  uint32_t temp = milli_seconds * 10;	// 10^(-4) seconds
  temp = temp / 60;		// Four digits of decimal minutes.
  addon = sexagesimal::to_string_hack(temp);	// std::to_string( temp );
  while (addon.size() < 4) {	// zero padding
    addon = '0' + addon;
  }
  str += addon;
  ************************************/
  return str;
}



/* Usefull for comparison with Nautical Almanac. */
const std::string sexagesimal::Sexagesimal::to_longitude_string()
{
  std::string str;
  std::string addon;
  if( binary_tidbits::west_longitude_is_positive() ){
    if (data.ui & 1 << 31) {
      str = 'E';
    } else {
      str = 'W';
    }
  }else{
    if (data.ui & 1 << 31) {
      str = 'W';
    } else {
      str = 'E';
    }
  }
  str += sexagesimal::to_string_hack(msb());	// std::to_string( msb() );
  while (str.size() < 4) {	// space padding
    str = ' ' + str;
  }
  str += ':';
  addon = sexagesimal::to_string_hack(minutes());	// std::to_string( minutes() );
  while (addon.size() < 2) {	// zero padding
    addon = '0' + addon;
  }
  str += addon;
  str += ':';
  addon = sexagesimal::to_string_hack(seconds());	// std::to_string( minutes() );
  while (addon.size() < 2) {	// zero padding
    addon = '0' + addon;
  }
  str += addon;
  /************************************
  str += '.';
  // finally add on 4 digits of decimal minutes.
  uint32_t milli_seconds = 1000 * seconds() + millis();	// 1 second = 1000 milliseconds.
  uint32_t temp = milli_seconds * 10;	// 10^(-4) seconds
  temp = temp / 60;		// Four digits of decimal minutes.
  addon = sexagesimal::to_string_hack(temp);	// std::to_string( temp );
  while (addon.size() < 4) {	// zero padding
    addon = '0' + addon;
  }
  str += addon;
  ************************************/
  return str;
}
