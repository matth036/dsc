#include <iostream>
#include "AA2DCoordinate.h"
#include "AACoordinateTransformation.h"
#include "AADate.h"
#include "extra_solar_transforms.h"
#include "AANutation.h"
#include "sexagesimal.h"
#include "solar_system.h"
#include <math.h>
#include "AAMercury.h"
#include "AAVenus.h"
#include "AAMars.h"
#include "AAEarth.h"
#include "AAJupiter.h"
#include "AASaturn.h"
#include "AAUranus.h"
#include "AANeptune.h"
#include "AAPluto.h"


using std::cout;
using std::endl;

CAA3DCoordinate local_minus(CAA3DCoordinate & a, CAA3DCoordinate & b)
{
  CAA3DCoordinate diff;
  diff.X = a.X - b.X;
  diff.Y = a.Y - b.Y;
  diff.Z = a.Z - b.Z;
  return diff;
}


CAA3DCoordinate local_plus(CAA3DCoordinate & a, CAA3DCoordinate & b)
{
  CAA3DCoordinate sum;
  sum.X = a.X + b.X;
  sum.Y = a.Y + b.Y;
  sum.Z = a.Z + b.Z;
  return sum;
}

CAA3DCoordinate local_times(double a, CAA3DCoordinate & b)
{
  CAA3DCoordinate prod;
  prod.X = a * b.X;
  prod.Y = a * b.Y;
  prod.Z = a * b.Z;
  return prod;
}

CAA3DCoordinate local_times(CAA3DCoordinate & a, double b)
{
  CAA3DCoordinate prod;
  prod.X = a.X * b;
  prod.Y = a.Y * b;
  prod.Z = a.Z * b;
  return prod;
}

double local_dot_product(CAA3DCoordinate & a, CAA3DCoordinate & b)
{
  return a.X * b.X + a.Y * b.Y + a.Z * b.Z;
}

CAA3DCoordinate local_LBR_to_XYZ(CAA3DCoordinate lbr)
{
  double L = CAACoordinateTransformation::DegreesToRadians(lbr.X);
  double B = CAACoordinateTransformation::DegreesToRadians(lbr.Y);
  double R = lbr.Z;
  CAA3DCoordinate xyz;
  double cosB = cos(B);
  double sinB = sin(B);
  double cosL = cos(L);
  double sinL = sin(L);
  xyz.X = R * cosB * cosL;
  xyz.Y = R * cosB * sinL;
  xyz.Z = R * sinB;
  return xyz;
}

/* Reverse transformations. (First implemented for debugging.) */
CAA3DCoordinate local_XYZ_to_LBR(CAA3DCoordinate xyz)
{
  CAA3DCoordinate lbr;
  double x2 = xyz.X * xyz.X;
  double y2 = xyz.Y * xyz.Y;
  double z2 = xyz.Z * xyz.Z;
  lbr.X = atan2(xyz.Y, xyz.X );
  lbr.X = CAACoordinateTransformation::RadiansToDegrees(lbr.X);
  lbr.X = CAACoordinateTransformation::MapTo0To360Range( lbr.X );
  lbr.Y = atan2( xyz.Z, sqrt( x2 + y2 ) );
  lbr.Y = CAACoordinateTransformation::RadiansToDegrees(lbr.Y);
  lbr.Z = sqrt( x2 + y2 + z2 );
  return lbr;
}

CAA3DCoordinate local_velocity_earth(double JD, double dt)
{
  CAA3DCoordinate lbr;
  lbr.X = CAAEarth::EclipticLongitude(JD + dt);
  lbr.Y = CAAEarth::EclipticLatitude(JD + dt);
  lbr.Z = CAAEarth::RadiusVector(JD + dt);
  CAA3DCoordinate R_forward = local_LBR_to_XYZ(lbr);
  lbr.X = CAAEarth::EclipticLongitude(JD - dt);
  lbr.Y = CAAEarth::EclipticLatitude(JD - dt);
  lbr.Z = CAAEarth::RadiusVector(JD - dt);
  CAA3DCoordinate R_backward = local_LBR_to_XYZ(lbr);
  dt *= 2.0;
  CAA3DCoordinate V_earth;
  V_earth.X = (R_forward.X - R_backward.X) / dt;
  V_earth.Y = (R_forward.Y - R_backward.Y) / dt;
  V_earth.Z = (R_forward.Z - R_backward.Z) / dt;
  return V_earth;
}



