#include "controller.h"

#include "main.h"
#include "microsecond_delay.h"
#include "rtc_management.h"
#include "solar_system.h"
// #include <regex>
#include <cctype>
#include <cmath>
#include "matrix_keypad.h"

#include "AAElliptical.h"
#include "AA3DCoordinate.h"
#include "AA2DCoordinate.h"
#include "AAPrecession.h"
#include "AADynamicalTime.h"
#include "sexagesimal.h"
#include "extra_solar_transforms.h"
#include "ngc_objects.h"
#include "ngc_list.h"
#include "input_views.h"
#include "output_views.h"
#include "menu_views.h"
#include "binary_tidbits.h"
#include "navigation_star.h"
#include "AADate.h"
#include "build_date.h"
#include "alignment_sight_info.h"
#include "linear_algebra_interface.h"
#include "navigation_star.h"
#include "telescope_model.h"
// #include "starlist.h"
/* forward declarations */
void long_lat_dialog();
void navigation_star_menu_test();
void navigation_star_menu();
void solar_system_menu();
/* Display Julian Day of current time. */
void cmd_555()
{
  double JD;
  // JD = JD_timestamp();
  JD = JD_timestamp_pretty_good_000();
  auto lcd = check_out_main_lcd();
  lcd->clear();
  //lcd->setCursor(0,0);      
  lcd->home();
  MicroSecondDelay::millisecond_delay(1);
  lcd->print(JD, 8);
  MicroSecondDelay::millisecond_delay(6000);
  check_in_main_lcd(std::move(lcd));
}

void display_details(std::string name, CAAEllipticalPlanetaryDetails & details)
{
  int n;
  auto lcd = check_out_main_lcd();
  lcd->clear();
  lcd->home();
  MicroSecondDelay::millisecond_delay(1);
  lcd->setCursor(0, 0);
  n = lcd->print(name);
  while (n < 20) {
    n += lcd->print(" ");
  }
  lcd->setCursor(0, 1);
  n = lcd->print(" RA: ");
  n += lcd->print(sexagesimal::Sexagesimal(details.ApparentGeocentricRA).
		  to_string());
  while (n < 20) {
    n += lcd->print(" ");
  }
  lcd->setCursor(0, 2);
  n = lcd->print("DEC: ");
  n += lcd->
      print(sexagesimal::Sexagesimal(details.ApparentGeocentricDeclination).
	    to_string());
  while (n < 20) {
    n += lcd->print(" ");
  }
  lcd->setCursor(0, 3);
  n = lcd->print("Distance: ");
  n += lcd->print(details.ApparentGeocentricDistance, 4);
  while (n < 20)
    n += lcd->print(" ");
  MicroSecondDelay::millisecond_delay(9000);
  lcd->clear();
  MicroSecondDelay::millisecond_delay(1);
  check_in_main_lcd(std::move(lcd));
}

void calculate_and_display(std::string body)
{
  double JD = JD_timestamp_pretty_good_000();
  CAAEllipticalPlanetaryDetails details =
      solar_system::calculate_details(body, JD);
  display_details(body, details);
}

void moon_stuff()
{
  double JD = JD_timestamp_pretty_good_000();

  CAA3DCoordinate RA_Dec_Dist = solar_system::calculate_moon_RA_Dec_Dist(JD);

  int n;
  auto lcd = check_out_main_lcd();
  lcd->clear();
  lcd->home();
  MicroSecondDelay::millisecond_delay(1);
  lcd->setCursor(0, 0);
  n = lcd->print("Moon, Geocentric");
  while (n < 20) {
    n += lcd->print(" ");
  }
  lcd->setCursor(0, 1);
  n = lcd->print(" RA: ");
  n += lcd->print(sexagesimal::Sexagesimal(RA_Dec_Dist.X).to_string());
  while (n < 20) {
    n += lcd->print(" ");
  }
  lcd->setCursor(0, 2);
  n = lcd->print("DEC: ");
  n += lcd->print(sexagesimal::Sexagesimal(RA_Dec_Dist.Y).to_string());
  while (n < 20) {
    n += lcd->print(" ");
  }
  lcd->setCursor(0, 3);
  n = lcd->print("  R: ");
  n += lcd->print(RA_Dec_Dist.Z);
  n += lcd->print(" km");
  while (n < 20) {
    n += lcd->print(" ");
  }
  MicroSecondDelay::millisecond_delay(9000);
  lcd->clear();
  MicroSecondDelay::millisecond_delay(1);
  check_in_main_lcd(std::move(lcd));
}

