#include "telescope_model.h"
#include "quadrature_decoder.h"
#include "AACoordinateTransformation.h"
#include "topocentric_unit_vectors.h"
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

Simple_Altazimuth_Scope::Simple_Altazimuth_Scope(std::unique_ptr <
						 Quadrature_Decoder > azi,
						 std::unique_ptr <
						 Quadrature_Decoder > alt)
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
  if (binary_tidbits::zero_azimuth_is_north()) {
    azimuth_offset = 0.0;
  } else {
    azimuth_offset = 180.0;
  }
  altitude_offset = 0.0;
  reverse_azimuth_direction = false;
  reverse_altitude_direction = false;
}

Alt_Azi_Snapshot_t Simple_Altazimuth_Scope::get_snapshot()
{
  Alt_Azi_Snapshot_t d;
  d.alt_value = altitude_hardware->get_count();
  d.azi_value = azimuth_hardware->get_count();
  return d;
}

/* When attempting a more abstact telescope model, this seemed important. 
 * This is actually used by the optimizing algorithm.
*/
const CAA3DCoordinate Simple_Altazimuth_Scope::
altitude_forward_backward_difference(Alt_Azi_Snapshot_t aa)
{
  Alt_Azi_Snapshot_t temp;
  temp.alt_value = aa.alt_value + 1;
  temp.azi_value = aa.azi_value;
  CAA3DCoordinate forward = calculate_mount_frame_unit_vector(temp);
  temp.alt_value = aa.alt_value - 1;	/* Since this is now a signed interger, underflow is not an issue. */
  temp.azi_value = aa.azi_value;
  CAA3DCoordinate aftward = calculate_mount_frame_unit_vector(temp);
  CAA3DCoordinate diff;
  diff.X = forward.X - aftward.X;
  diff.Y = forward.Y - aftward.Y;
  diff.Z = forward.Z - aftward.Z;
  return diff;
}

/* 
 * This is in the telescope mount body frame.   
 * For example the base board of a Dobsonian.
*/
const CAA3DCoordinate Simple_Altazimuth_Scope::
calculate_mount_frame_unit_vector(Alt_Azi_Snapshot_t aa)
{
  CAA2DCoordinate azi_alt;
  azi_alt.X = this->azimuth_degrees(aa.azi_value);
  azi_alt.Y = this->altitude_degrees(aa.alt_value);
  /* We are not in the topcentric frame, but the geometry is ths same. */
  return topocentric_unit_vectors::AziAlt_to_UV(azi_alt);
}

/* 
 * This is in the telescope mount body frame.   
*/
Alt_Azi_Snapshot_t Simple_Altazimuth_Scope::
calculate_target_snapshot(CAA3DCoordinate uv)
{
  Alt_Azi_Snapshot_t encoder_target;
  /* Again, we are not in the topcentric frame, but the geometry is ths same. */
  CAA2DCoordinate azi_alt = topocentric_unit_vectors::UV_to_AziAlt(uv);

  double altitude_tics = this->altitude_encoder_value(azi_alt.Y);
  double azimuth_tics = this->azimuth_encoder_value(azi_alt.X);

  encoder_target.azi_value = static_cast < int >(round(azimuth_tics));
  encoder_target.alt_value = static_cast < int >(round(altitude_tics));

  return encoder_target;
}

const double Simple_Altazimuth_Scope::altitude_degrees()
{
  return altitude_degrees(altitude_hardware->get_count());
}

const double Simple_Altazimuth_Scope::azimuth_degrees()
{
  return azimuth_degrees(azimuth_hardware->get_count());
}

const double Simple_Altazimuth_Scope::altitude_degrees(int32_t encoder_value)
{
  double altitude_ticks;
  if (reverse_altitude_direction) {
    altitude_ticks = -static_cast < double >(encoder_value);
  } else {
    altitude_ticks = static_cast < double >(encoder_value);
  }
  double modulus =
      static_cast < double >(altitude_hardware->get_ticks_per_revolution());
  double altitude_deg = (altitude_ticks * 360.0) / modulus;
  altitude_deg -= altitude_offset;
  return altitude_deg;
}

/* 
 * Except that the return value is a  double, this is the inverse of 
 * Simple_Altazimuth_Scope::altitude_degrees( uint32_t encoder_value )
 * 
 */
const double Simple_Altazimuth_Scope::
altitude_encoder_value(double altitude_degrees)
{
  //  altitude_degrees = CAACoordinateTransformation::MapTo0To360Range( altitude_degrees );
  altitude_degrees += altitude_offset;
  double modulus =
      static_cast < double >(altitude_hardware->get_ticks_per_revolution());
  double altitude_ticks = (altitude_degrees * modulus) / 360.0;
  if (reverse_altitude_direction) {
    altitude_ticks = (-altitude_ticks);
  } else {
    // do nothing
  }
  return altitude_ticks;;
}

