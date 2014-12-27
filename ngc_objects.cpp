#include "ngc_objects.h"
#include "ngc_list.h"
#include "sexagesimal.h"

float ngc_objects::get_magnitude( uint32_t ngc_num ){
  int index = 0;
  for( int i = 0; i < NGC_LIST_SIZE; ++i ){
    if( ngc_list[i].NGC_number == ngc_num ){
      index = i;
      break;
    }
  }
  return ngc_list[index].Vmag;
}

float ngc_objects::get_magnitude_i( int index ){
  return ngc_list[index].Vmag;
}

float ngc_objects::get_dimension_a_i( int index ){
  return ngc_list[index].dimension_a;
}

float ngc_objects::get_dimension_b_i( int index ){
  return ngc_list[index].dimension_b;
}

sexagesimal::Sexagesimal ngc_objects::get_RA_i( int index ){
  sexagesimal::Sexagesimal RA;
  RA.set_binary_data( ngc_list[index].RA_data );
  return RA;
}

sexagesimal::Sexagesimal ngc_objects::get_Dec_i( int index ){
  sexagesimal::Sexagesimal DEC;
  DEC.set_binary_data( ngc_list[index].DEC_data );
  return DEC;
}

/* returns -1 if ngc_num is not in the onboard catalog. */
int ngc_objects::get_index( uint32_t ngc_num ){
  int index = -1;
  for( int i = 0; i < NGC_LIST_SIZE; ++i ){
    if( ngc_list[i].NGC_number == ngc_num ){
      index = i;
      break;
    }
  }
  return index;
}


