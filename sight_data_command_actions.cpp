#include "sight_data_command_actions.h"

#include "main.h"
#include "AADate.h"
#include "microsecond_delay.h"
#include "rtc_management.h"
#include "navigation_star.h"

/* @TODO put in an annoying confirm dialog. */
void clear_sight_data(){
  Alignment_Data_Set*  data_set =  get_main_sight_data();
  data_set->clear();
}

void pre_fab_example_test(){
  auto lcd = check_out_main_lcd();
  lcd->clear();
  lcd->noDisplay();
  MicroSecondDelay::millisecond_delay(1);
  lcd->display();
  lcd->home();
  lcd->setCursor(0,0);
  lcd->print( "Prefab Example Test" );

  Simple_Altazimuth_Scope* scope = get_main_simple_telescope();
  // scope->invert_azimuth_direction();
  // scope->invert_altitude_direction();

  // CAADate sunday_eve{2014,3,17,5,0,0.0,true};
  //  CAADate date{2014,3,21,3,0,0.,true};
  /*
   * The sample was intended for 3 hours U.T. 
   * The data better fits 4 hours U.T.
   */
  CAADate date{2014,3,21,4,0,0.,true};
  double JD = date.Julian();

  Alignment_Data_Set*  data_set =  get_main_sight_data();
  data_set->set_longitude_and_latitude( get_backup_domain_longitude(),
					get_backup_domain_latitude() );

#if 0
  double longitude = data_set->get_longitude();
  lcd->setCursor(0,1);
  lcd->print( longitude,2);

  double latitude = data_set->get_latitude();
  lcd->setCursor(0,2);
  lcd->print( latitude,2);
#endif

  Alt_Azi_Snapshot_t data_0{3990,498};
  Alignment_Sight_Item polaris( navigation_star::get_navigation_star_name(0) );
  polaris.set_pointing_information( JD,data_0);
  data_set->push_item( polaris );

  Alt_Azi_Snapshot_t data_26{1866,627};
  Alignment_Sight_Item regulus( navigation_star::get_navigation_star_name(26) );
  regulus.set_pointing_information( JD,data_26);
  data_set->push_item( regulus );

  Alt_Azi_Snapshot_t data_J{2815,539};
  Alignment_Sight_Item jupiter( "Jupiter" );
  jupiter.set_pointing_information( JD,data_J);
  data_set->push_item( jupiter );

  Alt_Azi_Snapshot_t data_27{316,771};
  Alignment_Sight_Item dubhe( navigation_star::get_navigation_star_name(27) );
  dubhe.set_pointing_information( JD,data_27);
  data_set->push_item( dubhe );

  double least_error = std::numeric_limits<double>::max();
  double most_error = std::numeric_limits<double>::min();
  double best_offset = 0;
  for( double offset=0; offset<=360.0; offset += 12.0 ){
    scope->set_altitude_offset( offset );
    double error = compare_pair_by_pair(  data_set, 4 );
    if( error < least_error ){
      least_error = error;
      best_offset = offset;
    }
    if( error > most_error ){
      most_error = error;
    }
  }
  scope->set_altitude_offset( best_offset );

  linear_algebra_interface::simple_altazimuth_optimize_altitude_offset(data_set,scope);

  lcd->setCursor(0,1);
  lcd->print( " Max Error: ");
  lcd->print( scope->get_align_error_max(), 6 );
  lcd->setCursor(0,2);
  lcd->print( "Mean Error: " );
  lcd->print( scope->get_align_error_mean(), 6 );

  MicroSecondDelay::millisecond_delay(9000);  

  check_in_main_lcd(std::move(lcd));
}