bool is_two_digits(std::string & cmd)
{
  if (cmd.size() != 2) {
    return false;
  }
  if (!isdigit(cmd[0]))
    return false;
  if (!isdigit(cmd[1]))
    return false;
  return true;
}

bool is_four_digits(std::string & cmd)
{
  if (cmd.size() != 4) {
    return false;
  }
  if (!isdigit(cmd[0]))
    return false;
  if (!isdigit(cmd[1]))
    return false;
  if (!isdigit(cmd[2]))
    return false;
  if (!isdigit(cmd[3]))
    return false;
  return true;
}

void debug_print(std::string & msg)
{
  auto lcd = check_out_main_lcd();
  lcd->clear();
  lcd->home();
  MicroSecondDelay::millisecond_delay(1);
  lcd->setCursor(0, 0);
  int n = lcd->print("[");
  n += lcd->print(msg);
  n += lcd->print("]");
  while (n < 20) {
    n += lcd->print("]");
  }
  MicroSecondDelay::millisecond_delay(9000);
  lcd->clear();
  MicroSecondDelay::millisecond_delay(1);
  check_in_main_lcd(std::move(lcd));
}

void nav_star_command(int num)
{
  if( num<0 || num > 57 ){
    return;
  }
  double JD = JD_timestamp_pretty_good_000();
  CAA2DCoordinate RA_Dec = navigation_star::nav_star_RA_Dec(num, JD);
  int n;
  auto lcd = check_out_main_lcd();
  lcd->clear();
  lcd->home();
  MicroSecondDelay::millisecond_delay(1);
  lcd->setCursor(0, 0);
  n = lcd->print("Navigation Star: ");
  n += lcd->print(num);
  while (n < 20) {
    n += lcd->print(" ");
  }
  lcd->setCursor(0, 1);
  n = lcd->print(" RA: ");
  n += lcd->print(sexagesimal::Sexagesimal(RA_Dec.X).to_string());
  while (n < 20) {
    n += lcd->print(" ");
  }
  lcd->setCursor(0, 2);
  n = lcd->print("DEC: ");
  n += lcd->print(sexagesimal::Sexagesimal(RA_Dec.Y).to_string());
  while (n < 20) {
    n += lcd->print(" ");
  }
  lcd->setCursor(0, 3);
  n = lcd->print( "* "  );
  n += lcd->print(  navigation_star::get_navigation_star_name(num)  );
  n += lcd->print( " *"  );
  while (n < 20) {
    n += lcd->print(" ");
  }

  MicroSecondDelay::millisecond_delay(9000);
  lcd->clear();
  MicroSecondDelay::millisecond_delay(1);
  check_in_main_lcd(std::move(lcd));
}

#if 0
void ngc_object_command(int num)
{
  if( num <= 0 || num > 7840 ){
    return;
  }
  // double JD  = JD_timestamp_pretty_good_000();
  int index = ngc_objects::get_index( num );
  CAA2DCoordinate RA_Dec;
  sexagesimal::Sexagesimal RA;
  sexagesimal::Sexagesimal Dec;
  RA.set_binary_data(ngc_list[index].RA_data);
  Dec.set_binary_data(ngc_list[index].DEC_data);
  RA_Dec.X = RA.to_double();
  RA_Dec.Y = Dec.to_double();

  int n;
  auto lcd = check_out_main_lcd();
  lcd->clear();
  lcd->home();
  MicroSecondDelay::millisecond_delay(1);
  lcd->setCursor(0, 0);
  n = lcd->print("NGC Object: ");
  n += lcd->print(num);
  while (n < 20) {
    n += lcd->print(" ");
  }
  lcd->setCursor(0, 1);
  n = lcd->print(" RA: ");
  n += lcd->print(sexagesimal::Sexagesimal(RA_Dec.X).to_string());
  while (n < 20) {
    n += lcd->print(" ");
  }
  lcd->setCursor(0, 2);
  n = lcd->print("DEC: ");
  n += lcd->print(sexagesimal::Sexagesimal(RA_Dec.Y).to_string());
  while (n < 20) {
    n += lcd->print(" ");
  }
  lcd->setCursor(0, 3);
  n = lcd->print( "Magnitue = "  );
  n += lcd->print(  ngc_list[index].Vmag  );
  while (n < 20) {
    n += lcd->print(" ");
  }

  MicroSecondDelay::millisecond_delay(9000);
  lcd->clear();
  MicroSecondDelay::millisecond_delay(1);
  check_in_main_lcd(std::move(lcd));
}
#endif


