#include "alignment_sight_info.h"
#include "navigation_star.h"
// For some reason, navigation_star::nav_star_RA_Dec(int,double) is declared here:
#include "extra_solar_transforms.h"
#include "solar_system.h"

#include "re_assert.h"
// #include "stm32f4xx.h" /* For assert_param() */
#include "AACoordinateTransformation.h"
#include <cmath>
#include "binary_tidbits.h"
#include "AASidereal.h"
#include "AARefraction.h"
#include "refraction_temperature_pressure.h"

#include "horizontal_equatorial.h"
#include "horizontal_parallax.h"
#include "topocentric_unit_vectors.h"
#include "lbr_and_xyz.h" 
#include "rtc_management.h"

/***********
template<uint32_t N_axis> struct Encoder_Data_Set{
public:
  uint32_t data[N_axis];
};
*******************/

/***************************
template<typename INT_T, size_t N> struct Encoder_Snapshot{
public:
  INT_T& operator[](size_t i){
    return data[i];
  }
  INT_T data[N];
};

typedef Encoder_Snapshot<uint32_t,2> Encoder2_t;
******************************/

/**
uint32_t& template<uint32_t N>Encoder_Snapshot::operator[](size_t i){
  return data[i]&;
}
**/


/* @TODO when funnction calls for temperature and pressure are available, use them. */
Alignment_Sight_Item::Alignment_Sight_Item(std::string object){
  object_name = object;
  temperature = refraction_temperature_pressure::DEFAULT_TEMPERATURE;
  pressure = refraction_temperature_pressure::DEFAULT_PRESSURE;
}

/* alignment_prompt (scope=0x2001ff28, object=<incomplete type>) at controller.cpp:417 */
void Alignment_Sight_Item::set_pointing_information( double jd, Alt_Azi_Snapshot_t encoder_data ){
  JD = jd;   /* Break here. */
  encoder_values = encoder_data;
}
 
void Alignment_Sight_Item::set_temperature( float t ){
  temperature = t;
}

void Alignment_Sight_Item::set_pressure( float p ){
  pressure = p;
}

void Alignment_Sight_Item::set_object_name( std::string& object ){
  object_name = object;
  // object_set = false;
}

const double Alignment_Sight_Item::get_julian_date(){
  return JD;
}

const float Alignment_Sight_Item::get_temperature(){
  return temperature;
}

const float Alignment_Sight_Item::get_pressure(){
  return pressure;
}

const std::string Alignment_Sight_Item::get_object_name(){
  return object_name;
}

const Alt_Azi_Snapshot_t Alignment_Sight_Item::get_encoder_data(){
  return encoder_values;
}

/* 
 * A well tested final project is possible only if
 * this routine uses only those 
 * computational functions employed in 
 * ./test/nautical_almanac.cpp. 
 * 
 **/
CAA2DCoordinate calulate_RA_and_Dec(std::string object, double JD, bool& success){
  success = false;
  CAA2DCoordinate RA_and_DEC;
  RA_and_DEC.X = 0;
  RA_and_DEC.Y = 0;
  for( uint32_t num = 0; num<=navigation_star::NAVIGATION_LIST_MAX; ++num ){
    if( object == navigation_star::get_navigation_star_name(num) ){
      RA_and_DEC = navigation_star::nav_star_RA_Dec(num, JD);
      success = true;
      return RA_and_DEC;
    }
  }
  if( object == "Moon" ){
    /* 
       Fixed but not tested. April 2015.

       MOON STUFF.  The handling of the moon is botched in other parts of
       this project.  Test to see if this is correct.  
       Test against Kstars. December 2014.

       I have made no correction for horizontal parallax.
     */
    CAA3DCoordinate RA_Dec_Dist = solar_system::calculate_moon_RA_Dec_Dist(JD);
    if( rtc_have_longitude()  && rtc_have_latitude() ){
      sexagesimal::Sexagesimal longitude = get_backup_domain_longitude();
      sexagesimal::Sexagesimal latitude = get_backup_domain_latitude();
      double altitude = 0.0;
      RA_and_DEC = Equatorial2TopocentricRigorousAlternative(RA_Dec_Dist.X, RA_Dec_Dist.Y, RA_Dec_Dist.Z/solar_system::AU_kilometers,
							     longitude.to_double(), latitude.to_double(), altitude,
							     JD);

    }else{
      RA_and_DEC.X = RA_Dec_Dist.X;
      RA_and_DEC.Y = RA_Dec_Dist.Y;
    }
    success = true;
    return RA_and_DEC;
  }
  if( object == "Sun" ||
      object == "Mercury" || 
      object == "Venus" ||
      object == "Mars" ||
      object == "Jupiter" || 
      object == "Saturn" ||
      object == "Uranus" ||
      object == "Neptune" ){
    CAAEllipticalPlanetaryDetails details =
      solar_system::calculate_details(object, JD);
    RA_and_DEC.X = details.ApparentGeocentricRA;
    RA_and_DEC.Y = details.ApparentGeocentricDeclination;
    success = true;
    return RA_and_DEC;
  }
  success = false;
  return RA_and_DEC;
}


