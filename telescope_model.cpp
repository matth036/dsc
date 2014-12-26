#include "telescope_model.h"
#include "quadrature_decoder.h"
#include "AACoordinateTransformation.h"
#include "solar_system.h"  /* Happens to contain LBR_to_XYZ(CAA3DCoordinate lbr) */
#include "alignment_sight_info.h"
#include "rtc_management.h"
#include "AARefraction.h"
#include "AASidereal.h"
#include "AADate.h"
#include "binary_tidbits.h"
#include "sexagesimal.h"
#include "build_date.h"
#include "horizontal_equatorial.h"

Simple_Altazimuth_Scope::Simple_Altazimuth_Scope( std::unique_ptr<Quadrature_Decoder> azi, 
						  std::unique_ptr<Quadrature_Decoder> alt)
{
  azimuth_hardware = std::move(azi);
  altitude_hardware = std::move(alt);
  /* Assume we start with the telescope pointed toward Polaris.
   * This can be very approximate.
   *
   * While azimuth offset can be arbitrary the telescope and 
   * topocentric frames are nearly cooincident with a judiciously 
   * choosen azimuth offset. This aids debugging.
   */
  if( binary_tidbits::zero_azimuth_is_north() ){
    azimuth_offset = 0.0;
  }else{
    azimuth_offset = 180.0;
  }
  altitude_offset = 0.0;
  reverse_azimuth_direction = false;
  reverse_altitude_direction = false;
}
 
Alt_Azi_Snapshot_t Simple_Altazimuth_Scope::get_snapshot(){
  Alt_Azi_Snapshot_t d;
  d.alt_value = altitude_hardware->get_count();
  d.azi_value = azimuth_hardware->get_count();
  return d;
}

const CAA3DCoordinate Simple_Altazimuth_Scope::altitude_forward_backward_difference( Alt_Azi_Snapshot_t aa ){
  Alt_Azi_Snapshot_t temp;
  uint32_t modulus = altitude_hardware->get_ticks_per_revolution();
  temp.alt_value = aa.alt_value + 1;
  temp.azi_value = aa.azi_value;
  CAA3DCoordinate forward = calculate_unit_vector( temp );
  if( aa.alt_value > 0 ){
    temp.alt_value = aa.alt_value - 1;
  }else{
    temp.alt_value = modulus - 1;
  }
  temp.azi_value = aa.azi_value;
  CAA3DCoordinate aftward = calculate_unit_vector( temp );
  CAA3DCoordinate diff;
  diff.X = forward.X - aftward.X;
  diff.Y = forward.Y - aftward.Y;
  diff.Z = forward.Z - aftward.Z;
  return diff;
}

const CAA3DCoordinate Simple_Altazimuth_Scope::calculate_unit_vector( Alt_Azi_Snapshot_t aa ){
  CAA3DCoordinate uv;
  uv.X = -azimuth_degrees( aa.azi_value );  /*  DETERMINANT REVERSE REVERSING ACTION  */
  /* Check zero_azimuth_is_north. */
  if( binary_tidbits::zero_azimuth_is_north() ){

  }else{
    uv.X += 180.0;
    assert_param( false ); /* This code not tested. */
  }
  uv.Y = altitude_degrees( aa.alt_value ) ;
  /* d(azimuth), d(altitude), d(R) is left handed. 
   * d(azimuth), d(altitude), d(-R) is right handed. 
   */
  uv.Z = 1.0;   /*  DETERMINANT REVERSE REVERSING ACTION  */
  return LBR_to_XYZ(uv);
}

Alt_Azi_Snapshot_t Simple_Altazimuth_Scope::calculate_target_snapshot(CAA3Dfloat uv ){
  Alt_Azi_Snapshot_t encoder_target;  
  CAA3Dfloat lbr = XYZ_to_LBR( uv ); /* How well has this been tested?  */
  lbr.X = -lbr.X;
  encoder_target.azi_value = static_cast<int>(floor( lbr.X*azimuth_hardware->get_ticks_per_revolution()/360.0 + .5 ));
  encoder_target.alt_value = static_cast<int>(floor( lbr.Y*altitude_hardware->get_ticks_per_revolution()/360.0 + .5 ));
  return encoder_target;
}

Alt_Azi_Snapshot_t Simple_Altazimuth_Scope::calculate_target_snapshot(CAA3DCoordinate uv ){
  CAA3Dfloat uv_float{uv};
  return calculate_target_snapshot( uv_float );
}

