#include "navigation_star.h"
#include "extra_solar_transforms.h"
#include "solar_system.h"
#include <cmath>
#include "AACoordinateTransformation.h"
#include "AAPrecession.h"
#include "AAAberration.h"
#include "AANutation.h"
#include "AADynamicalTime.h"
#include "neo_bsc_starlist.h"
#include "neo_whiz_bang_ngc_list.h"
#include <algorithm>

inline uint32_t get_bsc_number( uint32_t index ){
  return flash_memory_array::bsc_array[index].BSCnum;
}


int32_t extra_solar_bsc::neo_get_index_fast( const int32_t bsc_number ){
  int32_t lo = 0;
  int32_t hi = flash_memory_array::bsc_array.size() - 1;
  while( lo <= hi ){
    int32_t mid = (hi+lo)/2;
    int32_t mid_bsc = get_bsc_number( mid );
    if( mid_bsc > bsc_number ){
      hi = mid - 1;
    }else if( mid_bsc < bsc_number ){
      lo = mid + 1;
    }else if( bsc_number == mid_bsc ){
      return mid;
    }
  }
  return -1; // Not found.
}

int32_t extra_solar_bsc::neo_get_index( uint32_t bsc_number ){
  for( uint32_t i=0; i<flash_memory_array::bsc_array.size(); ++i ){
    if( flash_memory_array::bsc_array[i].BSCnum == static_cast<int32_t>(bsc_number) ){
      return static_cast<int32_t>(i);
    }
  }
  return -1;
}


int32_t extra_solar_ngc::neo_get_index( uint32_t ngc_number ){
  for( uint32_t i=0; i<flash_memory_array::ngc_list.size(); ++i ){
    if( flash_memory_array::ngc_list[i].NGC_number == ngc_number ){
      return static_cast<int32_t>(i);
    }
  }
  return -1;
}

double extra_solar_ngc::neo_get_RA_i(uint32_t index){
  sexagesimal::Sexagesimal RA;
  RA.set_binary_data( flash_memory_array::ngc_list[index].RA_data );
  return RA.to_double();
}

double extra_solar_ngc::neo_get_Dec_i(uint32_t index){
  sexagesimal::Sexagesimal DEC;
  DEC.set_binary_data( flash_memory_array::ngc_list[index].DEC_data );
  return DEC.to_double();
}





int32_t extra_solar_ngc::neo_get_index_fast( int32_t ngc_number ){
  // @FIXME really a implement fast implementation.
  return extra_solar_ngc::neo_get_index( static_cast<uint32_t>(ngc_number) );
}






CAA2DCoordinate extra_solar_bsc::proper_motion_adjusted_position(const star_data & sd,
						double JD)
{
  CAA2DCoordinate ra_dec;
  sexagesimal::Sexagesimal RA;
  RA.set_binary_data(sd.RAdata);
  ra_dec.X = RA.to_double();
  sexagesimal::Sexagesimal DEC;
  DEC.set_binary_data(sd.DECdata);
  ra_dec.Y = DEC.to_double();
  /* Bright Star Catalog proper motions are arc seconds per year. */
  double days = JD - solar_system::J2000_0;
  double years = days / 365.25;
  double DECrad =
      CAACoordinateTransformation::DegreesToRadians(DEC.to_double());
  double cos_factor = cos(DECrad);
  CAA2DCoordinate pm_arc_sec;
  pm_arc_sec.X = sd.RApm * years / 3600;
  pm_arc_sec.Y = sd.DECpm * years / 3600;
  if (cos_factor != 0.0) { /* This approach fails at the poles.  */
    ra_dec.X += sd.RApm * years * (24.0 / 360.0) * (1.0 / 3600.0) / cos_factor;
    ra_dec.Y += sd.DECpm * years * (1.0 / 3600.0);
  }
  return ra_dec;
}

/* 
 * Applicable to stars, deep space objects. 
 *
 * ra_dec should be J2000.0 coordinates.
 * JD should be corrected for dynamical time.
 *
 * This uses CAAAberration::EquatorialAberration(RA,Dec,JD) 
 */
CAA2DCoordinate apply_aberration(CAA2DCoordinate ra_dec, double JD)
{
  CAA2DCoordinate ra_dec_a;
  ra_dec_a.X = ra_dec.X;
  ra_dec_a.Y = ra_dec.Y;
  CAA2DCoordinate aberration_addon =
      CAAAberration::EquatorialAberration(ra_dec.X, ra_dec.Y, JD);
  ra_dec_a.X += aberration_addon.X;
  ra_dec_a.Y += aberration_addon.Y;
  return ra_dec_a;
}


CAA2DCoordinate precession_correct(CAA2DCoordinate RA_Dec_J2000, double JD)
{
  CAA2DCoordinate position;
  position =
      CAAPrecession::PrecessEquatorial(RA_Dec_J2000.X, RA_Dec_J2000.Y,
				       solar_system::J2000_0, JD);
  return position;
}


