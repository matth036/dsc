#ifndef _LINEAR_ALGEBRA_FACILIITIES
#define _LINEAR_ALGEBRA_FACILIITIES

#ifdef USE_EIGEN_FOR_LINEAR_ALGEBRA

#include <Eigen/Dense>
#include <Eigen/SVD>
#include <Eigen/Geometry>
#include "AA3DCoordinate.h"
#include "linear_algebra_interface.h"

using namespace Eigen;
//linear_algebra_interface
//extern double linear_algebra_interface::Matrix_A_as_data[9];

class Alignment_Data_Set;
class Simple_Altazimuth_Scope;

namespace linear_algebra_facilities {
  void simple_altazimuth_optimize_altitude_offset(Alignment_Data_Set * data_set,
						  Simple_Altazimuth_Scope *
						  telescope);
} 

Matrix3d B_Matrix_from_Alignment_Data_Set(Alignment_Data_Set &);
/* I have changed the return value to a triple of the singualr values. 
 * @TODO rename this function so the name is not misleading.
 **/
double compare_pair_by_pair(Alignment_Data_Set *, uint32_t max_size );

CAA3DCoordinate optimize_alt_offset_and_get_singular_values(Alignment_Data_Set
							    *);
CAA3DCoordinate
optimize_alt_offset_and_get_singular_values_HACKTATION(Alignment_Data_Set *);

void simple_svd_test();

#endif				/* USE_EIGEN_FOR_LINEAR_ALGEBRA */

#endif   /*  _LINEAR_ALGEBRA_FACILIITIES */
