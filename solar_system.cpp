#include "solar_system.h"

#include "AACoordinateTransformation.h"
#include "AA3DCoordinate.h"
#include <cmath>
#include "AASun.h"
#include "AAMercury.h"
#include "AAVenus.h"
#include "AAMars.h"
#include "AAEarth.h"
#include "AAJupiter.h"
#include "AASaturn.h"
#include "AAUranus.h"
#include "AANeptune.h"
#include "AAPluto.h"
#include "AAElliptical.h"
// #include "AAFK5.h"
#include "AANutation.h"
#include "AAMoon.h"
#include "lbr_and_xyz.h"

// #include "main.h"
/*
 * Earth velocity determined by numerical differentiation of position.
 * Don't make dt too small.  Because forward and backward evaluation
 * points a fairly large dt should be fine.
 * Try 1 minute = (1.0/1440.0) day.
 * Better, use an nearby power of two: 1.0/1024.0.
 * Should be good enough for aberration computation.
 * Units of the returned value is (AU/day).
 *
 */
CAA3DCoordinate solar_system::velocity_Earth(double JD, double dt)
{
  CAA3DCoordinate lbr;
  lbr.X = CAAEarth::EclipticLongitude(JD + dt);
  lbr.Y = CAAEarth::EclipticLatitude(JD + dt);
  lbr.Z = CAAEarth::RadiusVector(JD + dt);
  CAA3DCoordinate R_forward = LBR_to_XYZ(lbr);
  lbr.X = CAAEarth::EclipticLongitude(JD - dt);
  lbr.Y = CAAEarth::EclipticLatitude(JD - dt);
  lbr.Z = CAAEarth::RadiusVector(JD - dt);
  CAA3DCoordinate R_backward = LBR_to_XYZ(lbr);
  dt *= 2.0;
  CAA3DCoordinate V_earth;
  V_earth.X = (R_forward.X - R_backward.X) / dt;
  V_earth.Y = (R_forward.Y - R_backward.Y) / dt;
  V_earth.Z = (R_forward.Z - R_backward.Z) / dt;
  return V_earth;
}

double solar_system::dot_product(CAA3DCoordinate & a, CAA3DCoordinate & b)
{
  return a.X * b.X + a.Y * b.Y + a.Z * b.Z;
}

CAA3DCoordinate solar_system::minus(CAA3DCoordinate & a, CAA3DCoordinate & b)
{
  CAA3DCoordinate diff;
  diff.X = a.X - b.X;
  diff.Y = a.Y - b.Y;
  diff.Z = a.Z - b.Z;
  return diff;
}

CAA3DCoordinate solar_system::plus(CAA3DCoordinate & a, CAA3DCoordinate & b)
{
  CAA3DCoordinate sum;
  sum.X = a.X + b.X;
  sum.Y = a.Y + b.Y;
  sum.Z = a.Z + b.Z;
  return sum;
}

CAA3DCoordinate solar_system::product(double a, CAA3DCoordinate & b)
{
  CAA3DCoordinate prod;
  prod.X = a * b.X;
  prod.Y = a * b.Y;
  prod.Z = a * b.Z;
  return prod;
}

CAA3DCoordinate solar_system::product(CAA3DCoordinate & a, double b)
{
  CAA3DCoordinate prod;
  prod.X = a.X * b;
  prod.Y = a.Y * b;
  prod.Z = a.Z * b;
  return prod;
}

std::string solar_system::solar_system_body_name(int num)
{
  switch (num) {
  case 0:
    return "Sun";
  case 1:
    return "Mercury";
  case 2:
    return "Venus";
  case 3:
    return "Moon";
  case 4:
    return "Mars";
  case 5:
    return "Jupiter";
  case 6:
    return "Saturn";
  case 7:
    return "Uranus";
  case 8:
    return "Neptune";
  case 9:
    return "Pluto";
  default:
    for (;;) {
      /* 
       *  Removed assert for compilation to desktop native for testing.
       */
    }
    //assert_param( false );
    return "Sherman's Planet";
  }
}

CAA3DCoordinate vsop87_XYZ(const std::string & body, const double JD)
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
    return LBR_to_XYZ(lbr);
  } else if (body == "Venus") {
    lbr.X = CAAVenus::EclipticLongitude(JD);
    lbr.Y = CAAVenus::EclipticLatitude(JD);
    lbr.Z = CAAVenus::RadiusVector(JD);
    return LBR_to_XYZ(lbr);
  } else if (body == "Earth") {
    lbr.X = CAAEarth::EclipticLongitude(JD);
    lbr.Y = CAAEarth::EclipticLatitude(JD);
    lbr.Z = CAAEarth::RadiusVector(JD);
    return LBR_to_XYZ(lbr);
  } else if (body == "Mars") {
    lbr.X = CAAMars::EclipticLongitude(JD);
    lbr.Y = CAAMars::EclipticLatitude(JD);
    lbr.Z = CAAMars::RadiusVector(JD);
    return LBR_to_XYZ(lbr);
  } else if (body == "Jupiter") {
    lbr.X = CAAJupiter::EclipticLongitude(JD);
    lbr.Y = CAAJupiter::EclipticLatitude(JD);
    lbr.Z = CAAJupiter::RadiusVector(JD);
    return LBR_to_XYZ(lbr);
  } else if (body == "Saturn") {
    lbr.X = CAASaturn::EclipticLongitude(JD);
    lbr.Y = CAASaturn::EclipticLatitude(JD);
    lbr.Z = CAASaturn::RadiusVector(JD);
    return LBR_to_XYZ(lbr);
  } else if (body == "Uranus") {
    lbr.X = CAAUranus::EclipticLongitude(JD);
    lbr.Y = CAAUranus::EclipticLatitude(JD);
    lbr.Z = CAAUranus::RadiusVector(JD);
    return LBR_to_XYZ(lbr);
  } else if (body == "Neptune") {
    lbr.X = CAANeptune::EclipticLongitude(JD);
    lbr.Y = CAANeptune::EclipticLatitude(JD);
    lbr.Z = CAANeptune::RadiusVector(JD);
    return LBR_to_XYZ(lbr);
  } else if (body == "Pluto") {	/* Not really VSOP87.  Is the coordinate system correct? */
    lbr.X = CAAPluto::EclipticLongitude(JD);
    lbr.Y = CAAPluto::EclipticLatitude(JD);
    lbr.Z = CAAPluto::RadiusVector(JD);
    return LBR_to_XYZ(lbr);
  } else {
    CAA3DCoordinate xyz;
    xyz.X = xyz.Y = xyz.Z = 0.0;
    return xyz;
  }
}

