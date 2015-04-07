#ifndef _LINEAR_ALBEGRA_INTERFACE_H
#define _LINEAR_ALBEGRA_INTERFACE_H

#include "AA3DCoordinate.h"
class Alignment_Data_Set;
class Simple_Altazimuth_Scope;

namespace linear_algebra_interface {

  /* 
     Need to define: 
     double linear_algebra_interface::Matrix_A_as_data[9] = {0.,0.,0.,0.,0.,0.,0.,0.,0.};
     double linear_algebra_interface::alignment_error_mean = 0;
     double linear_algebra_interface::alignment_error_max = 0;

     Do so in linear_algebra_interface.cpp
     These are global but protected (from namespace polution) by a namespace.
   */
  extern double Matrix_A_as_data[9];
  extern double alignment_error_mean;
  extern double alignment_error_max;
  extern CAA3DCoordinate singular_values;

  void simple_altazimuth_optimize_altitude_offset(Alignment_Data_Set*,
						  Simple_Altazimuth_Scope* );






  CAA3DCoordinate optimize_solution_get_singular_values();
  CAA3DCoordinate optimize_solution_get_singular_values_HACKTATION();

  CAA3DCoordinate topocentric_to_telescope(CAA3DCoordinate r);

  CAA3DCoordinate telescope_to_topocentric(CAA3DCoordinate r);

  void svd_test();

}
#endif /*  _LINEAR_ALBEGRA_INTERFACE_H  */
