#ifndef _TELESCOPE_MODEL_H
#define _TELESCOPE_MODEL_H

#include <cstdint>
#include <vector>
#include <memory>
#include "AA3DCoordinate.h"
#include "AA2DCoordinate.h"
// #include "float_coordinates.h"
// #include "quadrature_decoder.h"
class Quadrature_Decoder;

/* 
 * While encoder data is born as unsigned integers it is 
 * better to work with signed integers to avoid special code
 * for the wrap around near the maximum value and zero.
 *
 * Use signed integer types for encoder data.
 *
 */
typedef struct {
  int32_t azi_value;
  int32_t alt_value;
} Alt_Azi_Snapshot_t;

class Simple_Altazimuth_Scope {
 public:
  Simple_Altazimuth_Scope(std::unique_ptr < Quadrature_Decoder > azi,
			  std::unique_ptr < Quadrature_Decoder > alt);
  Alt_Azi_Snapshot_t get_snapshot();
  const CAA3DCoordinate calculate_mount_frame_unit_vector(Alt_Azi_Snapshot_t);
  Alt_Azi_Snapshot_t calculate_target_snapshot(CAA3DCoordinate target);
  const CAA3DCoordinate altitude_forward_backward_difference(Alt_Azi_Snapshot_t);
  const CAA3DCoordinate azimuth_forward_backward_difference(Alt_Azi_Snapshot_t);
  const double azimuth_degrees();
  const double altitude_degrees();
  const double azimuth_degrees(int32_t encoder_value);
  const double altitude_degrees(int32_t encoder_value);
  const double azimuth_encoder_value( double azimuth_degrees );
  const double altitude_encoder_value( double altitude_degrees );
  void increment_azimuth_encoder();
  void decrement_azimuth_encoder();

  const double get_altitude_offset();
  const double get_azimuth_offset();

  void invert_azimuth_direction();
  void invert_altitude_direction();
  const bool altitude_direction_is_reversed();
  const bool azimuth_direction_is_reversed();
  void set_altitude_offset(double);
  void set_azimuth_offset(double);
  void set_top_to_tel_matrix(double *);
  void set_determinant(double);
  double get_determinant();
  inline void set_singular_values(CAA3DCoordinate svs) {
    /* The existence of this presumes a specific algorithm.  
     * Not very object oriented. 
     * It is inlined only so it can be more easily removed. 
     */
    singular_values = svs;
  } 
  double get_align_error_mean();
  double get_align_error_max();
  void set_align_error_mean(double);
  void set_align_error_max(double);
  CAA3DCoordinate topo_to_tele(CAA3DCoordinate);
  CAA3DCoordinate tele_to_topo(CAA3DCoordinate);
  CAA2DCoordinate current_topocentric_Azi_and_Alt();
  CAA2DCoordinate topocentric_Azi_and_Alt(Alt_Azi_Snapshot_t);
  // Eliminate this stinker. 
  // CAA2DCoordinate encoder_Azi_and_Alt(CAA2DCoordinate topocentric_azi_alt);

  void coordinate_debug();

  CAA2DCoordinate current_RA_and_Dec();

  CAA2DCoordinate RA_and_Dec(Alt_Azi_Snapshot_t snapshot, double JD,
			     float temperature, float pressure);

  Alt_Azi_Snapshot_t Target_Snapshot(CAA2DCoordinate RA_and_Dec, double JD,
			     float temperature, float pressure);

  int32_t get_azimuth_ticks_per_revolution();
  int32_t get_altitude_ticks_per_revolution();
 private:
  void to_topocentric_transform(double &x, double &y, double &z);
  void from_topocentric_transform(double &x, double &y, double &z);
  std::unique_ptr < Quadrature_Decoder > azimuth_hardware;
  std::unique_ptr < Quadrature_Decoder > altitude_hardware;
  double altitude_offset;
  double azimuth_offset;
  bool reverse_altitude_direction;
  bool reverse_azimuth_direction;
  CAA3DCoordinate singular_values;
  double topocentric_to_telescope_matrix[9];
  double align_error_mean;
  double align_error_max;
  double determinant;
};

#endif
/*******************************
  The interface approach for describing a telescope mount has been set aside while I work to make a viable 
  product for non abstract implementation Simple_Altazimuth_Scope
*************************************/
