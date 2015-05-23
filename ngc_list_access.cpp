#include "ngc_list_access.h"
#include "ngc_list.h" 


uint32_t ngc_list_access::ngc_list_size(){
  return NGC_LIST_SIZE; /* defined at the bottom of very large file ngc_list.h */
}

/* returns -1 if ngc_num is not in the onboard catalog. */
int ngc_list_access::get_index( uint32_t ngc_num ){
  int index = -1;
  for( uint32_t i = 0; i < NGC_LIST_SIZE; ++i ){
    if( ngc_list[i].NGC_number == ngc_num ){
      index = i;
      break;
    }
  }
  return index;
}

uint32_t ngc_list_access::ngc_number( int32_t index ){
  return ngc_list[index].NGC_number;
}


float ngc_list_access::get_magnitude_i( int index ){
  return ngc_list[index].Vmag;
}

float ngc_list_access::get_dimension_a_i( int index ){
  return ngc_list[index].dimension_a;
}

float ngc_list_access::get_dimension_b_i( int index ){
  return ngc_list[index].dimension_b;
}

sexagesimal::Sexagesimal ngc_list_access::get_RA_i( int index ){
  sexagesimal::Sexagesimal RA;
  RA.set_binary_data( ngc_list[index].RA_data );
  return RA;
}

sexagesimal::Sexagesimal ngc_list_access::get_Dec_i( int index ){
  sexagesimal::Sexagesimal DEC;
  DEC.set_binary_data( ngc_list[index].DEC_data );
  return DEC;
}