const double Simple_Altazimuth_Scope::altitude_degrees( ){
  return altitude_degrees( altitude_hardware->get_count() );
}

const double Simple_Altazimuth_Scope::azimuth_degrees( ){
  return azimuth_degrees( azimuth_hardware->get_count() );
}

/*
  DO NOT TAMPER WITH THIS FUNCTION  UNTIL THE EQUATORIAL COORDINATE VIEW HAS BEEN DEGUGGED.  dan -Dec 2014
  As the scope is wired, this returns about zero near the horizon and about 90 near the zenith.
 */

const double Simple_Altazimuth_Scope::altitude_degrees( uint32_t encoder_value ){
  double altitude;
  if( reverse_altitude_direction ){
    altitude = -static_cast<double>(encoder_value);
  }else{
    altitude = static_cast<double>(encoder_value);
  }
  altitude = (360.0 * altitude) / altitude_hardware->get_ticks_per_revolution();
  altitude -= altitude_offset;
  assert_param( altitude < 360.0*8 );
  assert_param( altitude > -360.0*8 );
  altitude = CAACoordinateTransformation::MapTo0To360Range( altitude );
  return altitude;
}

/*
  DO NOT TAMPER WITH THIS FUNCTION  UNTIL THE EQUATORIAL COORDINATE VIEW HAS BEEN DEGUGGED.  dan -Dec 2014
  As the scope is wired, this returns near zero sligly west of north, near 90 to the west.
 */
const double Simple_Altazimuth_Scope::azimuth_degrees( uint32_t encoder_value ){
  double azimuth;
  if( reverse_azimuth_direction ){
    azimuth = -static_cast<double>(encoder_value);
  }else{
    azimuth = static_cast<double>(encoder_value);
  }
  azimuth = (360.0 * azimuth) / azimuth_hardware->get_ticks_per_revolution();
  azimuth -= azimuth_offset;
  assert_param( azimuth < 360.0*8 );
  assert_param( azimuth > -360.0*8 );
  azimuth = CAACoordinateTransformation::MapTo0To360Range( azimuth );
  return azimuth;
}

const double Simple_Altazimuth_Scope::get_altitude_offset(){
  return altitude_offset;
}

const double Simple_Altazimuth_Scope::get_azimuth_offset(){
  return azimuth_offset;
}

void Simple_Altazimuth_Scope::set_altitude_offset( double d ){
  altitude_offset = d;
}

void Simple_Altazimuth_Scope::set_azimuth_offset( double d ){
  azimuth_offset = d;
}

void Simple_Altazimuth_Scope::invert_altitude_direction(){
  reverse_altitude_direction = !reverse_altitude_direction;
}

void Simple_Altazimuth_Scope::invert_azimuth_direction(){
  reverse_azimuth_direction = !reverse_azimuth_direction;
}

/* True if increasing encoder count returns decreasing altitude. */
const bool Simple_Altazimuth_Scope::altitude_direction_is_reversed(){
  return reverse_altitude_direction;
}

const bool Simple_Altazimuth_Scope::azimuth_direction_is_reversed(){
  return reverse_azimuth_direction;
}

void Simple_Altazimuth_Scope::set_top_to_tel_matrix( double* src ){
  /* Time seems right to abandon the pretense that double precision had meaning. */
  for(uint32_t i=0; i<9; ++i ){
    topocentric_to_telescope_matrix[i] = static_cast<float>(src[i]);
  }
}

void Simple_Altazimuth_Scope::set_top_to_tel_matrix( float* src ){
  /* Time seems right to abandon the pretense that double precision had meaning. */
  for(uint32_t i=0; i<9; ++i ){
    topocentric_to_telescope_matrix[i] = static_cast<float>(src[i]);
  }
}

void Simple_Altazimuth_Scope::set_determinant( double d ){
  determinant = static_cast<float>(d);
}

void Simple_Altazimuth_Scope::set_determinant( float d ){
  determinant = d;
}

float Simple_Altazimuth_Scope::get_determinant( ){
  return determinant;
}

void Simple_Altazimuth_Scope::set_align_error_mean( float e ){
  align_error_mean = e;
}

void Simple_Altazimuth_Scope::set_align_error_max( float e ){
  align_error_max = e;
}

float Simple_Altazimuth_Scope::get_align_error_mean(){
  return align_error_mean;
}
float Simple_Altazimuth_Scope::get_align_error_max(){
  return align_error_max;
}

