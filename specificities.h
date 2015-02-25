#ifndef _SPECIFICITIES_H
#define _SPECIFICITIES_H

/* 
 * Stuff (parameters) in this file
 * should be end user set-able in a finished product. 
 */
namespace specificities{
  constexpr uint32_t altitude_ticks_per_revolution = 4000;
  constexpr uint32_t azimuth_ticks_per_revolution = 4000;

  /* 
   * Ultimately, the startup count should not matter.
   * For the purpose of debugging and trouble shooting I arrange
   * for the azimuth count to be zero near north and the altitude count to be 
   * zero near the horizon.  
   * 
   * Working near 45 degress north latitude, these values ensure this if
   * the microcontroller is turned on while the telescope is pointed at Polaris.
   */
  constexpr int32_t your_latitude = 45;
  constexpr uint32_t altitude_startup_count = (altitude_ticks_per_revolution/4)*your_latitude/360;
  constexpr uint32_t azimuth_startup_count = 0;


}

#endif   /*  _SPECIFICITIES_H  */
