#ifndef _SIGHT_DATA_COMMAND_ACTIONS_H
#define _SIGHT_DATA_COMMAND_ACTIONS_H

/*
 Need to inlcude this because alignment_sight_test() 
 calls long_lat_dialog() if the longitude and latitude are not set.
 */
#include "command_actions.h"
#include <string>
#include "telescope_model.h"

void clear_sight_data();

void pre_fab_example_test();

void alignment_sight_test();

void navigation_star_menu();

void solar_system_menu();

void alignment_prompt(Simple_Altazimuth_Scope* scope, std::string object );

void make_optimized_solution_test();

#endif  /*   _SIGHT_DATA_COMMAND_ACTIONS_H  */
