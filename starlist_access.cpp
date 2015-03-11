#include "starlist_access.h"
#include "starlist.h"
#include "solar_system.h"
#include <cmath>
#include <AACoordinateTransformation.h>

int starlist_access::get_index( int bsc_number ){
  int index = -1;
  for (int i = 0; i < STARLIST_SIZE; ++i) {
    if (starlist[i].BSCnum == bsc_number) {
      index = i;
      break;
    }
  }
  return index;
}

float starlist_access::magnitude( int index ){
  return starlist[index].magnitude;
}

int32_t starlist_access::bsc_number( int index ){
  return starlist[index].BSCnum;
}

CAA2DCoordinate starlist_access::proper_motion_adjusted_position( int index, double JD){
  CAA2DCoordinate ra_dec;
  sexagesimal::Sexagesimal RA;
  RA.set_binary_data(starlist[index].RAdata);
  ra_dec.X = RA.to_double();
  sexagesimal::Sexagesimal DEC;
  DEC.set_binary_data(starlist[index].DECdata);
  ra_dec.Y = DEC.to_double();
  /* Bright Star Catalog proper motions are arc seconds per year. */
  double days = JD - solar_system::J2000_0;
  double years = days / 365.25;
  double DECrad =
      CAACoordinateTransformation::DegreesToRadians(DEC.to_double());
  double cos_factor = cos(DECrad);

  if (cos_factor != 0.0) { /* This approach fails at the poles.  */
    ra_dec.X += starlist[index].RApm * years * (24.0 / 360.0) * (1.0 / 3600.0) / cos_factor;
    ra_dec.Y += starlist[index].DECpm * years * (1.0 / 3600.0);
  }
  return ra_dec;

}

int32_t starlist_access::starlist_size(){
  return STARLIST_SIZE;
}
