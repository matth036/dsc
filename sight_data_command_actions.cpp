#include "sight_data_command_actions.h"

#include "main.h"

/* @TODO put in an annoying confirm dialog. */
void clear_sight_data(){
  Alignment_Data_Set*  data_set =  get_main_sight_data();
  data_set->clear();
}
