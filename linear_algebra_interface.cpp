#include "linear_algebra_interface.h"
#include "linear_algebra_facilities.h"
#include "main.h"

static Matrix3d A;

double linear_algebra_interface::Matrix_A_as_data[9] =
    { 0., 0., 0., 0., 0., 0., 0., 0., 0. };
double linear_algebra_interface::alignment_error_mean = 0;
double linear_algebra_interface::alignment_error_max = 0;



void linear_algebra_interface::simple_altazimuth_optimize_altitude_offset(
									  Alignment_Data_Set * data_set,
									  Simple_Altazimuth_Scope * telescope)
{
  /* Just pass the arguemts on to namespace linear_algebra_facilities */
  linear_algebra_facilities::simple_altazimuth_optimize_altitude_offset(data_set,telescope);
}

CAA3DCoordinate linear_algebra_interface::
optimize_solution_get_singular_values()
{
  Alignment_Data_Set *data_set = get_main_sight_data();
  return optimize_alt_offset_and_get_singular_values(data_set);
}

CAA3DCoordinate linear_algebra_interface::
optimize_solution_get_singular_values_HACKTATION()
{
  Alignment_Data_Set *data_set = get_main_sight_data();
  return optimize_alt_offset_and_get_singular_values_HACKTATION(data_set);
}

void linear_algebra_interface::svd_test()
{
  simple_svd_test();
}
