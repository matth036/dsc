#include <iostream>
#include "AADate.h"

using std::cout;
using std::endl;

/* 
 * Julian date from command line input arguments year,month,day.
 * @TODO Make a version that gets the current time and 
 * expresses the julian date
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
    printf( "usage: %s YYYY MM DD.ddd\n", argv[0] );
    return 1;
  }

  bool gregorian; 
  CAADate test_date{year,month,day,gregorian=true};

  // cout << test_date.Julian() << endl;
  printf( "%18.8f\n", test_date.Julian() );

  return 0;
}
