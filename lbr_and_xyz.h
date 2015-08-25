#ifndef _LBR_AND_XYZ_H
#define _LBR_AND_XYZ_H

#include "AA3DCoordinate.h"

CAA3DCoordinate LBR_to_XYZ(CAA3DCoordinate lbr) noexcept;
/* Reverse transformation. */
CAA3DCoordinate XYZ_to_LBR(CAA3DCoordinate xyz) noexcept;

CAA3DCoordinate AziAltR_to_XYZ(CAA3DCoordinate azi_alt_r);
CAA3DCoordinate XYZ_to_AziAltR(CAA3DCoordinate xyz) noexcept;

#endif   /*   _LBR_AND_XYZ_H  */