CAA3DCoordinate apparent_ecliptic_position_xyz(std::string body, double JD)
{
  CAA3DCoordinate vantage = vsop87_XYZ("Earth", JD);

  CAA3DCoordinate vsop87body = vsop87_XYZ(body, JD);
  for (int32_t iteration = 1; iteration <= 3; ++iteration) {
    double dx = vsop87body.X - vantage.X;
    double dy = vsop87body.Y - vantage.Y;
    double dz = vsop87body.Z - vantage.Z;
    double dist = sqrt(dx * dx + dy * dy + dz * dz);
    vsop87body = vsop87_XYZ(body, JD - dist / (solar_system::C_AU_per_day));
  }
  /* Astrometric position takes into account time for light to travel but not abberation. */
  CAA3DCoordinate astrometric_position = solar_system::minus(vsop87body, vantage);
  double tau =
      sqrt(solar_system::
	   dot_product(astrometric_position,
		       astrometric_position)) / solar_system::C_AU_per_day;

  /*  dt = 1/2048.0 days is about 42 seconds. */
  CAA3DCoordinate v_earth = solar_system::velocity_Earth(JD, 1.0 / 2048.0);
  double v2 = solar_system::dot_product(v_earth, v_earth);
  double v = sqrt(v2);
  /* beta and gamma are the symbols commonly used in Lorentz transformations. */
  double beta = v / solar_system::C_AU_per_day;
  double gamma = 1.0 / sqrt(1 - beta * beta);
  CAA3DCoordinate uv = solar_system::product(v_earth, 1.0 / v);
  /* Decompose astrometric_position into components parallel and perpendicular to v_earth. */
  double mag_parallel = solar_system::dot_product(uv, astrometric_position);
  CAA3DCoordinate R_parallel = solar_system::product(mag_parallel, uv);
  CAA3DCoordinate R_perpendicular = solar_system::minus(astrometric_position, R_parallel);
  double mag_parallel_transformed = gamma * (mag_parallel + v * tau);
  R_parallel = solar_system::product(mag_parallel_transformed, uv);
  /* Logically tau is also transformed, but we don't use the tranformed value. */
  CAA3DCoordinate R_transfomed = solar_system::plus(R_perpendicular, R_parallel);
  return R_transfomed;
}

CAAEllipticalPlanetaryDetails solar_system::calculate_details(std::string body,
							      double JD)
{
  CAA3DCoordinate R = apparent_ecliptic_position_xyz(body, JD);
  CAA3DCoordinate lbr = XYZ_to_LBR(R);
  /*  
   *  R is corrected for light travel time and aberration, so the rest of the
   *  calculation is "only" a coordinate transformation.   The complication 
   *  is that the [tranformed_to_coordinate_system] depends upon the physical motions
   *  called precession and nutation. 
   */
  CAAEllipticalPlanetaryDetails details;

  details.ApparentGeocentricLongitude = lbr.X;
  details.ApparentGeocentricLatitude = lbr.Y;
  details.ApparentGeocentricDistance = lbr.Z;

  double NutationInLongitude = CAANutation::NutationInLongitude(JD);
  double Epsilon = CAANutation::TrueObliquityOfEcliptic(JD);
  details.ApparentGeocentricLongitude +=
      CAACoordinateTransformation::DMSToDegrees(0, 0, NutationInLongitude);

  /* Convert to RA and Declination */
  CAA2DCoordinate ApparentEqu =
      CAACoordinateTransformation::Ecliptic2Equatorial(details.
						       ApparentGeocentricLongitude,
						       details.
						       ApparentGeocentricLatitude,
						       Epsilon);
  details.ApparentGeocentricRA = ApparentEqu.X;
  details.ApparentGeocentricDeclination = ApparentEqu.Y;
  details.ApparentLightTime =
      details.ApparentGeocentricDistance / solar_system::C_AU_per_day;

  return details;
}

CAA3DCoordinate solar_system::calculate_moon_RA_Dec_Dist(double JD)
{
  CAA3DCoordinate RA_Dec_Dist;
  double L = CAAMoon::EclipticLongitude(JD);
  double B = CAAMoon::EclipticLatitude(JD);
  double R = CAAMoon::RadiusVector(JD);
  /* Something is wrong, probably here. */
  double NutationInLongitude = CAANutation::NutationInLongitude(JD);
  double Epsilon = CAANutation::TrueObliquityOfEcliptic(JD);
  L += CAACoordinateTransformation::DMSToDegrees(0, 0, NutationInLongitude);
  /* Convert to RA and Declination */
  CAA2DCoordinate RA_DEC =
      CAACoordinateTransformation::Ecliptic2Equatorial(L, B, Epsilon);

  RA_Dec_Dist.X = RA_DEC.X;
  RA_Dec_Dist.Y = RA_DEC.Y;
  RA_Dec_Dist.Z = R;

  return RA_Dec_Dist;
}
