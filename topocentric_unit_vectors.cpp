#include "topocentric_unit_vectors.h"


#include "binary_tidbits.h"
#include "lbr_and_xyz.h"

CAA3DCoordinate topocentric_unit_vectors::AziAlt_to_UV(CAA2DCoordinate azi_alt){
  CAA3DCoordinate Azi_Alt_R;
  Azi_Alt_R.X = azi_alt.X;
  Azi_Alt_R.Y = azi_alt.Y;
  Azi_Alt_R.Z = 1.0;
  return topocentric_unit_vectors::AziAltR_to_XYZ( Azi_Alt_R );
}


CAA2DCoordinate topocentric_unit_vectors::UV_to_AziAlt(CAA3DCoordinate topocentric_uv ){
  CAA3DCoordinate Azi_Alt_R = topocentric_unit_vectors::XYZ_to_AziAltR( topocentric_uv );
  CAA2DCoordinate azi_alt;
  azi_alt.X = Azi_Alt_R.X;
  azi_alt.Y = Azi_Alt_R.Y;
  return azi_alt;
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
CAA3DCoordinate topocentric_unit_vectors::XYZ_to_AziAltR(CAA3DCoordinate xyz){
  /* Reflect thorugh the origin  */
  xyz.X = -xyz.X;
  xyz.Y = -xyz.Y;
  xyz.Z = -xyz.Z;
  /* Use a well tested rectangular -> spherical conversion. */
  CAA3DCoordinate temp = XYZ_to_LBR( xyz );
  /* Reflect thorugh the origin in spherical coordinates.  */
  temp.X += 180.0;
  temp.Y = - temp.Y;
  return temp;
}


/*
 * The Jacobian of this transformation is positive IF R > 0.
 */
CAA3DCoordinate topocentric_unit_vectors::AziAltR_to_XYZ(CAA3DCoordinate azi_alt_r){

  azi_alt_r.X -= 180;
  azi_alt_r.Y = -azi_alt_r.Y;
  CAA3DCoordinate temp = LBR_to_XYZ( azi_alt_r );  
  temp.X = -temp.X;
  temp.Y = -temp.Y;
  temp.Z = -temp.Z;
  return temp;
}
