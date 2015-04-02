#include "linear_algebra_facilities.h"

/* This file exists to avoid binding to any linear albebra 
 * package too tightly.
 * 
 *
 */

#include "alignment_sight_info.h"
#include "main.h"
#include "AACoordinateTransformation.h"
#include "solar_system.h"
#include "stm32_e407_utilities.h"

#ifdef USE_EIGEN_FOR_LINEAR_ALGEBRA

Vector3d AA3D_to_Vector(CAA3DCoordinate src)
{
  Vector3d vect;
  vect[0] = src.X;
  vect[1] = src.Y;
  vect[2] = src.Z;
  return vect;
}

void simple_altazimuth_optimize_altitude_offset_iteration(Alignment_Data_Set *
							  data_set,
							  Simple_Altazimuth_Scope
							  * telescope)
{
  CAA3DCoordinate svs;
  CAA3DCoordinate temporary;
  double Matrix_A_data[9];
  Matrix3d A{Matrix_A_data};
  double det_A;
  uint32_t size = data_set->size();

  /* r contains unit vectors in topocentric frame.   */
  std::vector < CAA3DCoordinate > r {size};   /*  A speed refinement would be to make this static between calls.  */
  /* b contains unit vectors in telesope mount frame */
  std::vector < CAA3DCoordinate > b {size}; 
  /* c contains unit vectors indicating the change of direction resulting from a change in altitude offset. 
   * Thse will be perpendicular to the b vectors.
   */
  std::vector < CAA3DCoordinate > c {size}; 


  for (uint32_t s = 0; s < size; ++s) {
    std::string object = data_set->get_object_name(s);
    double JD = data_set->get_julian_date(s);
    bool success = false;
    CAA2DCoordinate RA_and_Dec = calulate_RA_and_Dec(object, JD, success);
    if (!success) {
      assert_param(false);
      continue;
    }
    float pressure = data_set->get_pressure(s);
    float temperature = data_set->get_temperature(s);
    temporary =data_set->topocentric_unit_vector(RA_and_Dec, 
						 JD, pressure,temperature);
    r[s] = temporary;

    Alt_Azi_Snapshot_t encoder_data = data_set->get_encoder_data(s);
    temporary = telescope->calculate_mount_frame_unit_vector( encoder_data );
    b[s] = temporary;
    temporary = telescope->altitude_forward_backward_difference(encoder_data);
    c[s] = temporary;
  }

  /* Form the matrix B as described by F. Landis Markley. */
  Matrix3d B = Matrix3d::Zero();
  for (uint32_t s = 0; s < size; ++s) {
    Vector3d rr = AA3D_to_Vector(r[s]);
    Vector3d bb = AA3D_to_Vector(b[s]);
    B += bb * rr.transpose();
  }
  JacobiSVD < Matrix3d > svd {B, NoQRPreconditioner | ComputeFullU | ComputeFullV};
  Matrix3d U = svd.matrixU();
  Matrix3d V = svd.matrixV();
  A = U * V.transpose();
  det_A = A.determinant();
  if (det_A < 0.0) {	/* Not a proper rotation. */
    /*******************
    for (int j = 0; j < 3; ++j) {
      (V.transpose()(2, j)) *= -1.0;
    }
    A = U * V.transpose();
    ************************/
  }
  det_A = A.determinant();
  telescope->set_determinant( det_A );
  auto would_be_nice_to_know = svd.singularValues();
  /* The return type is Matrix<double,3,1> */
  svs.X = would_be_nice_to_know[0];
  svs.Y = would_be_nice_to_know[1];
  svs.Z = would_be_nice_to_know[2];
  /* A is now the optimal matrix if the altitude offset was itself optimal.
   * Adjust the altitude offset to improve the fit.
   **/
  double numerator = 0.0;
  double denominator = 0.0;
  double sum_error = 0.0;
  double max_error = std::numeric_limits<double>::min();
  int count = 0;
  double sum_lengths = 0.0;
  for (uint32_t s = 0; s < size; ++s) {
    Vector3d rr = AA3D_to_Vector(r[s]);
    Vector3d bb = AA3D_to_Vector(b[s]);
    Vector3d cc = AA3D_to_Vector(c[s]);
    double norm = cc.norm();
    cc /= norm;
    numerator += cc.transpose() * A * rr;
    denominator += cc.squaredNorm();
    sum_lengths += cc.norm();
    ++count;
    double error = (bb - A * rr).norm();
    sum_error += error;
    if( error >= max_error ){
      max_error = error;
    }
  }
  double lambda = numerator / denominator;
  double lambda_radians = lambda;
  double old_offset = telescope->get_altitude_offset();
  double lambda_degrees =
      CAACoordinateTransformation::RadiansToDegrees(lambda_radians);
  double new_offset;
  if( telescope->altitude_direction_is_reversed() ){
    /* Altitude decreases with encoder value increasing. */
    for(;;){/* NOT TESTETD */}
    new_offset = old_offset + lambda_degrees;
  }else{
    /* Altitude increases with encoder value increasing. */
    new_offset = old_offset - lambda_degrees;
  }
  telescope->set_altitude_offset(new_offset);
  telescope->set_singular_values( svs );
  /* Matrix_A_data was supposed to be the data inside Matrix3d A
   * Apparently it is not.  This is a hack to make the 
   * applicaton work.
   */
  Matrix_A_data[0] = A(0,0);
  Matrix_A_data[1] = A(0,1);
  Matrix_A_data[2] = A(0,2);
  Matrix_A_data[3] = A(1,0);
  Matrix_A_data[4] = A(1,1);
  Matrix_A_data[5] = A(1,2);
  Matrix_A_data[6] = A(2,0);
  Matrix_A_data[7] = A(2,1);
  Matrix_A_data[8] = A(2,2);
  /* */
  telescope->set_top_to_tel_matrix(Matrix_A_data);
  /* Some math fuzz is involved here. (Small angle approximation, chord vs. arc.) */
  /* With two alignment sights, the error goes to zero! */
  double mean_error = sum_error/count;
  telescope->set_align_error_max( static_cast<float>(CAACoordinateTransformation::RadiansToDegrees(max_error) ) );
  telescope->set_align_error_mean( static_cast<float>(CAACoordinateTransformation::RadiansToDegrees( mean_error ) ) );
}

