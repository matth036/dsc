#ifndef _NAVIGATION_STAR_H
#define _NAVIGATION_STAR_H

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <vector>
#include <string>
#include "star_data.h"

namespace navigation_star {

  int is_navigation_star(int bscnum);
  int get_navigation_star_num(int bscnum);
  //  const char *getNavstarName(int nav_number);
  const char *getNAVSTARNAME(int nav_number);

    /* Map the 57 navigational stars to Bright Star Catalog number. */
  constexpr int NAVIGATION_LIST_MAX = 57;

#define GIENAH 4662
  /*  @TODO Check ID of star 29 Gienah.
   *  Should be Gienah Corvi in Corvus.
   *  Should not be the Gienah in Cyngus
   *
   *  4662 seems to be the correct star.
   *  
   *  4662 or 7949?
  **/
  static const int nav2bsc[NAVIGATION_LIST_MAX + 1] = { 424,	/* 0 is Polaris */
    15, 99, 168, 188, 472, 617, 897, 911, 1017, 1457,	/*  1..11 */
    1713, 1708, 1790, 1791, 1903, 2061, 2326, 2491, 2618, 2943,	/* 11..20 */
    2990, 3307, 3207, 3685, 3748, 3982, 4301, 4534, GIENAH, 4730,	/* 21..30 */
    4763, 4905, 5056, 5191, 5267, 5288, 5340, 5459, 5531, 5563,	/* 31..40 */
    5793, 6134, 6217, 6378, 6527, 6556, 6705, 6879, 7001, 7121,	/* 41..50 */
    7557, 7790, 7924, 8308, 8425, 8728, 8781	/* 51..57 */
  };

  std::string get_navigation_star_name(uint32_t nav_num);

}       /* namespace navigation_star */

#endif	/* EOF  -  _NAVIGATION_STAR_H */
