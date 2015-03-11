#ifndef STAR_DATA_H
#define STAR_DATA_H
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include "sexagesimal.h"

/***********************
Structure use for positions for the 57 navigational stars and 
other stars from the Bright Star Catalog.
 *********************/
#ifdef __cplusplus
extern "C" {
#endif

  typedef struct {
    uint32_t RAdata;
    uint32_t DECdata;
    float RApm;
    float DECpm;
    float magnitude;
    int32_t BSCnum;
  } star_data;

#ifdef __cplusplus
}
#endif
#endif				/* EOF  - STAR_DATA_H  */