CAA3DCoordinate local_vsop87_XYZ(const std::string & body, const double JD)
{
  CAA3DCoordinate lbr;
  if (body == "Sun") {
    CAA3DCoordinate xyz;
    xyz.X = xyz.Y = xyz.Z = 0.0;
    return xyz;
  } else if (body == "Mercury") {
    lbr.X = CAAMercury::EclipticLongitude(JD);
    lbr.Y = CAAMercury::EclipticLatitude(JD);
    lbr.Z = CAAMercury::RadiusVector(JD);
    return local_LBR_to_XYZ(lbr);
  } else if (body == "Venus") {
    lbr.X = CAAVenus::EclipticLongitude(JD);
    lbr.Y = CAAVenus::EclipticLatitude(JD);
    lbr.Z = CAAVenus::RadiusVector(JD);
    printf( "lbr Venus\n" );
    printf( "L %20.8f\n", lbr.X );
    printf( "B %20.8f\n", lbr.Y );
    printf( "R %20.8f\n", lbr.Z );
    return local_LBR_to_XYZ(lbr);
  } else if (body == "Earth") {
    lbr.X = CAAEarth::EclipticLongitude(JD);
    lbr.Y = CAAEarth::EclipticLatitude(JD);
    lbr.Z = CAAEarth::RadiusVector(JD);
    printf( "lbr Earth\n" );
    printf( "L %20.8f\n", lbr.X );
    printf( "B %20.8f\n", lbr.Y );
    printf( "R %20.8f\n", lbr.Z );
    return local_LBR_to_XYZ(lbr);
  } else if (body == "Mars") {
    lbr.X = CAAMars::EclipticLongitude(JD);
    lbr.Y = CAAMars::EclipticLatitude(JD);
    lbr.Z = CAAMars::RadiusVector(JD);
    return local_LBR_to_XYZ(lbr);
  } else if (body == "Jupiter") {
    lbr.X = CAAJupiter::EclipticLongitude(JD);
    lbr.Y = CAAJupiter::EclipticLatitude(JD);
    lbr.Z = CAAJupiter::RadiusVector(JD);
    return local_LBR_to_XYZ(lbr);
  } else if (body == "Saturn") {
    lbr.X = CAASaturn::EclipticLongitude(JD);
    lbr.Y = CAASaturn::EclipticLatitude(JD);
    lbr.Z = CAASaturn::RadiusVector(JD);
    return local_LBR_to_XYZ(lbr);
  } else if (body == "Uranus") {
    lbr.X = CAAUranus::EclipticLongitude(JD);
    lbr.Y = CAAUranus::EclipticLatitude(JD);
    lbr.Z = CAAUranus::RadiusVector(JD);
    return local_LBR_to_XYZ(lbr);
  } else if (body == "Neptune") {
    lbr.X = CAANeptune::EclipticLongitude(JD);
    lbr.Y = CAANeptune::EclipticLatitude(JD);
    lbr.Z = CAANeptune::RadiusVector(JD);
    return local_LBR_to_XYZ(lbr);
  } else if (body == "Pluto") {
    lbr.X = CAAPluto::EclipticLongitude(JD);
    lbr.Y = CAAPluto::EclipticLatitude(JD);
    lbr.Z = CAAPluto::RadiusVector(JD);
    return local_LBR_to_XYZ(lbr);
  } else {
    CAA3DCoordinate xyz;
    xyz.X = xyz.Y = xyz.Z = 0.0;
    return xyz;
  }
}


CAA3DCoordinate local_apparent_ecliptic_position_xyz(std::string body, double JD)
{

  CAA3DCoordinate vantage = local_vsop87_XYZ("Earth", JD);
  /* Pure Debugging */
  CAA3DCoordinate lbr_earth = local_XYZ_to_LBR( vantage );
  lbr_earth.Z *= 1.0;

  CAA3DCoordinate vsop87body = local_vsop87_XYZ(body, JD);
  for (int32_t iteration = 1; iteration <= 2; ++iteration) {
    double dx = vsop87body.X - vantage.X;
    double dy = vsop87body.Y - vantage.Y;
    double dz = vsop87body.Z - vantage.Z;
    double dist = sqrt(dx * dx + dy * dy + dz * dz);
    vsop87body = local_vsop87_XYZ(body, JD - dist / (solar_system::C_AU_per_day));
  }
  /* Pure Debugging */
  CAA3DCoordinate lbr_body = local_XYZ_to_LBR( vsop87body );
  lbr_body.Z *= 1.0;



  /* Astrometric position takes into account time for light to travel but not abberation. */
  CAA3DCoordinate astrometric_position = local_minus(vsop87body, vantage);


  printf( "XYZ difference\n" );
  printf( "X %20.8f\n", astrometric_position.X - .621794 );
  printf( "Y %20.8f\n", astrometric_position.Y - -.664905);
  printf( "Z %20.8f\n", astrometric_position.Z - -.033138);

  CAA3DCoordinate lbr_astro_position = local_XYZ_to_LBR( astrometric_position );
  lbr_astro_position.Z *=1.0;
  printf( "lbr Astrometric Distance error\n" );
  printf( "L %20.8f\n", lbr_astro_position.X - 313.08102);
  printf( "B %20.8f\n", lbr_astro_position.Y - -2.08474);
  printf( "R %20.8f\n", lbr_astro_position.Z );


  double tau =
      sqrt(local_dot_product(astrometric_position, astrometric_position)) /
      solar_system::C_AU_per_day;
  printf( "tau %18.8f\n", tau );
  /*  dt = 1/2048.0 days is about 42 seconds. */
  CAA3DCoordinate v_earth = local_velocity_earth(JD, 1.0 / 2048.0);
  double v2 = local_dot_product(v_earth, v_earth);
  double v = sqrt(v2);
  /* beta and gamma are the symbols commonly used in Lorentz transformations. */
  double beta = v / solar_system::C_AU_per_day;
  double gamma = 1.0 / sqrt(1 - beta * beta);
  CAA3DCoordinate uv = local_times(v_earth, 1.0 / v);

  /* Decompose astrometric_position into components parallel and perpendicular to v_earth. */
  double mag_parallel = local_dot_product(uv, astrometric_position);
  CAA3DCoordinate R_parallel = local_times(mag_parallel, uv);
  CAA3DCoordinate R_perpendicular = local_minus(astrometric_position, R_parallel);

  double mag_parallel_transformed = gamma * (mag_parallel + v * tau);
  R_parallel = local_times(mag_parallel_transformed, uv);
  /* Logically tau is also transformed, but we don't use the tranformed value. */
  CAA3DCoordinate R_transfomed = local_plus(R_perpendicular, R_parallel);
  return R_transfomed;
}





