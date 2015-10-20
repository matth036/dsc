#ifndef STAR_DATA_H_
#define STAR_DATA_H_
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include "sexagesimal.h"

/**
 * Structure used for positions for the 57 navigational stars and 
 * other stars from the Bright Star Catalog.
 **/
#ifdef __cplusplus
extern "C" {
#endif

  /**
    * A struct holding data about a star from the Bright Star Catalog.
    * 
    * Binary data RAdata and DECdata are encoded as the data member of a sexagesimal::Sexagesimal object.
    *
    */
  
  typedef struct {
    uint32_t RAdata;   /*!< Right Ascencion binary data. */
    uint32_t DECdata;  /*!< Declination binary data. */
    float RApm;        /*!< Right Ascencion component of proper motion. (arcsec/yr) */
    float DECpm;       /*!< Declination component of proper motion. (arcsec/yr)*/
    float magnitude;   /*!< Visual magnitude. */
    int32_t BSCnum;    /*!< Bright Star Catalog number. */
  } star_data;

#ifdef __cplusplus
}
#endif
#endif  // STAR_DATA_H_
