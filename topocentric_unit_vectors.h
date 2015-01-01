#ifndef _TOPOCENTRIC_UNIT_VECTORS_H
#define _TOPOCENTRIC_UNIT_VECTORS_H

#include "AA2DCoordinate.h"
#include "AA3DCoordinate.h"

namespace topocentric_unit_vectors{

  CAA3DCoordinate AziAlt_to_UV(CAA2DCoordinate azi_alt);
  CAA2DCoordinate UV_to_AziAlt(CAA3DCoordinate topocentric_uv );

  CAA3DCoordinate XYZ_to_AziAltR(CAA3DCoordinate xyz);
  CAA3DCoordinate AziAltR_to_XYZ(CAA3DCoordinate azi_alt_r);
}

#endif  /*   _TOPOCENTRIC_UNIT_VECTORS_H   */