int32_t Simple_Altazimuth_Scope::get_azimuth_ticks_per_revolution(){
  return azimuth_hardware->get_ticks_per_revolution();
}

int32_t Simple_Altazimuth_Scope::get_altitude_ticks_per_revolution(){
  return altitude_hardware->get_ticks_per_revolution();
}




CAA2Dfloat Simple_Altazimuth_Scope::topocentric_Azi_and_Alt(Alt_Azi_Snapshot_t snapshot){
  // FUCK ALERT
  
  CAA3Dfloat lbr;
  lbr.X = azimuth_degrees( snapshot.azi_value );
  
  lbr.Y = altitude_degrees( snapshot.alt_value );
  
  lbr.Z = 1.0;
  
  CAA3Dfloat xyz = LBR_to_XYZ( lbr );
  /* 
   * This transformation of xyz is unlikely to cause gross errors since the matrix
   * is close to the identity matrix.
   */
  xyz = tele_to_topo( xyz );
  
  lbr = XYZ_to_LBR( xyz );
  
  CAA2Dfloat azi_and_alt;

  azi_and_alt.X = lbr.X;
  azi_and_alt.Y = lbr.Y;

  return azi_and_alt;
}

CAA2Dfloat Simple_Altazimuth_Scope::current_topocentric_Azi_and_Alt(){
  Alt_Azi_Snapshot_t snapshot = get_snapshot();
  return topocentric_Azi_and_Alt( snapshot );
}

CAA2DCoordinate Simple_Altazimuth_Scope::current_RA_and_Dec(){
  float pressure = 1010.0;
  float temperature = 10;

  Alt_Azi_Snapshot_t snapshot = get_snapshot();
  double JD = JD_timestamp_pretty_good_000();

  return RA_and_Dec( snapshot, JD, temperature, pressure );
}

CAA2DCoordinate Simple_Altazimuth_Scope::RA_and_Dec(Alt_Azi_Snapshot_t snapshot, double JD, float temperature, float pressure){
  CAA2DCoordinate ra_and_dec;
  // FUCKTATION HERE?
  double longitude = get_backup_domain_longitude().to_double();
  double latitude = get_backup_domain_latitude().to_double();

  CAA2Dfloat azi_alt = topocentric_Azi_and_Alt( snapshot );
  double azimuth = azi_alt.X;
  double altitude = azi_alt.Y;

  ra_and_dec = horizontal_equatorial::RA_and_Dec( azimuth,
						  altitude,
						  longitude,
						  latitude,
						  JD,
						  temperature, pressure);
  return ra_and_dec;
}

CAA3Dfloat Simple_Altazimuth_Scope::topo_to_tele( CAA3Dfloat temp ){
  from_topocentric_transform( temp.X, temp.Y, temp.Z ); 
  return temp;
}

CAA3Dfloat Simple_Altazimuth_Scope::tele_to_topo( CAA3Dfloat temp ){
  to_topocentric_transform( temp.X, temp.Y, temp.Z ); 
  return temp;
}

void Simple_Altazimuth_Scope::from_topocentric_transform(float& x,float& y,float& z){
  /* This ugly code is the price paid for uncoupling the linear algebra package. */
  float X = x;
  float Y = y;
  float Z = z;
  x = topocentric_to_telescope_matrix[0]*X +
    topocentric_to_telescope_matrix[1]*Y +
    topocentric_to_telescope_matrix[2]*Z;

  y = topocentric_to_telescope_matrix[3]*X +
    topocentric_to_telescope_matrix[4]*Y +
    topocentric_to_telescope_matrix[5]*Z;

  z = topocentric_to_telescope_matrix[6]*X +
    topocentric_to_telescope_matrix[7]*Y +
    topocentric_to_telescope_matrix[8]*Z;
}

void Simple_Altazimuth_Scope::to_topocentric_transform(float& x,float& y,float& z){
  /* This ugly code is the price paid for uncoupling the linear algebra package. */
  float X = x;
  float Y = y;
  float Z = z;
  x = topocentric_to_telescope_matrix[0]*X +
    topocentric_to_telescope_matrix[3]*Y +
    topocentric_to_telescope_matrix[6]*Z;

  y = topocentric_to_telescope_matrix[1]*X +
    topocentric_to_telescope_matrix[4]*Y +
    topocentric_to_telescope_matrix[7]*Z;

  z = topocentric_to_telescope_matrix[2]*X +
    topocentric_to_telescope_matrix[5]*Y +
    topocentric_to_telescope_matrix[8]*Z;
}




