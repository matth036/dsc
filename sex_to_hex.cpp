#include <iostream>
#include "sexagesimal.h"

/*
 * Treats the four command line arguments as digit fields in
 * DDD:MM:SS.xxx sexagesimal format.  Creates a sexagesimal::Sexagesimal 
 * object as defined in sexagesimal.h.   
 * 
 * The 32 bit binary data of that object is output as 8 hexadecimal digits.
 *
 * This used in a perl script to create ngc_list.h.  The intention is to 
 * assure the format defined sexagesimal.h and sexagesimal.cpp is adheared to.
 *
 */

int main( int argc, char **argv){
  if( argc != 5 ){
    printf( "usage: %s DDD MM SS xxx\n", argv[0] );
    exit(1);
  }
  int dd;
  int hh;
  int mm;
  int xxx;
  sscanf( argv[1], "%d", &dd );
  sscanf( argv[2], "%d", &hh );
  sscanf( argv[3], "%d", &mm );
  sscanf( argv[4], "%d", &xxx );
  /*
   * @TODO 
   * 
   * Do all of this or revert to a tested good but stylisticly bad version.
   * 
   * Use the correct integer types. ( int32_t, uint8_t, uint8_t, uint16_t ). 
   * Look up and use the correct format specifiers for sscanf.
   * 
   * Change the constructor call to curly braces.  
   *
   * Verify that make_ngc_list.pl creates an identical ngc_list.h
   *
   */
  sexagesimal::Sexagesimal value( dd, hh, mm, xxx );
  // printf( "%d %d %d %d\n", dd, hh, mm, xxx );
  printf( "%08X\n", value.get_data() );
}
