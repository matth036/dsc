#ifndef BINARY_TIDBITS_H
#define BINARY_TIDBITS_H

/* These values could well be contexpr. We provide them as functions
 * as a reminder that these are only conventions.  Reversing any of 
 * these should have no visible effect after a complete rebuild.  
 * But don't expect that I have tested this.
 */
namespace binary_tidbits{

  /*
   * See Meeuse, Astronimical Algorithims 2nd Edition pp. 93.
   */
  inline bool west_longitude_is_positive(){
    return true;
  }

  /* Adopt the conventions of a northern hemisphere navigator.
   *
   */
  inline bool zero_azimuth_is_north(){
    return true;
  }

  /******************************
  inline bool azimuth_increases_north_to_east(){
    return true;
  }
  *******************************/



}

#endif /* BINARY_TIDBITS_H */