CAAEllipticalPlanetaryDetails local_calculate_details(std::string body,
                                                              double JD)
{
  CAA3DCoordinate R = local_apparent_ecliptic_position_xyz(body, JD);

  printf( "XYZ Corrected for Aberration ... 7777\n" );
  printf( "X %20.8f\n", R.X );
  printf( "Y %20.8f\n", R.Y );
  printf( "Z %20.8f\n", R.Z );

  CAA3DCoordinate angular = local_XYZ_to_LBR(R);
  printf( "lbr aberrated error\n" );
  printf( "L %20.8f\n", angular.X - 313.07689);
  printf( "B %20.8f\n", angular.Y - (-2.08489));
  printf( "R %20.8f\n", angular.Z );



  /*
   *  R is corrected for light travel time and aberration, so the rest of the
   *  calculation is "only" a coordinate transformation.   The complication
   *  is that the [tranformed_to_coordinate_system] depends upon the physical motions
   *  called precession and nutation.
   */
  CAAEllipticalPlanetaryDetails details;
  // CAA3DCoordinate FK5_mean_of_the_day = CAAFK5::ConvertVSOPToFK5AnyEquinox( R, JD );
  double x2 = R.X * R.X;
  double y2 = R.Y * R.Y;
  double z2 = R.Z * R.Z;
  details.ApparentGeocentricLatitude =
    CAACoordinateTransformation::RadiansToDegrees(atan2(R.Z, sqrt(x2 + y2)));
  details.ApparentGeocentricDistance = sqrt(x2 + y2 + z2);
  details.ApparentGeocentricLongitude =
    CAACoordinateTransformation::MapTo0To360Range
    (CAACoordinateTransformation::RadiansToDegrees(atan2(R.Y, R.X)));




  double NutationInLongitude = CAANutation::NutationInLongitude(JD);
  double Epsilon = CAANutation::TrueObliquityOfEcliptic(JD);
  details.ApparentGeocentricLongitude +=
    CAACoordinateTransformation::DMSToDegrees(0, 0, NutationInLongitude);



  /* Convert to RA and Declination */
  CAA2DCoordinate ApparentEqu =
    CAACoordinateTransformation::
    Ecliptic2Equatorial(details.ApparentGeocentricLongitude,
			details.ApparentGeocentricLatitude,
			Epsilon);
  details.ApparentGeocentricRA = ApparentEqu.X;
  details.ApparentGeocentricDeclination = ApparentEqu.Y;
  details.ApparentLightTime =
    details.ApparentGeocentricDistance / solar_system::C_AU_per_day;

  return details;
 }



void example_pretty_print( CAAEllipticalPlanetaryDetails *pd, std::string name ){
  double RA = pd->ApparentGeocentricRA;
  double DEC = pd->ApparentGeocentricDeclination;
  // cout << sexagesimal::double2sexagesimal(GHA_object).to_hmxxxx_string()  << "    ";
  // cout << sexagesimal::double2sexagesimal(DEC).to_hmxxxx_string()  << endl;
  cout << "RA and Declination:   ";
  cout << sexagesimal::double2sexagesimal(RA).to_string()  << "    ";
  cout << sexagesimal::double2sexagesimal(DEC).to_string() << "   ";
  cout << name << "   " << endl;
}

int main( int argc, char **argv){
  cout << "Example 33.a page 225." << endl;
  cout << "Jean Meeus, Astronomical Algorithms" << endl;
  cout << "gcc = " << __GNUC__ << "." << __GNUC_MINOR__ << "." << __GNUC_PATCHLEVEL__ << endl;
  cout << "__cplusplus = " << __cplusplus << endl;
  CAADate test_date{1992,12,20,0,0,0,true};
  double JD = test_date.Julian();
  printf( "Test Date = %18.6f\n", JD );
  std::string body = "Venus";
  cout << "Solar system body = " << body << endl;
  CAAEllipticalPlanetaryDetails details = solar_system::calculate_details(body, JD);
  example_pretty_print( &details, body );

  cout << "Local copy of calulate_details( body, JD ) " << endl;

  details = local_calculate_details(body, JD);
  

  example_pretty_print( &details, body );

}
