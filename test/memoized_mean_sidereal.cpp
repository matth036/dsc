#include <iostream>
#include "AADate.h"
#include "AANutation.h"
#include "horizontal_equatorial.h"
#include "AASidereal.h"

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
  double step = 1.0/1440.0;
  double start = JD;
  while( JD <= start + 14.5 ){
    //CAASidereal::MeanGreenwichSiderealTime
    double best = CAASidereal::MeanGreenwichSiderealTime( JD );
    double test = horizontal_equatorial::memoized_MeanGreenwichSiderealTime( JD );
    double error = test - best;
    if( error >= 23.999 ){
      error -= 24.0;
    }
    cout << JD - start << "  " << error << endl;
    JD += step;
  }
}
