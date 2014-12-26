#include "float_coordinates.h"

#include "AACoordinateTransformation.h"
#include <cmath>

/* Narrowing cast constructor from double to float. */
CAA2Dfloat::CAA2Dfloat( CAA2DCoordinate src ){ 
  X = static_cast<float>( src.X );
  Y = static_cast<float>( src.Y );
}

/* Narrowing cast constructor from double to float. */
CAA3Dfloat::CAA3Dfloat( CAA3DCoordinate src ){ 
  X = static_cast<float>( src.X );
  Y = static_cast<float>( src.Y );
  Z = static_cast<float>( src.Z );
}

/* Spherical to rectangular coordinate transformation. */
CAA3Dfloat LBR_to_XYZ(CAA3Dfloat lbr)
{
  float L = CAACoordinateTransformation::DegreesToRadians(lbr.X);  
  float B = CAACoordinateTransformation::DegreesToRadians(lbr.Y);
  float R = lbr.Z;
  CAA3Dfloat xyz;
  float cosB = cos(B);
  float sinB = sin(B);
  float cosL = cos(L);
  float sinL = sin(L);
  xyz.X = R * cosB * cosL;
  xyz.Y = R * cosB * sinL;
  xyz.Z = R * sinB;
  return xyz;
}

/* Reverse transformation. */
CAA3Dfloat XYZ_to_LBR(CAA3Dfloat xyz)
{
  CAA3Dfloat lbr;
  float x2 = xyz.X * xyz.X;
  float y2 = xyz.Y * xyz.Y;
  float z2 = xyz.Z * xyz.Z;
  lbr.X = atan2(xyz.Y, xyz.X );
  lbr.X = CAACoordinateTransformation::RadiansToDegrees(lbr.X);
  lbr.X = CAACoordinateTransformation::MapTo0To360Range( lbr.X );
  lbr.Y = atan2( xyz.Z, sqrt( x2 + y2 ) );
  lbr.Y = CAACoordinateTransformation::RadiansToDegrees(lbr.Y);
  lbr.Z = sqrt( x2 + y2 + z2 );
  return lbr;
}
