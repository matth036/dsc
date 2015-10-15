#include <iostream>
#include "AADate.h"

/*
 * This is an abandoned test that I may return to.
 *
 * Idea: Use the methods of celestial navigation to model a telescope with
 * only an altitude encoder.
 */

using std::cout;
using std::endl;

/* 
 * Julian date from command line input arguments year,month,day.
 *
 */
int main( int argc, char **argv){
  int year = 2000;
  int month = 1;
  double day = 1;
  if( argc == 4 ){
    sscanf( argv[1], "%d", &year );
    sscanf( argv[2], "%d", &month );
    sscanf( argv[3], "%lf", &day );
  }else{
    printf( "usage: %s YYYY MM DD\n", argv[0] );
    return 1;
  }

  bool gregorian; 
  CAADate test_date{year,month,day,gregorian=true};

  // cout << test_date.Julian() << endl;
  printf( "%18.8f\n", test_date.Julian() );

  cout << "This is an abandoned test that I may return to. -Dan" << endl;
  
  return 0;
}
