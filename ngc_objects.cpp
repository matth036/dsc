#include "ngc_objects.h"
// #include "ngc_list.h"
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




