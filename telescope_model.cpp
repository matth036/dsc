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
#include "lbr_and_xyz.h"
#include "refraction_temperature_pressure.h"

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

/* When attempting a more abstact telescope model, this seemed important. */
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



Alt_Azi_Snapshot_t Simple_Altazimuth_Scope::calculate_target_snapshot(CAA3DCoordinate uv ){
  Alt_Azi_Snapshot_t encoder_target;  
  CAA3DCoordinate lbr = XYZ_to_LBR( uv ); /* How well has this been tested?  */
  lbr.X = -lbr.X;
  encoder_target.azi_value = static_cast<int>(floor( lbr.X*azimuth_hardware->get_ticks_per_revolution()/360.0 + .5 ));
  encoder_target.alt_value = static_cast<int>(floor( lbr.Y*altitude_hardware->get_ticks_per_revolution()/360.0 + .5 ));


  return encoder_target;
}

const double Simple_Altazimuth_Scope::altitude_degrees( ){
  return altitude_degrees( altitude_hardware->get_count() );
}

const double Simple_Altazimuth_Scope::azimuth_degrees( ){
  return azimuth_degrees( azimuth_hardware->get_count() );
}

const double Simple_Altazimuth_Scope::altitude_degrees( int32_t encoder_value ){
  double altitude_ticks;
  if( reverse_altitude_direction ){
    altitude_ticks = -static_cast<double>(encoder_value);
  }else{
    altitude_ticks = static_cast<double>(encoder_value);
  }
  double modulus = static_cast<double>( altitude_hardware->get_ticks_per_revolution() );
  double altitude_deg = (altitude_ticks *360.0) / modulus; 
  altitude_deg -= altitude_offset;
  return altitude_deg;
}

/* 
 * Inverse of 
 * Simple_Altazimuth_Scope::altitude_degrees( uint32_t encoder_value )
 * except the return value is a  double.
 */
const double Simple_Altazimuth_Scope::altitude_encoder_value( double altitude_degrees ){
  //  altitude_degrees = CAACoordinateTransformation::MapTo0To360Range( altitude_degrees );
  altitude_degrees += altitude_offset;
  double modulus = static_cast<double>( altitude_hardware->get_ticks_per_revolution() );
  double altitude_ticks = (altitude_degrees * modulus) / 360.0;
  if( reverse_altitude_direction ){
    altitude_ticks = (-altitude_ticks);
  }else{
    // do nothing
  }
  return altitude_ticks;;
}


/*
  DO NOT TAMPER WITH THIS FUNCTION  UNTIL THE EQUATORIAL COORDINATE VIEW HAS BEEN DEGUGGED.  dan -Dec 2014
  As the scope is wired, this returns near zero sligly west of north, near 90 to the west.
 */
