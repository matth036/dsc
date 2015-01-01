#ifndef _TELESCOPE_UNIT_VECTORS_H
#define _TELESCOPE_UNIT_VECTORS_H

/*
 * We have isolated transformations from cooridnates (azimuth,altitude)
 * to unit vector (uv_x, uv_y, uv_z) where,
 *  1) The angular coordinates azimuth and altitude are reckoned in the topocentric coordiate system.
 *  2) The Cartesian coordinates are also reckoned in the topocentric coordiate system.
 * These tranformations are declared in topocentric_unit_vectors.h and implemented in 
 * topocentric_unit_vectors.cpp.   
 *
 * We need similar transformations from the angular coordinates (Azimuth_Shaft_Angle,Altitude_Shaft_Angle)
 * to a unit vector expressed in a Cartesion coordinate system which is fixed with respect to the topocentric 
 * system.
 *
 * Established methods of attitude determination (solutions to Wahba's Problem) can then
 * be used to find the transformation from the telescope Cartesian frame to the topocentric Cartesian frame.
 *
 * We evidently need to declare and implement the transformations from the telescope mount shaft angles to
 * some arbitrary Cartesian system.  As a design decision, it has been decided that these transformations 
 * should be member functions of the telescope model class.  
 *
 * So, this file, telescope_unit_vectors.h is effectively white space.
 * 
 * Thank you for your attention.
 */

#endif  /*   _TELESCOPE_UNIT_VECTORS_H   */