void linear_algebra_facilities::
simple_altazimuth_optimize_altitude_offset(Alignment_Data_Set * data_set,
					   Simple_Altazimuth_Scope * telescope)
{
  uint32_t max_iterations = 110;
  for (uint32_t n = 1; n <= max_iterations; ++n) {
    simple_altazimuth_optimize_altitude_offset_iteration(data_set, telescope);
  }
}

void simple_svd_test()
{
  Matrix3d B;
  B(0, 0) = 1.0;
  B(0, 1) = 1.0;
  B(0, 2) = 0.0;

  B(1, 0) = 0.0;
  B(1, 1) = 1.0;
  B(1, 2) = 0.0;

  B(2, 0) = 0.0;
  B(2, 1) = 0.0;
  B(2, 2) = 1.0 / 23 * 23;	/* An arbitrary value smaller than the golden mean. */
  /****************************
  JacobiSVD < Matrix3d > svd {
  B, NoQRPreconditioner};
  ***************************/
  JacobiSVD < Matrix3d > svd {
  B, NoQRPreconditioner | ComputeFullU | ComputeFullV};

  Matrix3d U = svd.matrixU();	// The program goes to bad here on the uC.
  Matrix3d V = svd.matrixV();
  Matrix3d A = U * V.transpose();
  if (A.determinant() < 0) {

  } else {

  }

}

/* Empty comments aren't cool. */
Matrix3d B_Matrix_from_Alignment_Data_Set(Alignment_Data_Set & data_set)
{
  Matrix3d B = Matrix3d::Zero();
  // Matrix3d cAr = Matrix3d::Zero();
  uint32_t size = data_set.size();
  for (uint32_t s = 0; s < size; ++s) {
    std::string object = data_set.get_object_name(s);
    double JD = data_set.get_julian_date(s);
    bool success = false;
    CAA2DCoordinate RA_and_Dec = calulate_RA_and_Dec(object, JD, success);
    if (!success) {
      assert_param(false);
      continue;
    }
    float pressure = data_set.get_pressure(s);
    float temperature = data_set.get_temperature(s);
    CAA3DCoordinate uv_r =
	data_set.topocentric_unit_vector(RA_and_Dec, JD, pressure, temperature);
    CAA3DCoordinate uv_b = data_set.telescope_frame_unit_vector(s);
    // CAA3DCoordinate vect_c = data_set.forward_backward_difference(s);

    Vector3d r = AA3D_to_Vector(uv_r);
    Vector3d b = AA3D_to_Vector(uv_b);
    B += b * r.transpose();
    /*************************************
    B(0, 0) += uv_b.X * uv_r.X;
    B(0, 1) += uv_b.X * uv_r.Y;
    B(0, 2) += uv_b.X * uv_r.Z;

    B(1, 0) += uv_b.Y * uv_r.X;
    B(1, 1) += uv_b.Y * uv_r.Y;
    B(1, 2) += uv_b.Y * uv_r.Z;

    B(2, 0) += uv_b.Z * uv_r.X;
    B(2, 1) += uv_b.Z * uv_r.Y;
    B(2, 2) += uv_b.Z * uv_r.Z;
    ****************************************/
  }
  return B;
}

