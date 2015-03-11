#include <iostream>
#include "AA2DCoordinate.h"
#include "AACoordinateTransformation.h"
#include "AADate.h"
#include "extra_solar_transforms.h"
#include "AANutation.h"
#include "sexagesimal.h"

using std::cout;
using std::endl;


int main( int argc, char **argv){
  cout << "Example 23.b page 156." << endl;
  cout << "Jean Meeus, Astronomical Algorithms" << endl;
  cout << "This example starts with the given values for proper motion adjusted position." << endl;
  cout << "Then the Aberation correction is applied followed by Precession and Nutation corrections." << endl;
  cout << "gcc = " << __GNUC__ << "." << __GNUC_MINOR__ << "." << __GNUC_PATCHLEVEL__ << endl;
  cout << "__cplusplus = " << __cplusplus << endl;
  double a_hh = 2.;
  double a_mm = 44.;
  double a_ss = 12.9747;
  double HA = CAACoordinateTransformation::DMSToDegrees(a_hh, a_mm, a_ss,true);
  cout << "Hour Angle = " << HA << endl;
  double d_dd = 49;
  double d_mm = 13;
  double d_ss = 39.896;
  double alpha = 15.0*HA;
  double delta = CAACoordinateTransformation::DMSToDegrees(d_dd, d_mm, d_ss,true);
  printf( "alpha = %18.10f\n", alpha );
  printf( "delta = %18.10f\n", delta );
  /* HA means Hour Angle, for this case the same as Right Ascension.*/
  CAA2DCoordinate RA_Dec_J2000;
  RA_Dec_J2000.X = HA;
  RA_Dec_J2000.Y = delta;
  CAADate example_date{ 2028,11,13.19,true };
  double JD = example_date.Julian();
  printf( "   JD = %18.10f\n", JD );

  CAA2DCoordinate RA_Dec_J2000_with_aberration = apply_aberration( RA_Dec_J2000, JD );
  cout << "Aberration Correction:" << endl;

  printf( "Delta alpha = %18.10f\n", RA_Dec_J2000_with_aberration.X*15.0 - RA_Dec_J2000.X*15.0  );
  printf( "Delta delta = %18.10f\n", RA_Dec_J2000_with_aberration.Y - RA_Dec_J2000.Y );

  cout << "Precession and Nutation Correction:" << endl;  
  // CAA2DCoordinate RA_Dec_J2000_ab_nutation_precession = precession_and_nutation_correct( RA_Dec_J2000_with_aberration, JD );
  CAA2DCoordinate RA_Dec_J2000_ab_nutation_precession = precession_and_nutation_correct_from_mean_eqinox( RA_Dec_J2000_with_aberration, JD );

  printf( "alpha = %18.10f\n", RA_Dec_J2000_ab_nutation_precession.X*15.0 );
  printf( "delta = %18.10f\n", RA_Dec_J2000_ab_nutation_precession.Y );

  sexagesimal::Sexagesimal RA_JD( RA_Dec_J2000_ab_nutation_precession.X );
  sexagesimal::Sexagesimal DEC_JD( RA_Dec_J2000_ab_nutation_precession.Y );

  cout << " RA = " <<  RA_JD.to_string() << endl;
  cout << "DEC = " << DEC_JD.to_string() << endl;

  cout << RA_Dec_J2000_ab_nutation_precession.X*15 - 41.5599646 << "   alpha Discrepancy in Degrees"<<  endl;
  cout << RA_Dec_J2000_ab_nutation_precession.Y - 49.3520685 << "    delta Discrepancy in Degrees"<<  endl;

}