CAA2DCoordinate precession_correct(CAA2DCoordinate RA_Dec_J2000, double JD_0, double JD_1)
{
  CAA2DCoordinate position;
  position =
      CAAPrecession::PrecessEquatorial(RA_Dec_J2000.X, RA_Dec_J2000.Y, JD_0, JD_1);
  return position;
}




CAA2DCoordinate precession_and_nutation_correct(CAA2DCoordinate RA_Dec_J2000,
						double JD)
{
  CAA2DCoordinate position;
  double nil;
  double nio;
  double toe;
  double RA_nutation;
  double DEC_nutation;

  /* At the end, I add on a nutation correction.  
   * Should I subtract such a correction at the start?
   * If so, include this conditional code.
   *
   * Usually J2000 Coordinates are with respect to the mean eqinox so it
   * is incorrect tu un-apply nutation at the start of the calculation.
   *  
   * Consider eliminating this function to avoid erronious use.
   */
#if 1
  nil = CAANutation::NutationInLongitude(solar_system::J2000_0);	/* arc seconds. */
  nio = CAANutation::NutationInObliquity(solar_system::J2000_0);	/*  arc seconds */
  toe = CAANutation::TrueObliquityOfEcliptic(solar_system::J2000_0);	/*  Degrees */
  /* Will the compiler reconize that nil, nio, and toe as set above are constants?
   * They are return value of a static function with a constant argument */
  RA_nutation = CAANutation::NutationInRightAscension(RA_Dec_J2000.X,
						      RA_Dec_J2000.Y,
						      toe, nil, nio);
  DEC_nutation = CAANutation::NutationInDeclination(RA_Dec_J2000.X, toe, nil, nio);
  RA_Dec_J2000.X -= RA_nutation / 3600.0 / 15.0;
  RA_Dec_J2000.Y -= DEC_nutation / 3600.0;
#endif

  position =
      CAAPrecession::PrecessEquatorial(RA_Dec_J2000.X, RA_Dec_J2000.Y,
				       solar_system::J2000_0, JD);

  nil = CAANutation::NutationInLongitude(JD);	/* arc seconds. */
  nio = CAANutation::NutationInObliquity(JD);	/*  arc seconds */
  toe = CAANutation::TrueObliquityOfEcliptic(JD);	/*  Degrees */
  RA_nutation = CAANutation::NutationInRightAscension(position.X,
							     position.Y,
							     toe,
							     nil,
							     nio);
  DEC_nutation = CAANutation::NutationInDeclination(position.X,
							   toe,
							   nil,
							   nio);
  /* RA_nutation and DEC_nutation are both in arc seconds. */

  position.X += RA_nutation / 3600.0 / 15.0;
  position.Y += DEC_nutation / 3600.0;

  return position;
}

/*
 *
 * I add on nutation at the end, so logicaly it should be subracted at the start. 
 * TODO see if the subtract off part is zero for epoch JD2000.
 * Done: These are not zero for JD2000.  See the test program nutation_info.
 *
 */
CAA2DCoordinate precession_and_nutation_correct(CAA2DCoordinate RA_Dec_0,
						double JD_0, double JD_1)
{
  double nil;
  double nio;
  double toe;
  double RA_nutation;
  double DEC_nutation;
  
  /* 
   * Calculate and subtract off nutation for JD_0.  Effectivly, undo an assumed present nutation correction.
   */
  nil = CAANutation::NutationInLongitude(JD_0);	/* arc seconds. */
  nio = CAANutation::NutationInObliquity(JD_0);	/*  arc seconds */
  toe = CAANutation::TrueObliquityOfEcliptic(JD_0);	/*  Degrees */
  RA_nutation = CAANutation::NutationInRightAscension(RA_Dec_0.X,
						      RA_Dec_0.Y,
						      toe, nil, nio);
  DEC_nutation = CAANutation::NutationInDeclination(RA_Dec_0.X, toe, nil, nio);
  RA_Dec_0.X -= RA_nutation / 3600.0 / 15.0;
  RA_Dec_0.Y -= DEC_nutation / 3600.0;

  CAA2DCoordinate position;

  position =
      CAAPrecession::PrecessEquatorial(RA_Dec_0.X, RA_Dec_0.Y, JD_0, JD_1);

  nil = CAANutation::NutationInLongitude(JD_1);	/*  arc seconds */
  nio = CAANutation::NutationInObliquity(JD_1);	/*  arc seconds */
  toe = CAANutation::TrueObliquityOfEcliptic(JD_1);	/*  Degrees */
  RA_nutation = CAANutation::NutationInRightAscension(position.X,
						      position.Y,
						      toe, nil, nio);
  DEC_nutation = CAANutation::NutationInDeclination(position.X, toe, nil, nio);
  /* RA_nutation and DEC_nutation are both in arc seconds. */
  position.X += RA_nutation / 3600.0 / 15.0;
  position.Y += DEC_nutation / 3600.0;

  return position;
}

