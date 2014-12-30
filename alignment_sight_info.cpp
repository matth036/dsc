#include "alignment_sight_info.h"
#include "navigation_star.h"
// For some reason, navigation_star::nav_star_RA_Dec(int,double) is declared here:
#include "extra_solar_transforms.h"
#include "solar_system.h"
#include "stm32f4xx.h" /* For assert_param() */
#include "AACoordinateTransformation.h"
#include <cmath>
#include "binary_tidbits.h"
#include "AASidereal.h"
#include "AARefraction.h"
#include "refraction_temperature_pressure.h"

#include "horizontal_equatorial.h"
 
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
  temperature = DEFAULT_TEMPERATURE;
  pressure = DEFAULT_PRESSURE;
  // object_set = true;
  // pointing_set = false;
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
 * Fly what you test, test what you fly.
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
    /* MOON STUFF.  The handling of the moon is botched in other parts of
       this project.  Test to see if this is correct.  
       Test against Kstars. December 2014.
     */
    CAA3DCoordinate RA_Dec_Dist = solar_system::calculate_moon_RA_Dec_Dist(JD);
    RA_and_DEC.X = RA_Dec_Dist.X;
    RA_and_DEC.Y = RA_Dec_Dist.Y;
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
  return telescope->calculate_unit_vector( data );
}

const float Alignment_Data_Set::get_pressure( uint32_t item ){
  assert_param( item < size() );
  return sights[item].get_pressure();
}

const float Alignment_Data_Set::get_temperature( uint32_t item ){
  assert_param( item < size() );
  return sights[item].get_temperature();
}

const CAA2DCoordinate Alignment_Data_Set::azimuth_altitude( CAA2DCoordinate RA_and_Dec, 
						      double jd, 
						      float pressure, 
						      float temperature )
{
  CAA2DCoordinate new_answer;
  bool use_new_answer = false;
  double latitude = get_latitude();
  double longitude = get_longitude();

  new_answer = horizontal_equatorial::Azi_and_Alt( RA_and_Dec.X,
						   RA_and_Dec.Y,
						   longitude,
						   latitude,
						   jd,
						   temperature, pressure );





#if 1
  /* @TODO Fix this. This is spagetti code. Use calls to testable small units.  */

  double sidereal_time = CAASidereal::ApparentGreenwichSiderealTime( jd );
  double GHA_Aries = sidereal_time * 15.0;   /* Greenwhich Hour Angle of Aries. */
  /* 
   * alpha (Meeus ch.13) == SHA, Siderial Hour Angle for navigators.
   *   double RA = (360 - SHA_object)/15.0;
   */
  double SHA_object = 360.0 - RA_and_Dec.X * 15.0; /* Hours to Degrees conversion, direction reversed. */
  double GHA_object = GHA_Aries + SHA_object;
  double LHA; /* Local Hour Angle.  H in Meeus. */
  if( binary_tidbits::west_longitude_is_positive() ){
    LHA = GHA_object - longitude;
  }else{
    LHA = GHA_object + longitude;
  }

  double delta = RA_and_Dec.Y;
  //  static CAA2DCoordinate Equatorial2Horizontal(double LocalHourAngle, double Delta, double Latitude);
  CAA2DCoordinate Azi_Alt = CAACoordinateTransformation::Equatorial2Horizontal( LHA/15.0, delta, latitude );
  /* See text after equations 13.5 and 13.6 Meeus pp.93. */
  if( binary_tidbits::zero_azimuth_is_north() ){
    Azi_Alt.X += 180.0;
  }else{

  }
  /*
    Apply atmospheric refraction to the altitude at this point of the calculation.
    RefractionFromTrue(double Altitude, double Pressure = 1010, double Temperature = 10);
    This value is in degrees and the apparent position is higher than the true position.
  */
  double R = CAARefraction::RefractionFromTrue( Azi_Alt.Y, DEFAULT_PRESSURE, DEFAULT_TEMPERATURE  );
  Azi_Alt.Y += R;

#endif
  if( use_new_answer ){
    return new_answer;
  }else{
    return Azi_Alt;
  }
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
							     float pressure=Alignment_Sight_Item::DEFAULT_PRESSURE, 
							     float temperature=Alignment_Sight_Item::DEFAULT_TEMPERATURE
							     )
{
  CAA3DCoordinate uv_xyz;
  CAA2DCoordinate Azi_Alt = azimuth_altitude( RA_and_Dec, 
					      jd, 
					      pressure, 
					      temperature );




/*  OLD CODE */ 					      
#if 1 
  /* convert to radians. */
  Azi_Alt.X = CAACoordinateTransformation::DegreesToRadians( Azi_Alt.X );
  Azi_Alt.Y = CAACoordinateTransformation::DegreesToRadians( Azi_Alt.Y );
  /* Only the comments differ in this conditional.  */
  if( binary_tidbits::zero_azimuth_is_north() ){
    uv_xyz.Z = sin( Azi_Alt.Y );         /* Component toward the zenith. */
    double c = cos( Azi_Alt.Y );
    uv_xyz.X = c * cos( Azi_Alt.X );     /* Component toward North. */
    uv_xyz.Y = - c * sin( Azi_Alt.X );   /* Component toward West.  */
  }else{
    uv_xyz.Z = sin( Azi_Alt.Y );         /* Component toward the zenith. */
    double c = cos( Azi_Alt.Y );
    uv_xyz.X = c * cos( Azi_Alt.X );     /* Component toward South. */
    uv_xyz.Y = - c * sin( Azi_Alt.X );   /* Component toward East.  */
  }
  return uv_xyz;
#endif
}