/* The matrix used in Davenport quaternion method. (And other quaternion methods.) */
Matrix4d K_Matrix_from_Alignment_Data_Set(Alignment_Data_Set & data_set)
{
  Matrix4d K;
  Matrix3d B = B_Matrix_from_Alignment_Data_Set(data_set);
  double trB = B.trace();
  Matrix3d S = B + B.transpose();
  Matrix3d K3 = S - trB * Matrix3d::Identity();
  Vector3d z;
  z(0) = B(1, 2) - B(2, 1);
  z(1) = B(2, 0) - B(0, 2);
  z(2) = B(0, 1) - B(1, 0);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      K(i, j) = K3(i, j);
    }
  }
  for (int i = 0; i < 3; ++i) {
    K(0, i) = z(i);
    K(i, 0) = z(i);
  }
  K(3, 3) = trB;
  return K;
}

uint32_t debug_iteration_count;

CAA3DCoordinate optimize_alt_offset_and_get_singular_values(Alignment_Data_Set *
							    data_set)
{
  CAA3DCoordinate svs;
  CAA3DCoordinate temporary;
  Matrix3d A;
  Matrix3d B = Matrix3d::Zero();
  uint32_t max_iterations = 110 * 10;
  uint32_t size = data_set->size();

  for (uint32_t n = 1; n <= max_iterations; ++n) {
    debug_iteration_count = n;
    for (uint32_t s = 0; s < size; ++s) {
      std::string object = data_set->get_object_name(s);
      double JD = data_set->get_julian_date(s);
      bool success = false;
      CAA2DCoordinate RA_and_Dec = calulate_RA_and_Dec(object, JD, success);
      if (!success) {
	assert_param(false);
	continue;
      }
      float pressure = data_set->get_pressure(s);
      float temperature = data_set->get_temperature(s);
      CAA3DCoordinate uv_r =
	  data_set->topocentric_unit_vector(RA_and_Dec, JD, pressure,
					    temperature);
      CAA3DCoordinate uv_b = data_set->telescope_frame_unit_vector(s);
      Vector3d r = AA3D_to_Vector(uv_r);
      Vector3d b = AA3D_to_Vector(uv_b);
      B += b * r.transpose();
    }
    JacobiSVD < Matrix3d > svd {
    B, NoQRPreconditioner | ComputeFullU | ComputeFullV};
    Matrix3d U = svd.matrixU();
    Matrix3d V = svd.matrixV();
    Matrix3d A = U * V.transpose();
    if (A.determinant() < 0.0) {	/* Not a proper rotation. */
      for (int j = 0; j < 3; ++j) {
	(V.transpose()(2, j)) *= -1.0;
      }
      A = U * V.transpose();
    }
    auto would_be_nice_to_know = svd.singularValues();
    /* The return type is Matrix<double,3,1> */
    svs.X = would_be_nice_to_know[0];
    svs.Y = would_be_nice_to_know[1];
    svs.Z = would_be_nice_to_know[2];
    /* A is now the optimal matrix if the altitude offset itself was optimal.
     * Adjust the altitude offset to improve the fit.
     **/
    Simple_Altazimuth_Scope *telescope = data_set->get_telescope_model();

    double numerator = 0.0;
    double denominator = 0.0;
    int count = 0;
    double sum_lengths = 0.0;
    for (uint32_t s = 0; s < size; ++s) {
      std::string object = data_set->get_object_name(s);
      double JD = data_set->get_julian_date(s);
      bool success = false;
      /* Yes, this repeats a calulation that is likely expensive. */
      CAA2DCoordinate RA_and_Dec = calulate_RA_and_Dec(object, JD, success);
      if (!success) {
	assert_param(false);
	continue;
      }
      ++count;
      float pressure = data_set->get_pressure(s);
      float temperature = data_set->get_temperature(s);

      temporary =
	  data_set->topocentric_unit_vector(RA_and_Dec, JD, pressure,
					    temperature);
      Vector3d r = AA3D_to_Vector(temporary);
      Alt_Azi_Snapshot_t enc_data = data_set->get_encoder_data(s);

      temporary = telescope->altitude_forward_backward_difference(enc_data);
      Vector3d c = AA3D_to_Vector(temporary);
      numerator += c.transpose() * A * r;
      denominator += c.squaredNorm();
      sum_lengths += c.norm();
    }
    double lambda = numerator / denominator;
    double lambda_radians = lambda / count;	//  * sum_lengths/count;
    /* 
     * The altitude offset getter and setter are not part of the Telescope_Model interface.
     **/
    double old_offset = telescope->get_altitude_offset();
    /*
     * If azimuth increases with encoder counts, add lambda.
     * If azimuth  decrases with encoder counts, subtract lambda.
     */
    double lambda_degrees =
	CAACoordinateTransformation::RadiansToDegrees(lambda_radians);
    double new_offset = old_offset - lambda_degrees;
    telescope->set_altitude_offset(new_offset);
    double epsilon = 1.0E-8;
    if (-epsilon < lambda_radians && lambda_radians < epsilon) {
      break;
    }
  }
  // assert_param( false );  /* Will definately want a non halting way to deal with this. */
  return svs;
}