const double Simple_Altazimuth_Scope::azimuth_degrees( int32_t encoder_value ){
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

const double Simple_Altazimuth_Scope::azimuth_encoder_value( double azimuth_degrees ){
  // azimuth_degrees = CAACoordinateTransformation::MapTo0To360Range( azimuth_degrees );
  azimuth_degrees += azimuth_offset;
  double modulus = static_cast<double>( azimuth_hardware->get_ticks_per_revolution() );
  double encoder_value = azimuth_degrees * modulus / 360.0;
  if( reverse_azimuth_direction ){
    encoder_value = -encoder_value;
  }else{
    // do nothing
  }
  while( encoder_value > modulus ){
    encoder_value -= modulus;
  }
  while( encoder_value < 0 ){
    encoder_value += modulus;
  }
  return encoder_value;
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
  for(uint32_t i=0; i<9; ++i ){
    topocentric_to_telescope_matrix[i] = src[i];
  }
}

void Simple_Altazimuth_Scope::set_determinant( double d ){
  determinant = d;
}

double Simple_Altazimuth_Scope::get_determinant( ){
  return determinant;
}

void Simple_Altazimuth_Scope::set_align_error_mean( double e ){
  align_error_mean = e;
}

void Simple_Altazimuth_Scope::set_align_error_max( double e ){
  align_error_max = e;
}

double Simple_Altazimuth_Scope::get_align_error_mean(){
  return align_error_mean;
}
double Simple_Altazimuth_Scope::get_align_error_max(){
  return align_error_max;
}

int32_t Simple_Altazimuth_Scope::get_azimuth_ticks_per_revolution(){
  return azimuth_hardware->get_ticks_per_revolution();
}

int32_t Simple_Altazimuth_Scope::get_altitude_ticks_per_revolution(){
  return altitude_hardware->get_ticks_per_revolution();
}




CAA2DCoordinate Simple_Altazimuth_Scope::topocentric_Azi_and_Alt(Alt_Azi_Snapshot_t snapshot){
  CAA2DCoordinate azi_and_alt;
  // FUCK ALERT
  CAA3DCoordinate lbr_telescope;
  lbr_telescope.X = 360.0 - azimuth_degrees( snapshot.azi_value );
  lbr_telescope.Y = altitude_degrees( snapshot.alt_value );
  lbr_telescope.Z = 1.0; /* unit vector. */
  CAA3DCoordinate xyz_telescope = LBR_to_XYZ( lbr_telescope );
  /* 
   * This transformation of xyz is unlikely to cause gross errors since the matrix
   * is close to the identity matrix.
   */
  CAA3DCoordinate xyz_topocentric = tele_to_topo( xyz_telescope );
  CAA3DCoordinate lbr_topocentric = XYZ_to_LBR( xyz_topocentric );
  azi_and_alt.X = 360.0 - lbr_topocentric.X;
  azi_and_alt.Y = lbr_topocentric.Y;

  CAA2DCoordinate snapshot_check = encoder_Azi_and_Alt( azi_and_alt );
  bool problem = false;
  double diff = static_cast<double>(snapshot.alt_value) - snapshot_check.Y;
  diff = diff - floor( diff );
  problem = ( diff > .0001 && diff < .999 );
  if( problem ){
    snapshot_check = encoder_Azi_and_Alt( azi_and_alt );
    return azi_and_alt;
  }
  snapshot_check = encoder_Azi_and_Alt( azi_and_alt ); // Not reached if there is not a problem.
  return azi_and_alt;
}
/*
 * This is intended to be a testable inversion of
 * CAA2DCoordinate Simple_Altazimuth_Scope::topocentric_Azi_and_Alt(Alt_Azi_Snapshot_t snapshot)
 *  Notice that all of the coordinate transformations take place here
 *  at the bottom the atmosphere so refraction, pressure, and temperature are not
 *  parameters.  
*/
CAA2DCoordinate Simple_Altazimuth_Scope::encoder_Azi_and_Alt(CAA2DCoordinate topocentric_azi_alt){
  CAA2DCoordinate encoder_azi_alt;

  CAA3DCoordinate lbr_topocentric;
  lbr_topocentric.X = 360.0 - topocentric_azi_alt.X;
  lbr_topocentric.Y = topocentric_azi_alt.Y;
  lbr_topocentric.Z = 1.0; /* unit vector. */
  CAA3DCoordinate xyz_topocentric = LBR_to_XYZ( lbr_topocentric );
  CAA3DCoordinate xyz_telescope = topo_to_tele( xyz_topocentric );
  CAA3DCoordinate lbr_telescope = XYZ_to_LBR( xyz_telescope );
  double azimuth_degrees = 360.0 - lbr_telescope.X;
  double altitude_degrees = lbr_telescope.Y;
  /* If debugging, check that lbr_telescope.Z is approximately 1.0 */

  encoder_azi_alt.X = azimuth_encoder_value( azimuth_degrees );
  encoder_azi_alt.Y = altitude_encoder_value( altitude_degrees );
  return encoder_azi_alt;
}

CAA2DCoordinate Simple_Altazimuth_Scope::current_topocentric_Azi_and_Alt(){
  Alt_Azi_Snapshot_t snapshot = get_snapshot();
  return topocentric_Azi_and_Alt( snapshot );
}

CAA2DCoordinate Simple_Altazimuth_Scope::current_RA_and_Dec(){
  /* @TODO replace these constants with globally available function call retrieving user setable values. */
  float pressure = DEFAULT_PRESSURE;
  float temperature = DEFAULT_TEMPERATURE;

  Alt_Azi_Snapshot_t snapshot = get_snapshot();
  double JD = JD_timestamp_pretty_good_000();

  return RA_and_Dec( snapshot, JD, temperature, pressure );
}

CAA2DCoordinate Simple_Altazimuth_Scope::RA_and_Dec(Alt_Azi_Snapshot_t snapshot, double JD, 
						    float temperature, float pressure){
  CAA2DCoordinate ra_and_dec;
  // FUCKTATION HERE?
  double longitude = get_backup_domain_longitude().to_double();
  double latitude = get_backup_domain_latitude().to_double();

  CAA2DCoordinate azi_alt = topocentric_Azi_and_Alt( snapshot );
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

CAA2DCoordinate Simple_Altazimuth_Scope::Encoder_Alt_Azi(CAA2DCoordinate RA_and_Dec, double JD,
							 float temperature, float pressure){
  CAA2DCoordinate alt_and_azi;

  return alt_and_azi;
}


CAA3DCoordinate Simple_Altazimuth_Scope::topo_to_tele( CAA3DCoordinate temp ){
  from_topocentric_transform( temp.X, temp.Y, temp.Z ); 
  return temp;
}

CAA3DCoordinate Simple_Altazimuth_Scope::tele_to_topo( CAA3DCoordinate temp ){
  to_topocentric_transform( temp.X, temp.Y, temp.Z ); 
  return temp;
}

void Simple_Altazimuth_Scope::from_topocentric_transform(double& x,double& y,double& z){
  /* This ugly code is the price paid for uncoupling the linear algebra package. */
  double X = x;
  double Y = y;
  double Z = z;
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

void Simple_Altazimuth_Scope::to_topocentric_transform(double& x,double& y,double& z){
  /* This ugly code is the price paid for uncoupling the linear algebra package. */
  double X = x;
  double Y = y;
  double Z = z;
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