/*
 * "Mean Equinox" coordinates have nutation "averaged out."
 * 
 *  So, do not un-apply nutation correction at the start of the calculation.
 *
 *  Any expression of a value that makes a decomposition ((Mean Value) + (Variations From Mean Value)).
 *  must somehow choose what the Mean Value is.  Evidently, the nutation expressions take the Mean Value
 *  to be nothing more than the precession described by the Rigorous method of Meeus, Chapter 21.
 *  
 *  One should not expect that (Variations From Mean Value) will average to zero.
 */
CAA2DCoordinate precession_and_nutation_correct_from_mean_eqinox(CAA2DCoordinate RA_Dec_0,
						double JD_0, double JD_1)
{
  double nil;
  double nio;
  double toe;
  double RA_nutation;
  double DEC_nutation;

  CAA2DCoordinate position;

  position =
      CAAPrecession::PrecessEquatorial(RA_Dec_0.X, RA_Dec_0.Y, JD_0, JD_1);

  nil = CAANutation::NutationInLongitude(JD_1);	/*  arc seconds */
  nio = CAANutation::NutationInObliquity(JD_1);	/*  arc seconds */
  toe = CAANutation::TrueObliquityOfEcliptic(JD_1);	/*  Degrees */
  RA_nutation = CAANutation::NutationInRightAscension(position.X,
						      position.Y,
						      toe, nil, nio);
  DEC_nutation = CAANutation::NutationInDeclination(position.X, toe, nil, nio);
  /* RA_nutation and DEC_nutation are both in arc seconds. */
  position.X += RA_nutation / 3600.0 / 15.0;
  position.Y += DEC_nutation / 3600.0;

  return position;
}

/* This is an example of what I believe to be correct and rigorous coordinate transformations. */
CAA2DCoordinate burnham_correct(CAA2DCoordinate RA_Dec, double JD ){
  RA_Dec = precession_correct( RA_Dec, solar_system::B1950_0, solar_system::J2000_0 );
  double deltaT = CAADynamicalTime::DeltaT( JD );
  RA_Dec = apply_aberration( RA_Dec, JD + deltaT/86400.0 );
  RA_Dec = precession_and_nutation_correct_from_mean_eqinox( RA_Dec, solar_system::J2000_0, JD );

  return RA_Dec;
}

/* Overide where initial Julian date is solar_system::J2000_0 = 2451545.0  */
CAA2DCoordinate precession_and_nutation_correct_from_mean_eqinox(CAA2DCoordinate RA_Dec_J2000,
						double JD)
{
#if 1
  return precession_and_nutation_correct_from_mean_eqinox( RA_Dec_J2000, solar_system::J2000_0, JD);
#else
  /* Smaller code results if we do it the other way. Keeping this code in case errors are found. */
  CAA2DCoordinate position;
  double nil;
  double nio;
  double toe;
  double RA_nutation;
  double DEC_nutation;

  position =
      CAAPrecession::PrecessEquatorial(RA_Dec_J2000.X, RA_Dec_J2000.Y,
				       solar_system::J2000_0, JD);

  nil = CAANutation::NutationInLongitude(JD);	/* arc seconds. */
  nio = CAANutation::NutationInObliquity(JD);	/*  arc seconds */
  toe = CAANutation::TrueObliquityOfEcliptic(JD);	/*  Degrees */
  RA_nutation = CAANutation::NutationInRightAscension(position.X,
							     position.Y,
							     toe,
							     nil,
							     nio);
  DEC_nutation = CAANutation::NutationInDeclination(position.X,
							   toe,
							   nil,
							   nio);
  /* RA_nutation and DEC_nutation are both in arc seconds. */

  position.X += RA_nutation / 3600.0 / 15.0;
  position.Y += DEC_nutation / 3600.0;

  return position;
#endif
}

CAA2DCoordinate navigation_star::nav_star_RA_Dec(int navstar_num, double JD)
{
  CAA2DCoordinate RA_Dec;
  int bsc_num = navigation_star::nav2bsc[navstar_num];
  int index = extra_solar_bsc::neo_get_index_fast( bsc_num );
  if (index < 0) {
    RA_Dec.X = RA_Dec.Y = 0;
    return RA_Dec;
  }
  RA_Dec = extra_solar_bsc::proper_motion_adjusted_position(flash_memory_array::bsc_array[index], JD);
  double deltaT = CAADynamicalTime::DeltaT(JD);

  RA_Dec = apply_aberration(RA_Dec, JD + deltaT/86400.0 );

  RA_Dec = precession_and_nutation_correct_from_mean_eqinox(RA_Dec, JD);

  return RA_Dec;
}
