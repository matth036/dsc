#include "sight_data_command_actions.h"

#include "main.h"
#include "AADate.h"
#include "microsecond_delay.h"
#include "rtc_management.h"
#include "navigation_star.h"
#include "output_views.h"
#include "menu_views.h"
#include "input_views.h"


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



/* AAA234 */
void alignment_sight_test(){
  auto lcd = check_out_main_lcd();
  lcd->clear();
  lcd->noDisplay();
  MicroSecondDelay::millisecond_delay(1);
  lcd->display();
  lcd->home();
  lcd->print( "Alignment Sight Menu" );
  MicroSecondDelay::millisecond_delay(2000);
  Alignment_Data_Set* data_set = get_main_sight_data();
  if( !data_set->has_longitude_and_latitude() ){
    sexagesimal::Sexagesimal longitude = get_backup_domain_longitude();
    sexagesimal::Sexagesimal latitude = get_backup_domain_latitude();
    if( longitude.get_data() == 0 || latitude.get_data() == 0 ){
      check_in_main_lcd(std::move(lcd));
      long_lat_dialog();
      lcd = check_out_main_lcd();
      longitude = get_backup_domain_longitude();
      latitude =  get_backup_domain_latitude();
    }
    data_set->set_longitude_and_latitude( longitude, latitude );
  }
  std::unique_ptr<Alignment_Sights_View> sight_view =
    std::unique_ptr<Alignment_Sights_View>(new Alignment_Sights_View{ data_set });
  while( !sight_view->is_finished() ){
    lcd->setCursor(0,0);
    lcd = sight_view->write_first_line(std::move(lcd));
    lcd->setCursor(0,1);
    lcd = sight_view->write_second_line(std::move(lcd));
    lcd->setCursor(0,2);
    lcd = sight_view->write_third_line(std::move(lcd));
    lcd->setCursor(0,3);
    lcd = sight_view->write_fourth_line(std::move(lcd));
    if( sight_view->prompt_for_new_star_sight() ){
      check_in_main_lcd(std::move(lcd));
      navigation_star_menu();
      sight_view->clear_prompts();
      lcd = check_out_main_lcd();
    }
    if( sight_view->prompt_for_new_planet_sight() ){
      check_in_main_lcd(std::move(lcd));
      solar_system_menu();
      sight_view->clear_prompts();
      lcd = check_out_main_lcd();
    }
    if( sight_view->delete_item_with_confirm() ){
      check_in_main_lcd(std::move(lcd));
      delete_sight_item_with_confirm();
      sight_view->clear_prompts();
      lcd = check_out_main_lcd();
    }
  }
  check_in_main_lcd(std::move(lcd));
}



void navigation_star_menu(){
  auto lcd = check_out_main_lcd();
  lcd->clear();
  lcd->noDisplay();
  MicroSecondDelay::millisecond_delay(1);
  lcd->display();

  std::unique_ptr<Numbered_List_Menu> star_menu =
    std::unique_ptr<Numbered_List_Menu>(new Numbered_List_Menu{ "*** STARS ***" });

  double JD = JD_timestamp_pretty_good_000();
  Alignment_Data_Set* data_set = get_main_sight_data();

  for( uint32_t num = 0; num<=navigation_star::NAVIGATION_LIST_MAX; ++num ){
    std::string star_name = navigation_star::get_navigation_star_name(num);
    bool success = false;
    CAA2DCoordinate RA_and_Dec = calulate_RA_and_Dec(star_name, JD, success );
    if( success ){
      CAA2DCoordinate azi_alt = data_set->azimuth_altitude( RA_and_Dec, JD, 
							   Alignment_Sight_Item::DEFAULT_PRESSURE,
							   Alignment_Sight_Item::DEFAULT_TEMPERATURE);
      if( azi_alt.Y>0 ){
	star_menu->insert_menu_item( num, star_name );
      }
    }
    // star_menu->insert_menu_item( num, navigation_star::get_navigation_star_name(num) );
  }


  while( !star_menu->is_finished() ){
    lcd->setCursor(0,0);
    lcd = star_menu->write_first_line(std::move(lcd));
    lcd->setCursor(0,1);
    lcd = star_menu->write_second_line(std::move(lcd));
    lcd->setCursor(0,2);
    lcd = star_menu->write_third_line(std::move(lcd));
    lcd->setCursor(0,3);
    lcd = star_menu->write_fourth_line(std::move(lcd));
  }
  std::string selection = star_menu->get_selection_text();
  Simple_Altazimuth_Scope* telescope = get_main_simple_telescope();
  if( selection != "" ){
    check_in_main_lcd(std::move(lcd));
    alignment_prompt( telescope, selection );
    lcd = check_out_main_lcd();
  }
  check_in_main_lcd(std::move(lcd));
}