/*
  DO NOT TAMPER WITH THIS FUNCTION  UNTIL THE EQUATORIAL COORDINATE VIEW HAS BEEN DEGUGGED.  dan -Dec 2014
  As the scope is wired, this returns near zero sligly west of north, near 90 to the west.
 */
const double Simple_Altazimuth_Scope::azimuth_degrees(int32_t encoder_value)
{
  double azimuth;
  if (reverse_azimuth_direction) {
    azimuth = -static_cast < double >(encoder_value);
  } else {
    azimuth = static_cast < double >(encoder_value);
  }
  azimuth = (360.0 * azimuth) / azimuth_hardware->get_ticks_per_revolution();
  azimuth -= azimuth_offset;
  assert_param(azimuth < 360.0 * 8);
  assert_param(azimuth > -360.0 * 8);
  azimuth = CAACoordinateTransformation::MapTo0To360Range(azimuth);
  return azimuth;
}

const double Simple_Altazimuth_Scope::
azimuth_encoder_value(double azimuth_degrees)
{
  // azimuth_degrees = CAACoordinateTransformation::MapTo0To360Range( azimuth_degrees );
  azimuth_degrees += azimuth_offset;
  double modulus =
      static_cast < double >(azimuth_hardware->get_ticks_per_revolution());
  double encoder_value = azimuth_degrees * modulus / 360.0;
  if (reverse_azimuth_direction) {
    encoder_value = -encoder_value;
  } else {
    // do nothing
  }
  while (encoder_value > modulus) {
    encoder_value -= modulus;
  }
  while (encoder_value < 0) {
    encoder_value += modulus;
  }
  return encoder_value;
}

/********** Only Use 
const CAA3DCoordinate Simple_Altazimuth_Scope::calculate_mount_frame_unit_vector( Alt_Azi_Snapshot_t aa )
******************************************/

CAA2DCoordinate Simple_Altazimuth_Scope::topocentric_Azi_and_Alt(Alt_Azi_Snapshot_t snapshot)
{

  CAA3DCoordinate xyz_telescope = calculate_mount_frame_unit_vector(snapshot);
  CAA3DCoordinate xyz_topocentric = tele_to_topo(xyz_telescope);
  return topocentric_unit_vectors::UV_to_AziAlt(xyz_topocentric);
}

CAA2DCoordinate Simple_Altazimuth_Scope::current_topocentric_Azi_and_Alt()
{
  Alt_Azi_Snapshot_t snapshot = get_snapshot();
  return topocentric_Azi_and_Alt(snapshot);
}

CAA2DCoordinate Simple_Altazimuth_Scope::current_RA_and_Dec()
{
  /* @TODO replace these constants with globally available function call retrieving user setable values. */
  float pressure = refraction_temperature_pressure::DEFAULT_PRESSURE;
  float temperature = refraction_temperature_pressure::DEFAULT_TEMPERATURE;

  Alt_Azi_Snapshot_t snapshot = get_snapshot();
  double JD = JD_timestamp_pretty_good_000();

  return RA_and_Dec(snapshot, JD, temperature, pressure);
}

/********** Only Use 
const CAA3DCoordinate Simple_Altazimuth_Scope::calculate_mount_frame_unit_vector( Alt_Azi_Snapshot_t aa )
******************************************/

CAA2DCoordinate Simple_Altazimuth_Scope::RA_and_Dec(Alt_Azi_Snapshot_t snapshot,
						    double JD,
						    float temperature,
						    float pressure)
{
  CAA2DCoordinate ra_and_dec;
  double longitude = get_backup_domain_longitude().to_double();
  double latitude = get_backup_domain_latitude().to_double();

  CAA2DCoordinate azi_alt = topocentric_Azi_and_Alt(snapshot);
  double azimuth = azi_alt.X;
  double altitude = azi_alt.Y;

  ra_and_dec = horizontal_equatorial::RA_and_Dec(azimuth,
						 altitude,
						 longitude,
						 latitude,
						 JD, temperature, pressure);
  return ra_and_dec;
}

/*************************************
 * Dangerous Stuff
 *************************************/
void Simple_Altazimuth_Scope::increment_azimuth_encoder(){
  uint32_t modulus = azimuth_hardware->get_ticks_per_revolution();
  int32_t count = azimuth_hardware->get_count();
  count = (count + 1) % modulus;
  azimuth_hardware->set_count( count );
}

void Simple_Altazimuth_Scope::decrement_azimuth_encoder(){
  uint32_t modulus = azimuth_hardware->get_ticks_per_revolution();
  int32_t count = azimuth_hardware->get_count();
  count = (count - 1) % modulus;
  azimuth_hardware->set_count( count );
}