double include_angle(CAA3DCoordinate a, CAA3DCoordinate b)
{
  double norm = solar_system::dot_product(a, a);
  a = solar_system::product(a, 1.0 / norm);
  norm = solar_system::dot_product(b, b);
  b = solar_system::product(b, 1.0 / norm);
  double c = solar_system::dot_product(a, b);
  double theta = acos(c);
  return CAACoordinateTransformation::RadiansToDegrees(theta);
}

double compare_pair_by_pair(Alignment_Data_Set * data_set, uint32_t max_size)
{
  size_t size = data_set->size();
  if( size> max_size ){
    /* This is to avoid large N for this O(N^2) procedure. */
    size = max_size;
  }
  CAA3DCoordinate temporary;
  std::vector < CAA3DCoordinate > r {
  size};
  std::vector < CAA3DCoordinate > b {
  size};
  for (uint32_t s = 0; s < size; ++s) {
    std::string object = data_set->get_object_name(s);
    double JD = data_set->get_julian_date(s);
    bool success = false;
    CAA2DCoordinate RA_and_Dec = calulate_RA_and_Dec(object, JD, success);
    if (!success) {
      assert_param(false);
      continue;
    }
    float pressure = data_set->get_pressure(s);
    float temperature = data_set->get_temperature(s);
    temporary =
	data_set->topocentric_unit_vector(RA_and_Dec, JD, pressure,
					  temperature);
    r[s] = temporary;
    temporary = data_set->telescope_frame_unit_vector(s);
    b[s] = temporary;
  }
  double sum = 0.;
  uint32_t count = 0;
  for (uint32_t s = 0; s < size; ++s) {
    for (uint32_t t = s + 1; t < size; ++t) {
      double angle_r = include_angle(r[s], r[t]);
      double angle_b = include_angle(b[s], b[t]);
      double diff;
      if (angle_r > angle_b) {
	diff = angle_r - angle_b;
      } else {
	diff = angle_b - angle_r;
      }
      ++count;
      sum += diff;
    }
  }
  return sum / count;
}