void solar_system_menu(){
  auto lcd = check_out_main_lcd();
  lcd->clear();
  lcd->noDisplay();
  MicroSecondDelay::millisecond_delay(1);
  lcd->display();
  std::unique_ptr<Numbered_List_Menu> ss_menu =
    std::unique_ptr<Numbered_List_Menu>(new Numbered_List_Menu{ "Solar System" });
  double JD = JD_timestamp_pretty_good_000();
  Alignment_Data_Set* data_set = get_main_sight_data();

  std::vector<std::string> bodies{"Sun","Mercury","Venus","Moon","Mars","Jupiter","Saturn","Uranus","Neptune" };

  for( uint32_t num = 0; num<bodies.size(); ++num ){
    bool success = false;
    CAA2DCoordinate RA_and_Dec = calulate_RA_and_Dec(bodies[num], JD, success );
    if( success ){
      CAA2DCoordinate azi_alt = data_set->azimuth_altitude( RA_and_Dec, JD,
							    Alignment_Sight_Item::DEFAULT_PRESSURE,
							    Alignment_Sight_Item::DEFAULT_TEMPERATURE);
      if( azi_alt.Y > 0 ){
	ss_menu->insert_menu_item( num, bodies[num] );
      }else{
	ss_menu->insert_menu_item( num, "(" + bodies[num] + ")" );
      }
    }
    // star_menu->insert_menu_item( num, navigation_star::get_navigation_star_name(num) );
  }
  while( !ss_menu->is_finished() ){
    lcd->setCursor(0,0);
    lcd = ss_menu->write_first_line(std::move(lcd));
    lcd->setCursor(0,1);
    lcd = ss_menu->write_second_line(std::move(lcd));
    lcd->setCursor(0,2);
    lcd = ss_menu->write_third_line(std::move(lcd));
    lcd->setCursor(0,3);
    lcd = ss_menu->write_fourth_line(std::move(lcd));
  }
  std::string selection = ss_menu->get_selection_text();
  Simple_Altazimuth_Scope* telescope = get_main_simple_telescope();
  if( selection != "" ){
    check_in_main_lcd(std::move(lcd));
    alignment_prompt( telescope, selection );
    lcd = check_out_main_lcd();
  }
  check_in_main_lcd(std::move(lcd));
}

void delete_sight_item_with_confirm(){
  std::unique_ptr<Confirm_Input_View> confirm =
    std::unique_ptr<Confirm_Input_View>(new Confirm_Input_View{} );
  auto lcd = check_out_main_lcd();

  while( !confirm->is_finished() ){
    lcd->setCursor(0,2);
    lcd = confirm->write_first_line(std::move(lcd));
    lcd->setCursor(0,3);
    lcd = confirm->write_second_line(std::move(lcd));
  }


  check_in_main_lcd(std::move(lcd));
}



void alignment_prompt(Simple_Altazimuth_Scope* scope, std::string object ){
  auto lcd = check_out_main_lcd();
  std::unique_ptr<Alignment_Timestamp_Prompt> prompt_view = 
    std::unique_ptr<Alignment_Timestamp_Prompt>(
						new Alignment_Timestamp_Prompt(scope,object)  );
  while( !prompt_view->is_finished() ){
    lcd->setCursor(0,0);
    lcd = prompt_view->write_first_line(std::move(lcd));
    lcd->setCursor(0,1);
    lcd = prompt_view->write_second_line(std::move(lcd));
    lcd->setCursor(0,2);
    lcd = prompt_view->write_third_line(std::move(lcd));
    lcd->setCursor(0,3);
    lcd = prompt_view->write_fourth_line(std::move(lcd));
  }
  if( prompt_view->has_data() ){
    //    get_main_sight_data()
    /* Here is where we collate the pieces of information associated 
     * with an alignment sight.
     **/
    Alignment_Sight_Item item{ prompt_view->get_object_name() };
    double timestamp = prompt_view->get_timestamp();   /* A Julian date. */
    Alt_Azi_Snapshot_t encoder_data = prompt_view->get_encoder_data();
    item.set_pointing_information( timestamp, encoder_data );
    /* Alignment_Sight_Item also has fields for the temperature and pressure.  
     * These are set to default values that they are non zero and somewhat arbitrary 
     * but not insane.
     **/
    // Alignment_Data_Set
    get_main_sight_data()->push_item( item );
  }
  check_in_main_lcd(std::move(lcd));
}

/* AAA456 */
/* This is now believed to be funtional and correct if not optimal.
 *
 * compare_pair_by_pair() is slow for large data sets. O(N^2). 
 * Introduced a second argument to limit the size considered.
 *
 */
void make_optimized_solution_test(){
  auto lcd = check_out_main_lcd();
  lcd->clear();
  lcd->noDisplay();
  MicroSecondDelay::millisecond_delay(1);
  lcd->display();
  lcd->home();
  lcd->setCursor(0,0);
  lcd->print( "Extracting Solution" );
  //  MicroSecondDelay::millisecond_delay(2000);  
  Simple_Altazimuth_Scope* scope = get_main_simple_telescope();
  Alignment_Data_Set*  data_set =  get_main_sight_data();

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
