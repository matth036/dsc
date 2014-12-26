#include <iostream>
#include "AADate.h"
#include "AANutation.h"

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
    printf( "# usage: %s YYYY MM DD\n", argv[0] );
    return 1;
  }

  bool gregorian; 
  CAADate test_date{year,month,day,gregorian=true};
  double JD = test_date.Julian();
  // cout << test_date.Julian() << endl;
  printf( "# Julian Date = %18.8f\n", JD );
  double nil, nio, toe;
  nil = CAANutation::NutationInLongitude(JD); /* arc seconds. */
  nio = CAANutation::NutationInObliquity(JD); /*  arc seconds */
  toe = CAANutation::TrueObliquityOfEcliptic(JD);     /*  Degrees */ 

  printf( "#       nil = %18.8f  arc seconds\n", nil );
  printf( "#       nio = %18.8f  arc seconds\n", nio );
  printf( "#       toe = %18.8f  degrees\n", toe );
#if 0
  for( int i=0; i<365*50; ++i){
    nil = CAANutation::NutationInLongitude(JD + i); /* arc seconds. */
    nio = CAANutation::NutationInObliquity(JD + i); /*  arc seconds */
    printf( "%18.8f  %18.8f\n", nil,nio );
  }
#endif
  return 0;
}