/******************************************
 * Alignment_Data_Set methods
 ********************************************/

Alignment_Data_Set::Alignment_Data_Set( Simple_Altazimuth_Scope* tele ){
  telescope = tele;
  _has_long_and_lat = false;
}

void Alignment_Data_Set::push_item(Alignment_Sight_Item& item){
  /* Check for unset fields. */
  assert_param( item.get_julian_date() > 0.0 );
  sights.push_back( item );
}

void Alignment_Data_Set::clear(){
  while( sights.size() > 0 ){
    sights.pop_back();
  }
}

void Alignment_Data_Set::delete_item( uint32_t position ){
  for( uint32_t i = position; i+1<sights.size(); ++i ){
    sights[i] = sights[i+1];
  }
  sights.pop_back();
}

const double Alignment_Data_Set::get_longitude(){
  return longitude.to_double();
}

const double Alignment_Data_Set::get_latitude(){
  return latitude.to_double();
}

void Alignment_Data_Set::set_longitude_and_latitude(sexagesimal::Sexagesimal lo,
						    sexagesimal::Sexagesimal la)
{
  longitude = lo;
  latitude = la;
}

void Alignment_Data_Set::set_telescope_model( Simple_Altazimuth_Scope* t ){
  /* This the telescope model is set by the constructor. */
  assert_param( false ); 
  telescope = t;
}

Simple_Altazimuth_Scope* Alignment_Data_Set::get_telescope_model(){
  return telescope;
}

const size_t Alignment_Data_Set::size(){
  return sights.size();
}

const bool Alignment_Data_Set::has_longitude_and_latitude(){
  return _has_long_and_lat;
}

const std::string Alignment_Data_Set::get_object_name( uint32_t item ){
  if( item < size() ){
    return sights[item].get_object_name();
  }else{
    return "Out of range error";
  }
}

const double Alignment_Data_Set::get_julian_date( uint32_t item ){
  if( item < size() ){
    return sights[item].get_julian_date();
  }else{
    return 0.0;
  }
}

const Alt_Azi_Snapshot_t Alignment_Data_Set::get_encoder_data( uint32_t item ){
  assert_param( item < size() );
  Alt_Azi_Snapshot_t return_data = sights[item].get_encoder_data();
  return return_data;
}

const CAA3DCoordinate Alignment_Data_Set::telescope_frame_unit_vector( uint32_t item ){
  Alt_Azi_Snapshot_t data = get_encoder_data( item );
  return telescope->calculate_mount_frame_unit_vector( data );
}

const float Alignment_Data_Set::get_pressure( uint32_t item ){
  assert_param( item < size() );
  return sights[item].get_pressure();
}

const float Alignment_Data_Set::get_temperature( uint32_t item ){
  assert_param( item < size() );
  return sights[item].get_temperature();
}


/* Return a unit vector representing the topocentric direction to object n. 
 *
 * Under the zero_azimuth_is_north() convention,
 * X is North
 * Y is West
 * Z is towards the Zenith.
 *
 * If zero azimuth is South, 
 * X is South
 * Y is East
 * Z is towards the Zenith.
 *
 * @TODO implement default pressure and temperature.
 * @NOTA_BENE To turn off refraction, set pressure to zero. (See the astronomical algorithims code.)
 **/
const CAA3DCoordinate Alignment_Data_Set::topocentric_unit_vector( CAA2DCoordinate RA_and_Dec, 
								   double jd, 
								   float pressure=refraction_temperature_pressure::DEFAULT_PRESSURE,
								   float temperature=refraction_temperature_pressure::DEFAULT_TEMPERATURE
								   )
{
  CAA3DCoordinate uv_xyz;
  /* What Function? */ 
  /* These coordinates are in the topocentric frame  */
  CAA2DCoordinate Azi_Alt = azimuth_altitude( RA_and_Dec, 
					      jd, 
					      pressure, 
					      temperature );

  return topocentric_unit_vectors::AziAlt_to_UV(Azi_Alt);
}

const CAA2DCoordinate Alignment_Data_Set::azimuth_altitude( CAA2DCoordinate RA_and_Dec, 
						      double jd, 
						      float pressure, 
						      float temperature )
{
  /* 
  *  The this-> constuct ephasizes that we are using the locally stored
  *  longitude and latitude. 
  */
  double latitude = this->get_latitude();
  double longitude = this->get_longitude();
  CAA2DCoordinate azimuth_and_altitude;
  azimuth_and_altitude = horizontal_equatorial::Azi_and_Alt( RA_and_Dec.X,
							     RA_and_Dec.Y,
							     longitude,
							     latitude,
							     jd,
							     temperature, pressure );

  /* The adjustment for 
   * binary_tidbits::zero_azimuth_is_north() 
   * has already been make in 
   * horizontal_equatorial::Azi_and_Alt( ... );
   */
  return azimuth_and_altitude;
}