void two_digit_command(int value)
{
  if (value >= 0 && value <= 57) {
    nav_star_command(value);
    return;
  } else {
    auto lcd = check_out_main_lcd();
    lcd->clear();
    lcd->home();
    MicroSecondDelay::millisecond_delay(1);
    lcd->setCursor(0, 0);
    lcd->print("TWO DIGIT ");
    lcd->print(value);
    lcd->print(";   ");
    MicroSecondDelay::millisecond_delay(9000);
    lcd->clear();
    MicroSecondDelay::millisecond_delay(1);
    check_in_main_lcd(std::move(lcd));
  }
}

void four_digit_command(int value)
{
  if (value >= 0 && value <= 7840) {
    //  ngc_object_command(value);
    return;
  } else {
    auto lcd = check_out_main_lcd();
    lcd->clear();
    lcd->home();
    MicroSecondDelay::millisecond_delay(1);
    lcd->setCursor(0, 0);
    lcd->print("FOUR DIGIT ");
    lcd->print(value);
    lcd->print(";   ");
    MicroSecondDelay::millisecond_delay(9000);
    lcd->clear();
    MicroSecondDelay::millisecond_delay(1);
    check_in_main_lcd(std::move(lcd));
  }
}

void dsc_controller::execute_early_experimental_command_implementations(std::string & cmd){
  if (cmd == "555") {
    cmd_555();
  } else if (cmd == "0") {
    calculate_and_display("Sun");
  } else if (cmd == "1") {
    calculate_and_display("Mercury");
  } else if (cmd == "2") {
    calculate_and_display("Venus");
  } else if (cmd == "3") {
    moon_stuff();
  } else if (cmd == "4") {
    calculate_and_display("Mars");
  } else if (cmd == "5") {
    calculate_and_display("Jupiter");
  } else if (cmd == "6") {
    calculate_and_display("Saturn");
  } else if (cmd == "7") {
    calculate_and_display("Uranus");
  } else if (cmd == "8") {
    calculate_and_display("Neptune");
  } else if (cmd == "9") {
    calculate_and_display("Pluto");
  } else if (is_two_digits(cmd)) {
    int value;
    int n = sscanf(cmd.data(), "%d", &value);
    if (n == 1) {
      two_digit_command(value);
    }
  } else if ( is_four_digits(cmd) ){
    int value;
    int n = sscanf(cmd.data(), "%d", &value);
    if (n == 1) {
      four_digit_command(value);
    }
  }else {
    // debug_print( "fall through" );
  }
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



/* Bright star catalog point to. B*Cdddd */
void bsc_point_to( std::string cmd ){
  assert_param( cmd[0] == 'B' );
  assert_param( cmd[1] == '*' );
  assert_param( cmd[2] == 'C' );

}


/*  B[0-9] command Solar system bodies. */
void B0123456789_point_to( std::string cmd ){
}

/* CA**** NGC ngc   */
void ngc_catalog_point_to( std::string cmd ){
}


/* AAA090 */
void angular_coordinate_view(){
  auto lcd = check_out_main_lcd();
  Simple_Altazimuth_Scope* scope = get_main_simple_telescope();
  std::unique_ptr<Angular_Coordinates_View> view = 
    std::unique_ptr<Angular_Coordinates_View>( new Angular_Coordinates_View( scope ) );
  
  while( !view->is_finished() ){
    lcd->setCursor(0,0);
    lcd = view->write_first_line(std::move(lcd));
    lcd->setCursor(0,1);
    lcd = view->write_second_line(std::move(lcd));
    lcd->setCursor(0,2);
    lcd = view->write_third_line(std::move(lcd));
    lcd->setCursor(0,3);
    lcd = view->write_fourth_line(std::move(lcd));
  }

  check_in_main_lcd(std::move(lcd));
}






/* AAA789 */
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

/* AAA234 */
void alignment_sight_test(){
  auto lcd = check_out_main_lcd();
  lcd->clear();
  lcd->noDisplay();
  MicroSecondDelay::millisecond_delay(1);
  lcd->display();
  lcd->home();
  lcd->print( "Test Alignment Menu" );
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

/* AAA58 */
void navigation_star_menu_test(){
  auto lcd = check_out_main_lcd();
  lcd->clear();
  lcd->noDisplay();
  MicroSecondDelay::millisecond_delay(1);
  lcd->display();

  lcd->home();
  lcd->print( "Test of LCD menu ..." );
  MicroSecondDelay::millisecond_delay(2000);

  std::unique_ptr<Numbered_List_Menu> star_menu =
    std::unique_ptr<Numbered_List_Menu>(new Numbered_List_Menu{ "*** STARS ***" });

  for( uint32_t num = 0; num<=navigation_star::NAVIGATION_LIST_MAX; ++num ){
    star_menu->insert_menu_item( num, navigation_star::get_navigation_star_name(num) );
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

  check_in_main_lcd(std::move(lcd));
}

/* AAA0317 */


void sexagesimal_test_dialog(){
  auto lcd = check_out_main_lcd();
  std::unique_ptr<Sexagesimal_Input_View> view =
    std::unique_ptr<Sexagesimal_Input_View>(new Sexagesimal_Input_View{ "Longitude DDD:MM:SS." });
  view->set_plus_char( 'W' );
  view->set_minus_char( 'E' );
  lcd->setCursor(0,0);
  lcd = view->write_first_line(std::move(lcd));
  //lcd->cursor();
  while( !view->is_finished() ){
    lcd->setCursor(0,1);
    lcd = view->write_second_line(std::move(lcd));
    int col = view->get_cursor_column();
    lcd->setCursor(col,1);
  }
  MicroSecondDelay::millisecond_delay(350);
  check_in_main_lcd(std::move(lcd));
}

/* AAA2490 */
void RA_and_Declination_dialog(){
  auto lcd = check_out_main_lcd();
  lcd->clear();
  std::unique_ptr<Sexagesimal_Input_View> RA_view =
    std::unique_ptr<Sexagesimal_Input_View>(new Sexagesimal_Input_View{ "RA HH:MM:SS.xxx" });
  RA_view->set_minus_char( '-' );  /* Not expected to be used. */
  RA_view->set_minus_char( ' ' );
  RA_view->set_msd_digits( 2 );
  RA_view->set_center_label(false);
  while( !RA_view->is_finished() ){
    lcd->setCursor(0,0);
    lcd = RA_view->write_first_line(std::move(lcd));
    lcd->setCursor(0,1);
    lcd = RA_view->write_second_line(std::move(lcd));
  }
  sexagesimal::Sexagesimal RA = RA_view->get_value();

  std::unique_ptr<Sexagesimal_Input_View> Dec_view =
    std::unique_ptr<Sexagesimal_Input_View>(new Sexagesimal_Input_View{ "Declination DD:MM:SS" });
  Dec_view->set_plus_char( 'N' );
  Dec_view->set_minus_char( 'S' );
  Dec_view->set_msd_digits( 2 );
  lcd->setCursor(0,2);
  lcd = Dec_view->write_first_line(std::move(lcd));
  while( !Dec_view->is_finished() ){
    lcd->setCursor(0,3);
    lcd = Dec_view->write_second_line(std::move(lcd));
  }
  sexagesimal::Sexagesimal declination = Dec_view->get_value();
  CAA2DCoordinate RA_Dec;
  RA_Dec.X = RA.to_double();
  RA_Dec.Y = declination.to_double();

  MicroSecondDelay::millisecond_delay(750);

  lcd->clear();
  lcd->setCursor( 0,0 );
  lcd->print( RA.to_string() + " " + declination.to_string() );

  std::unique_ptr< Pushto_Output_View > view = 
    std::unique_ptr<Pushto_Output_View >( new Pushto_Output_View( RA_Dec ) );
  view->set_label_2( "Current Epoch" );

  while( !view->is_finished() ){
    lcd->setCursor(0,0);
    lcd = view->write_first_line(std::move(lcd));
    lcd->setCursor(0,1);
    lcd = view->write_second_line(std::move(lcd));
    lcd->setCursor(0,2);
    lcd = view->write_third_line(std::move(lcd));
    lcd->setCursor(0,3);
    lcd = view->write_fourth_line(std::move(lcd));
  }

  check_in_main_lcd(std::move(lcd));
}



/* AAA1950 
 * For Burnham's Guide.
 * 
 * Burnham gives the coordinates in this format:
 * 22115s2119 = RA 22h 11.5m: Dec -21(deg) 19(min).
 * 06078n4844 = RA  6h 07.8m: Dec +48(deg) 44(min).
 *
 */
void Burnham_Handbook_Point_To(){
  auto lcd = check_out_main_lcd();
  lcd->clear();
  std::unique_ptr<Burnham_Format_Input_View> input_view =
    std::unique_ptr<Burnham_Format_Input_View>(new Burnham_Format_Input_View{ });
  while( !input_view->is_finished() ){
    lcd->setCursor(0,0);
    lcd = input_view->write_first_line(std::move(lcd));
    lcd->setCursor(0,1);
    lcd = input_view->write_second_line(std::move(lcd));
  }
  CAA2DCoordinate RA_Dec = input_view->get_RA_and_Dec();
  double JD = JD_timestamp_pretty_good_000();
  RA_Dec = burnham_correct(RA_Dec, JD);

  std::unique_ptr< Pushto_Output_View > pushto_view = 
    std::unique_ptr<Pushto_Output_View >( new Pushto_Output_View( RA_Dec ) );

  pushto_view->set_label_1( "Push To Data" );

  while( !pushto_view->is_finished() ){
    lcd->setCursor(0,2);
    lcd = pushto_view->write_first_line(std::move(lcd));
    lcd->setCursor(0,3);
    lcd = pushto_view->write_second_line(std::move(lcd));
  }

  check_in_main_lcd(std::move(lcd));
}






void dsc_controller::AAA_command(std::string & cmd){
  if( cmd == "AAA1032" ){
    auto lcd = check_out_main_lcd();
    lcd->row_switcheroo();
    check_in_main_lcd(std::move(lcd));
  }else if( cmd == "AAA0" ){
    auto lcd = check_out_main_lcd();
    lcd->VARIOUS_HACKS();
    check_in_main_lcd(std::move(lcd));    
  }else if( cmd == "AAA555" ){
    // This command has been moved to command_actions.cpp
    // display_time_view();
  }else if( cmd == "AAA556" ){
    // add_seconds_dialog();
  }else if( cmd == "AAA554" ){
    // subtract_seconds_dialog();
  }else if( cmd == "AAA360" ){
    sexagesimal_test_dialog();
  }else if( cmd == "AAA720" ){
    // long_lat_dialog();
  }else if( cmd == "AAA58" ){
    navigation_star_menu_test();
  }else if( cmd == "AAA0317" ){
    // countdown_to_Saint_Patricks_Day();
  }else if( cmd == "AAA234" ){
    alignment_sight_test();
  }else if( cmd == "AAA456" ){
    make_optimized_solution_test();
  }else if( cmd == "AAA789" ){
    pre_fab_example_test();
  }else if( cmd == "AAA987" ){
    //clear_sight_data();
  }else if( cmd == "AAA090" ){
    angular_coordinate_view();
  }else if( cmd == "AAAB*C" ){
    /* Bright star catalog point to action. */;
  }else if( cmd == "AAA2490" ){
    RA_and_Declination_dialog();
  }else if( cmd == "AAA1950" ){
    Burnham_Handbook_Point_To();
  }

}

void dsc_controller::AA_command(std::string & cmd){
  if( cmd.size() < 3 ){
    return;
  }
  switch( cmd[2] ){
  case 'A':
    dsc_controller::AAA_command(cmd);
    return;
  default:
    return;
  }
}

void dsc_controller::BA_command(std::string &cmd){}
void dsc_controller::BB_command(std::string &cmd){}
void dsc_controller::BC_command(std::string &cmd){}


void dsc_controller::CA_command(std::string &cmd){}
void dsc_controller::CB_command(std::string &cmd){}
void dsc_controller::CC_command(std::string &cmd){}





void dsc_controller::A_command(std::string & cmd){
  if( cmd.size() < 2 ){
    return;
  }
  switch( cmd[1] ){
  case 'A':
    dsc_controller::AA_command(cmd);
    return;
  default:
    return;
  }
}

void dsc_controller::B_command(std::string & cmd){
  if( cmd.size() < 2 ){
    return;
  }
  switch( cmd[1] ){
  case 'A':
    dsc_controller::BA_command(cmd);
    return;
  case 'B':
    dsc_controller::BB_command(cmd);
    return;
  case 'C':
    dsc_controller::BC_command(cmd);
    return;
  case '*':
    if( cmd[2] == 'C' ){
      /* cmd begins with B*C (Bright Star Catalog) */
      bsc_point_to( cmd );
    }
    return;
  case'0':
  case'1':
  case'2':
  case'4':
  case'5':
  case'6':
  case'7':
  case'8':
  case'9':
    B0123456789_point_to( cmd );
    return;
  default:
    return;
  }
}

void dsc_controller::C_command(std::string & cmd){
  if( cmd.size() < 2 ){
    return;
  }
  switch( cmd[1] ){
  case 'A':
    dsc_controller::CA_command(cmd);
    /* CA => Catalog (NGC) */
    ngc_catalog_point_to( cmd );
    return;
  case 'B':
    dsc_controller::CB_command(cmd);
    return;
  case 'C':
    dsc_controller::CC_command(cmd);
    return;
  default:
    return;
  }
}

void dsc_controller::do_execute_command(std::string & cmd)
{
  if( cmd.size() < 1 ){
    return;
  }
  switch( cmd[0] ){
  case 'A':
    dsc_controller::A_command(cmd);
    return;
  case 'B':
    dsc_controller::B_command(cmd);
    return;
  case 'C':
    dsc_controller::C_command(cmd);
    return;

  default:
    dsc_controller::execute_early_experimental_command_implementations(cmd);
  }
} 

static Character_Reciever* current_character_reciever = nullptr;
/* This may be called by any funtion with arguemnt a pointer to any class implementing
 * the Character_Reciever interface.  That is, at any time the 
 * stream of input characters may be usurped.  This is not expected to 
 * be a problem.  If this becomes a problem the plan is to deal with it 
 * by implementing a subscriber pattern.
 * 
 * This lax treatment stands in contrast to the controlled 
 * treatment of the main LCD display where conflicting print() calls 
 * would be chaotic.
 **/
void dsc_controller::set_character_reciever(Character_Reciever* cr){
  current_character_reciever = cr;
}

Character_Reciever* dsc_controller::get_character_reciever(){
  return current_character_reciever;
}


/* The default treatment of input characters is to take them as forming command language words. */
void default_put_char(char c){
  if (c == keypad_backspace_char) {		/* behave like a Backspace. */
    if (dsc_controller::char_buffer.size() > 0) {
      dsc_controller::char_buffer.pop_back();
    }
    return;
  } else if (c == keypad_return_char) {      /* behave like a [Enter|Return] */
    int length = dsc_controller::char_buffer.size();
    std::string buffer_copy = "";
    for (int i = 0; i < length; ++i) {
      buffer_copy += dsc_controller::char_buffer[i];
    }
    dsc_controller::char_buffer.clear();
    dsc_controller::cmd_buffer.push_back(buffer_copy);
    return;
  } else {
    if (dsc_controller::char_buffer.size() < 20) {
      dsc_controller::char_buffer.push_back(c);
    } 
  }
}

/* Functions in namespace dsc_controller. */
void dsc_controller::put_char(char c)
{
  if( current_character_reciever ){
    current_character_reciever->put_char(c);
  }else{
    default_put_char(c);
  }
}

std::vector < char >dsc_controller::char_buffer {24};
std::vector < std::string >dsc_controller::cmd_buffer {8};

std::string dsc_controller::pop_cmd_buffer(){
  std::string item = cmd_buffer[0];
  /* Simulate FIFO  action  [at cost of O(n)]. 
   * @TODO see if there is a proper FIFO buffer that compiles for uC.  
   **/
  for (auto p = cmd_buffer.begin(); (p + 1) != cmd_buffer.end(); ++p) {
    *p = *(p + 1);
  }
  cmd_buffer.pop_back();
  return item;
}

