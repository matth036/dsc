#ifndef FLOAT_COORDINATES_H
#define FLOAT_COORDINATES_H

#include "AA3DCoordinate.h"
#include "AA2DCoordinate.h"
#include "AACoordinateTransformation.h"



template<typename N> struct Triple{
  N X;
  N Y;
  N Z;
};

template<typename N>
Triple<N> LBR_to_XYZ( Triple<N> lbr ){
  N L = CAACoordinateTransformation::DegreesToRadians(lbr.X);
  N B = CAACoordinateTransformation::DegreesToRadians(lbr.Y);
  N R = lbr.Z;
  Triple<N> xyz;
  N cosB = cos(B);
  N sinB = sin(B);
  N cosL = cos(L);
  N sinL = sin(L);
  xyz.X = R * cosB * cosL;
  xyz.Y = R * cosB * sinL;
  xyz.Z = R * sinB;
  return xyz;
};

template<typename N>
Triple<N> XYZ_to_LBR( Triple<N> xyz )
{
  Triple<N> lbr;
  N x2 = xyz.X * xyz.X;
  N y2 = xyz.Y * xyz.Y;
  N z2 = xyz.Z * xyz.Z;
  lbr.X = atan2(xyz.Y, xyz.X );
  lbr.X = CAACoordinateTransformation::RadiansToDegrees(lbr.X);
  lbr.X = CAACoordinateTransformation::MapTo0To360Range( lbr.X );
  lbr.Y = atan2( xyz.Z, sqrt( x2 + y2 ) );
  lbr.Y = CAACoordinateTransformation::RadiansToDegrees(lbr.Y);
  lbr.Z = sqrt( x2 + y2 + z2 );
  return lbr;
}




struct CAA2Dfloat{
  CAA2Dfloat()=default;
  CAA2Dfloat( CAA2DCoordinate src ); /* Cast from double to float */
  float X;
  float Y;
};



struct CAA3Dfloat{
  CAA3Dfloat()=default;
  CAA3Dfloat( CAA3DCoordinate src ); /* Cast from double to float */
  float X;
  float Y;
  float Z;
};

CAA3Dfloat LBR_to_XYZ(CAA3Dfloat lbr);
CAA3Dfloat XYZ_to_LBR(CAA3Dfloat xyz);

#endif
