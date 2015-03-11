#include "lbr_and_xyz.h"

#include "AACoordinateTransformation.h"
#include <cmath>

CAA3DCoordinate LBR_to_XYZ(CAA3DCoordinate lbr)
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

/* Reverse transformation. */
CAA3DCoordinate XYZ_to_LBR(CAA3DCoordinate xyz)
{
  CAA3DCoordinate lbr;
  double x2 = xyz.X * xyz.X;
  double y2 = xyz.Y * xyz.Y;
  double z2 = xyz.Z * xyz.Z;
  lbr.X = atan2(xyz.Y, xyz.X);
  lbr.X = CAACoordinateTransformation::RadiansToDegrees(lbr.X);
  lbr.X = CAACoordinateTransformation::MapTo0To360Range(lbr.X);
  lbr.Y = atan2(xyz.Z, sqrt(x2 + y2));
  lbr.Y = CAACoordinateTransformation::RadiansToDegrees(lbr.Y);
  lbr.Z = sqrt(x2 + y2 + z2);
  return lbr;
}

/* 
 * Used for transforming 
 *    {Azimuth,Altitude,1.0}   <--[to|from]-->  unit vector {x,y,z}
 * in the topocentic coordinate system.
 * The z coordinate of corresponding to an object above the horizon will be negative.
 * Think of the unit vector as representing the direction of an incoming photon.
 *
 * Debugged under the notion that azimuth is zero to the north and increases
 * moving eastward from north.  Should work fine if zero azimuth is south.
 * 
 * The Jacobian of this transformation is positive.
 */
CAA3DCoordinate XYZ_to_AziAltR(CAA3DCoordinate xyz){
  /* Reverse Y and Z, (which is a 180 degree rotation about X) */
  xyz.Y = -xyz.Y;
  xyz.Z = -xyz.Z;
  CAA3DCoordinate temp = XYZ_to_LBR( xyz );
  return temp;
}



CAA3DCoordinate AziAltR_to_XYZ(CAA3DCoordinate azi_alt_r){
  CAA3DCoordinate temp;
  if( azi_alt_r.Z < 0 ){
    for(;;){
      // Spin for ever.
    }
  }
  temp = LBR_to_XYZ( azi_alt_r );
  temp.Y = -temp.Y;
  temp.Z = -temp.Z;
  return temp;
}