void Simple_Altazimuth_Scope::increment_altitude_encoder(){
  uint32_t modulus = altitude_hardware->get_ticks_per_revolution();
  int32_t count = altitude_hardware->get_count();
  count = (count + 1) % modulus;
  altitude_hardware->set_count( count );
}

void Simple_Altazimuth_Scope::decrement_altitude_encoder(){
  uint32_t modulus = altitude_hardware->get_ticks_per_revolution();
  int32_t count = altitude_hardware->get_count();
  count = (count - 1) % modulus;
  altitude_hardware->set_count( count );
}

/******************************
 * Boring setters and getters
 ****************************/


const double Simple_Altazimuth_Scope::get_altitude_offset()
{
  return altitude_offset;
}

const double Simple_Altazimuth_Scope::get_azimuth_offset()
{
  return azimuth_offset;
}

void Simple_Altazimuth_Scope::set_altitude_offset(double d)
{
  altitude_offset = d;
}

void Simple_Altazimuth_Scope::set_azimuth_offset(double d)
{
  azimuth_offset = d;
}

void Simple_Altazimuth_Scope::invert_altitude_direction()
{
  reverse_altitude_direction = !reverse_altitude_direction;
}

void Simple_Altazimuth_Scope::invert_azimuth_direction()
{
  reverse_azimuth_direction = !reverse_azimuth_direction;
}

/* True if increasing encoder count returns decreasing altitude. */
const bool Simple_Altazimuth_Scope::altitude_direction_is_reversed()
{
  return reverse_altitude_direction;
}

const bool Simple_Altazimuth_Scope::azimuth_direction_is_reversed()
{
  return reverse_azimuth_direction;
}

void Simple_Altazimuth_Scope::set_top_to_tel_matrix(double *src)
{
  for (uint32_t i = 0; i < 9; ++i) {
    topocentric_to_telescope_matrix[i] = src[i];
  }
}

void Simple_Altazimuth_Scope::set_determinant(double d)
{
  determinant = d;
}

double Simple_Altazimuth_Scope::get_determinant()
{
  return determinant;
}

void Simple_Altazimuth_Scope::set_align_error_mean(double e)
{
  align_error_mean = e;
}

void Simple_Altazimuth_Scope::set_align_error_max(double e)
{
  align_error_max = e;
}

double Simple_Altazimuth_Scope::get_align_error_mean()
{
  return align_error_mean;
}

double Simple_Altazimuth_Scope::get_align_error_max()
{
  return align_error_max;
}

int32_t Simple_Altazimuth_Scope::get_azimuth_ticks_per_revolution()
{
  return azimuth_hardware->get_ticks_per_revolution();
}

int32_t Simple_Altazimuth_Scope::get_altitude_ticks_per_revolution()
{
  return altitude_hardware->get_ticks_per_revolution();
}

/*****************************
 *  Transfomation between topocentric frame and telescope mount frame.
 *  Implementation is matrix multiplication.
 ******************************/


CAA3DCoordinate Simple_Altazimuth_Scope::topo_to_tele(CAA3DCoordinate temp)
{
  from_topocentric_transform(temp.X, temp.Y, temp.Z);
  return temp;
}

CAA3DCoordinate Simple_Altazimuth_Scope::tele_to_topo(CAA3DCoordinate temp)
{
  to_topocentric_transform(temp.X, temp.Y, temp.Z);
  return temp;
}

void Simple_Altazimuth_Scope::from_topocentric_transform(double &x, double &y,
							 double &z)
{
  /* This ugly code is the price paid for uncoupling the linear algebra package. */
  double X = x;
  double Y = y;
  double Z = z;
  x = topocentric_to_telescope_matrix[0] * X +
      topocentric_to_telescope_matrix[1] * Y +
      topocentric_to_telescope_matrix[2] * Z;

  y = topocentric_to_telescope_matrix[3] * X +
      topocentric_to_telescope_matrix[4] * Y +
      topocentric_to_telescope_matrix[5] * Z;

  z = topocentric_to_telescope_matrix[6] * X +
      topocentric_to_telescope_matrix[7] * Y +
      topocentric_to_telescope_matrix[8] * Z;
}

void Simple_Altazimuth_Scope::to_topocentric_transform(double &x, double &y,
						       double &z)
{
  /* This ugly code is the price paid for uncoupling the linear algebra package. */
  double X = x;
  double Y = y;
  double Z = z;
  x = topocentric_to_telescope_matrix[0] * X +
      topocentric_to_telescope_matrix[3] * Y +
      topocentric_to_telescope_matrix[6] * Z;

  y = topocentric_to_telescope_matrix[1] * X +
      topocentric_to_telescope_matrix[4] * Y +
      topocentric_to_telescope_matrix[7] * Z;

  z = topocentric_to_telescope_matrix[2] * X +
      topocentric_to_telescope_matrix[5] * Y +
      topocentric_to_telescope_matrix[8] * Z;
}