CAA3DCoordinate
optimize_alt_offset_and_get_singular_values_HACKTATION(Alignment_Data_Set *
						       data_set)
{
  CAA3DCoordinate svs;
  CAA3DCoordinate temporary;
  Matrix3d A;
  Matrix3d B = Matrix3d::Zero();
  uint32_t max_iterations = 110;
  uint32_t size = data_set->size();
  uint32_t fliptation_count = 0;
  Simple_Altazimuth_Scope *telescope = data_set->get_telescope_model();

  double check = compare_pair_by_pair(data_set, 80);
  if (check > 180) {
    // this can be way off it the altitude offset is off. 
    assert_param(true);
  }

  for (uint32_t n = 1; n <= max_iterations; ++n) {
    debug_iteration_count = n;
    for (uint32_t s = 0; s < size; ++s) {
      std::string object = data_set->get_object_name(s);
      double JD = data_set->get_julian_date(s);
      bool success = false;
      CAA2DCoordinate RA_and_Dec = calulate_RA_and_Dec(object, JD, success);
      if (!success) {
	assert_param(false);
	continue;
      }
      float pressure = data_set->get_pressure(s);
      float temperature = data_set->get_temperature(s);
      temporary =
	  data_set->topocentric_unit_vector(RA_and_Dec, JD, pressure,
					    temperature);
      Vector3d r = AA3D_to_Vector(temporary);
      temporary = data_set->telescope_frame_unit_vector(s);
      Vector3d b = AA3D_to_Vector(temporary);
      // Alt_Azi_Snapshot_t enc_data = data_set->get_encoder_data(s);
      // temporary = telescope->calculate_unit_vector( enc_data );
      /* Compare temporary and uv_b */

      B += b * r.transpose();
    }
    JacobiSVD < Matrix3d > svd {
    B, NoQRPreconditioner | ComputeFullU | ComputeFullV};
    Matrix3d U = svd.matrixU();
    Matrix3d V = svd.matrixV();
    Matrix3d A = U * V.transpose();
    if (A.determinant() < 0.0) {	/* Not a proper rotation. */
      for (int j = 0; j < 3; ++j) {
	(V.transpose()(2, j)) *= -1.0;
      }
      A = U * V.transpose();
    }
    auto would_be_nice_to_know = svd.singularValues();
    /* The return type is Matrix<double,3,1> */
    svs.X = would_be_nice_to_know[0];
    svs.Y = would_be_nice_to_know[1];
    svs.Z = would_be_nice_to_know[2];
    /* A is now the optimal matrix if the altitude offset itself was optimal.
     * Adjust the altitude offset to improve the fit.
     **/

    double numerator = 0.0;
    double denominator = 0.0;
    int count = 0;
    double sum_lengths = 0.0;
    double error = 0.0;
    for (uint32_t s = 0; s < size; ++s) {
      std::string object = data_set->get_object_name(s);
      double JD = data_set->get_julian_date(s);
      bool success = false;
      /* Yes, this repeats a calulation that is likely expensive. */
      CAA2DCoordinate RA_and_Dec = calulate_RA_and_Dec(object, JD, success);
      if (!success) {
	assert_param(false);
	continue;
      }
      ++count;
      float pressure = data_set->get_pressure(s);
      float temperature = data_set->get_temperature(s);
      /************
  temporary =
	data_set->topocentric_unit_vector(RA_and_Dec, JD, pressure, temperature);
      Vector3d r = AA3D_to_Vector( temporary );
      temporary = data_set->telescope_frame_unit_vector(s);
      Vector3d b = AA3D_to_Vector( temporary );
      ********************/

      temporary =
	  data_set->topocentric_unit_vector(RA_and_Dec, JD, pressure,
					    temperature);
      Vector3d r = AA3D_to_Vector(temporary);

      Alt_Azi_Snapshot_t enc_data = data_set->get_encoder_data(s);
      temporary = telescope->altitude_forward_backward_difference(enc_data);
      Vector3d c = AA3D_to_Vector(temporary);
      double norm = c.norm();
      c /= norm;

      temporary = data_set->telescope_frame_unit_vector(s);
      Vector3d b = AA3D_to_Vector(temporary);

      double error_term = (b - A * r).norm();
      error += error_term;

      double addon = (c.transpose() * A * r) (0, 0);
      numerator += addon;
      denominator += c.squaredNorm();
      sum_lengths += c.norm();
    }
    error = CAACoordinateTransformation::RadiansToDegrees(error / count);

    double lambda = numerator / denominator;
    double lambda_radians = lambda;	//  * sum_lengths/count;
    /* 
     * The altitude offset getter and setter are not part of the Telescope_Model interface.
     **/
    double old_offset = telescope->get_altitude_offset();
    /*
     * If azimuth increases with encoder counts, add lambda.
     * If azimuth  decrases with encoder counts, subtract lambda.
     */
    double lambda_degrees =
	CAACoordinateTransformation::RadiansToDegrees(lambda_radians);
    double new_offset = old_offset + lambda_degrees;
    telescope->set_altitude_offset(new_offset);
    double whacky_limit = 720.0;
    if (lambda_degrees > whacky_limit || lambda_degrees < -whacky_limit) {
      telescope->invert_altitude_direction();
      telescope->set_altitude_offset(0.0);
      ++fliptation_count;
      assert_param(fliptation_count < 4);
    }
    double epsilon = 1.0E-8;
    if (-epsilon < lambda_radians && lambda_radians < epsilon) {
      break;
    }
  }
  // assert_param( false );  /* Will definately want a non halting way to deal with this. */
  return svs;
}

JacobiSVD < Matrix3d >
singular_value_decompostion(Alignment_Data_Set & data_set)
{
  Matrix3d B = B_Matrix_from_Alignment_Data_Set(data_set);
  JacobiSVD < Matrix3d > svd {
  B, NoQRPreconditioner};
  /**************
  Matrix3d U = svd.matrixU();
  Matrix3d V = svd.matrixV();
  return U*V;
  ******************/
  return svd;
}

#endif
