#include "command_actions.h"
#include "flex_lexer.h"
#include "flex_lexer_declare.h"
#include "main.h"
#include "output_views.h"
#include "input_views.h"
#include "menu_views.h"
#include "microsecond_delay.h"
#include "rtc_management.h"
#include "binary_tidbits.h"
#include "solar_system.h"
#include "ngc_objects.h"
#include "ngc_list.h"
#include "starlist.h"
#include "extra_solar_transforms.h"

#include "AADynamicalTime.h"




void scan_for_action(std::string command)
{
  auto buffer_state = yy_scan_string(command.data());
  yylex();
  yy_delete_buffer(buffer_state);
}


void ngc_point_to_action(char *yytext, int yyleng)
{
  int ngc_num;
  int n = sscanf(yytext+2, "%d", &ngc_num);
  if (n != 1) {
    return; /* man sscanf indicates the return value is the number of succesfully assigned. */
  }
  if( ngc_num <= 0 || ngc_num > 7840 ){
    return;
  }
  int index = ngc_objects::get_index( ngc_num );
  /* get_index(ngc_num) fails by returning -1 */
  /* This will be little tested since I am inclined to include the entirety of the NGC. */
  if( index == -1 ){
    auto lcd = check_out_main_lcd();
    lcd->setCursor(0,0);
    n = 0;
    n += lcd->print( "NGC object " );
    n += lcd->print( ngc_num  );
    while (n < 20) {
      n += lcd->print(" ");
    }
    lcd->setCursor(0,1);
    n = 0;
    n += lcd->print( "not onboard" );
    while (n < 20) {
      n += lcd->print(" ");
    }
    MicroSecondDelay::millisecond_delay(9000);
    check_in_main_lcd(std::move(lcd));
    return;
  }
  CAA2DCoordinate RA_Dec;
  sexagesimal::Sexagesimal RA;
  sexagesimal::Sexagesimal Dec;
  RA.set_binary_data(ngc_list[index].RA_data);
  Dec.set_binary_data(ngc_list[index].DEC_data);
  RA_Dec.X = RA.to_double();
  RA_Dec.Y = Dec.to_double();
  /* For stars we would have a proper motion correction here. 
   * Possibly some open clusers in the NGC have a non-negligible proper motion. 
   * Check that out.
   */
  double JD = JD_timestamp_pretty_good_000();
  double deltaT = CAADynamicalTime::DeltaT( JD );
  RA_Dec = apply_aberration( RA_Dec, JD + deltaT/86400.0 );
  RA_Dec = precession_and_nutation_correct_from_mean_eqinox( RA_Dec, JD );

  auto lcd = check_out_main_lcd();

  lcd->clear();
  lcd->noDisplay();
  MicroSecondDelay::millisecond_delay(1);
  lcd->display();
  lcd->home();
  lcd->setCursor(0,0);
  n = 0;
  n += lcd->print( "Point to NGC " );
  n += lcd->print( ngc_num );
  while (n < 20) {
    n += lcd->print(" ");
  }
  std::unique_ptr< Pushto_Output_View > view = 
    std::unique_ptr<Pushto_Output_View >( new Pushto_Output_View( RA_Dec ) );
  while( !view->is_finished() ){
    lcd->setCursor(0,1);
    lcd = view->write_first_line(std::move(lcd));
    lcd->setCursor(0,2);
    lcd = view->write_second_line(std::move(lcd));
    lcd->setCursor(0,3);
    lcd = view->write_third_line(std::move(lcd));
  }
  check_in_main_lcd(std::move(lcd));
}

void bsc_point_to_action(char *yytext, int yyleng)
{
  int bsc_num;
  int n = sscanf( yytext+3, "%d", &bsc_num);
  if( n != 1 ){
    return;
  }
  CAA2DCoordinate RA_Dec;
  int index = -1;
  for (int i = 0; i < STARLIST_SIZE; ++i) {
    if (starlist[i].BSCnum == bsc_num) {
      index = i;
      break;
    }
  }
  if( index < 0 ){
    auto lcd = check_out_main_lcd();
    lcd->setCursor(0,0);
    n = 0;
    n += lcd->print( "B*C star " );
    n += lcd->print( bsc_num );
    while (n < 20) {
      n += lcd->print(" ");
    }
    lcd->setCursor(0,1);
    n = 0;
    n += lcd->print( "not onboard" );
    while (n < 20) {
      n += lcd->print(" ");
    }
    MicroSecondDelay::millisecond_delay(9000);
    check_in_main_lcd(std::move(lcd));
    return;
  }

  double JD = JD_timestamp_pretty_good_000();
  double days = JD - solar_system::J2000_0;

  RA_Dec = proper_motion_adjusted_position( starlist[index], days );
  double deltaT = CAADynamicalTime::DeltaT( JD );
  RA_Dec = apply_aberration( RA_Dec, JD + deltaT/86400.0 );
  RA_Dec = precession_and_nutation_correct_from_mean_eqinox( RA_Dec, JD );

  auto lcd = check_out_main_lcd();

  lcd->clear();
  lcd->noDisplay();
  MicroSecondDelay::millisecond_delay(1);
  lcd->display();
  lcd->home();
  lcd->setCursor(0,0);
  n = 0;
  n += lcd->print( "Point to star " );
  n += lcd->print( bsc_num );
  while (n < 20) {
    n += lcd->print(" ");
  }
  std::unique_ptr< Pushto_Output_View > view = 
    std::unique_ptr<Pushto_Output_View >( new Pushto_Output_View( RA_Dec ) );
  while( !view->is_finished() ){
    lcd->setCursor(0,1);
    lcd = view->write_first_line(std::move(lcd));
    lcd->setCursor(0,2);
    lcd = view->write_second_line(std::move(lcd));
    lcd->setCursor(0,3);
    lcd = view->write_third_line(std::move(lcd));
  }
  check_in_main_lcd(std::move(lcd));
}

void solar_system_point_to_action(char *yytext, int yyleng)
{
  int body_num;
  int n;
  n = sscanf(yytext + 1, "%d", &body_num);
  if (n != 1) {
    return;
  }

  std::string body_name = solar_system::solar_system_body_name(body_num);
  double JD = JD_timestamp_pretty_good_000();
  CAA2DCoordinate RA_Dec;
  if (body_num == 3) {
    CAA3DCoordinate RA_Dec_Dist = solar_system::calculate_moon_RA_Dec_Dist(JD);
    RA_Dec.X = RA_Dec_Dist.X;
    RA_Dec.X = RA_Dec_Dist.Y;
  } else {
    CAAEllipticalPlanetaryDetails details =
	solar_system::calculate_details(body_name, JD);
    RA_Dec.X = details.ApparentGeocentricRA;
    RA_Dec.Y = details.ApparentGeocentricDeclination;
  }
  auto lcd = check_out_main_lcd();

  lcd->clear();
  lcd->noDisplay();
  MicroSecondDelay::millisecond_delay(1);
  lcd->display();
  lcd->home();
  lcd->setCursor(0, 0);
  n = 0;
  n += lcd->print("Point to ");
  n += lcd->print(body_name);
  while (n < 20) {
    n += lcd->print(" ");
  }
  std::unique_ptr < Pushto_Output_View > view =
      std::unique_ptr < Pushto_Output_View > (new Pushto_Output_View(RA_Dec));
  while (!view->is_finished()) {
    lcd->setCursor(0, 1);
    lcd = view->write_first_line(std::move(lcd));
    lcd->setCursor(0, 2);
    lcd = view->write_second_line(std::move(lcd));
    lcd->setCursor(0, 3);
    lcd = view->write_third_line(std::move(lcd));
  }
  check_in_main_lcd(std::move(lcd));

}

void almanac_star_point_to_action(char *yytext, int yyleng)
{
  int nav_num = -1;
  int n;
  n = sscanf(yytext + 2, "%d", &nav_num);
  if (n != 1) {
    return;
  }
  if( nav_num > 57 || nav_num<0 ){
    /*  It would be nice to put up an error message. */
    return;
  }
  std::string star_name = navigation_star::get_navigation_star_name( nav_num );
  int bsc_num = navigation_star::nav2bsc[nav_num];
  int index = -1;
  for (int i = 0; i < STARLIST_SIZE; ++i) {
    if (starlist[i].BSCnum == bsc_num) {
      index = i;
      break;
    }
  }
  if( index == -1 ){
    return;
  }
  CAA2DCoordinate RA_Dec;


  double JD = JD_timestamp_pretty_good_000();
  double days = JD - solar_system::J2000_0;
  RA_Dec = proper_motion_adjusted_position( starlist[index], days );
  double deltaT = CAADynamicalTime::DeltaT( JD );
  RA_Dec = apply_aberration( RA_Dec, JD + deltaT/86400.0 );
  RA_Dec = precession_and_nutation_correct_from_mean_eqinox( RA_Dec, JD );

  auto lcd = check_out_main_lcd();

  lcd->clear();
  lcd->noDisplay();
  MicroSecondDelay::millisecond_delay(1);
  lcd->display();
  lcd->home();
  lcd->setCursor(0,0);
  n += lcd->print( "Point to " );
  n += lcd->print( star_name );
  while (n < 20) {
    n += lcd->print(" ");
  }


  std::unique_ptr< Pushto_Output_View > view = 
    std::unique_ptr<Pushto_Output_View >( new Pushto_Output_View( RA_Dec ) );
  view->set_label_1( star_name );
  while( !view->is_finished() ){
    lcd->setCursor(0,1);
    lcd = view->write_first_line(std::move(lcd));
    lcd->setCursor(0,2);
    lcd = view->write_second_line(std::move(lcd));
    lcd->setCursor(0,3);
    lcd = view->write_third_line(std::move(lcd));
  }
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






void default_action(char *yytext, int yyleng)
{

}

/* AAA555 */
void display_current_time_view()
{
  auto lcd = check_out_main_lcd();
  std::unique_ptr < Current_Time_View > view =
      std::unique_ptr < Current_Time_View > (new Current_Time_View());

  while (!view->is_finished()) {
    lcd->setCursor(0, 0);
    lcd = view->write_first_line(std::move(lcd));
    lcd->setCursor(0, 1);
    lcd = view->write_second_line(std::move(lcd));
    lcd->setCursor(0, 2);
    lcd = view->write_third_line(std::move(lcd));
    lcd->setCursor(0, 3);
    lcd = view->write_fourth_line(std::move(lcd));
  }
  check_in_main_lcd(std::move(lcd));
}

/* AAA556 */
void add_seconds_dialog()
{
  auto lcd = check_out_main_lcd();
  std::unique_ptr < Integer_Input_View > view =
      std::unique_ptr < Integer_Input_View > (new Integer_Input_View {
					      "RTC add seconds"});
  while (!view->is_finished()) {
    lcd->setCursor(0, 0);
    lcd = view->write_first_line(std::move(lcd));
    lcd->setCursor(0, 1);
    lcd = view->write_second_line(std::move(lcd));
  }
  if (view->get_value() != 0) {
    RTC_add_seconds(static_cast < double >(view->get_value()));
  }
  MicroSecondDelay::millisecond_delay(350);
  check_in_main_lcd(std::move(lcd));
}

/* AAA554 */
void subtract_seconds_dialog()
{
  auto lcd = check_out_main_lcd();
  std::unique_ptr < Integer_Input_View > view =
      std::unique_ptr < Integer_Input_View > (new Integer_Input_View {
					      "RTC subtact seconds"});
  while (!view->is_finished()) {
    lcd->setCursor(0, 0);
    lcd = view->write_first_line(std::move(lcd));
    lcd->setCursor(0, 1);
    lcd = view->write_second_line(std::move(lcd));
  }
  if (view->get_value() != 0) {
    RTC_add_seconds(-static_cast < double >(view->get_value()));
  }
  MicroSecondDelay::millisecond_delay(350);
  check_in_main_lcd(std::move(lcd));
}

void long_lat_dialog()
{
  auto lcd = check_out_main_lcd();
  std::unique_ptr < Sexagesimal_Input_View > long_view =
      std::unique_ptr < Sexagesimal_Input_View > (new Sexagesimal_Input_View {
						  "Longitude DDD:MM:SS."});
  /* 
   * Kstars and many others express west longitude as a  negative angle. 
   * Astronomical Algorithims has longitude measured positively westward.
   * See box on page 93. 
   * 
   * The convention used by this project is expressed in "binary_tidbits.h".
   */
  if (binary_tidbits::west_longitude_is_positive()) {
    long_view->set_minus_char('E');
    long_view->set_plus_char('W');
  } else {
    long_view->set_minus_char('W');
    long_view->set_plus_char('E');
  }
  lcd->setCursor(0, 0);
  lcd = long_view->write_first_line(std::move(lcd));
  while (!long_view->is_finished()) {
    lcd->setCursor(0, 1);
    lcd = long_view->write_second_line(std::move(lcd));
  }
  sexagesimal::Sexagesimal longitude = long_view->get_value();
  save_backup_domain_longitude(longitude);
  MicroSecondDelay::millisecond_delay(350);

  std::unique_ptr < Sexagesimal_Input_View > lat_view =
      std::unique_ptr < Sexagesimal_Input_View > (new Sexagesimal_Input_View {
						  "Latitude   DD:MM:SS."}
  );
  lat_view->set_plus_char('N');
  lat_view->set_minus_char('S');
  lat_view->set_msd_digits(2);
  lcd->setCursor(0, 2);
  lcd = lat_view->write_first_line(std::move(lcd));
  while (!lat_view->is_finished()) {
    lcd->setCursor(0, 3);
    lcd = lat_view->write_second_line(std::move(lcd));
  }
  sexagesimal::Sexagesimal latitude = lat_view->get_value();
  save_backup_domain_latitude(latitude);
  MicroSecondDelay::millisecond_delay(350);
  check_in_main_lcd(std::move(lcd));
}

void countdown_to_Saint_Patricks_Day()
{
  /* If you like this, buy me a Guiness. */
  auto lcd = check_out_main_lcd();
  lcd->clear();
  lcd->noDisplay();
  MicroSecondDelay::millisecond_delay(1);
  lcd->display();

  double JD_now = JD_timestamp_pretty_good_000();
  CAADate now {
  JD_now, true};
  long year = now.Year();
  CAADate st_patty_day_current {
  year, 3, 17, 0, 0, 0, true};
  CAADate st_patty_day_next {
  year + 1, 3, 17, 0, 0, 0, true};
  double target_jd;
  if (JD_now < st_patty_day_current.Julian()) {
    target_jd = st_patty_day_current.Julian();
  } else {
    target_jd = st_patty_day_next.Julian();
  }

  std::unique_ptr < Countdown_View > view =
      std::unique_ptr < Countdown_View > (new Countdown_View {
					  }
  );

  view->set_label_1("Saint Patrick's Day");
  view->set_label_2("Countdown Clock");
  view->set_countdown_moment(target_jd);

  while (!view->is_finished()) {
    view->update();
    lcd->setCursor(0, 0);
    lcd = view->write_first_line(std::move(lcd));
    lcd->setCursor(0, 1);
    lcd = view->write_second_line(std::move(lcd));
    lcd->setCursor(0, 2);
    lcd = view->write_third_line(std::move(lcd));
    lcd->setCursor(0, 3);
    lcd = view->write_fourth_line(std::move(lcd));
  }

  check_in_main_lcd(std::move(lcd));
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

void planetary_details_view_action( char* yytext, int yyleng ){
  int body_num;
  int n;
  n = sscanf(yytext + 2, "%d", &body_num);
  if (n != 1) {
    return;
  }

  std::string body_name = solar_system::solar_system_body_name(body_num);
  double JD = JD_timestamp_pretty_good_000();
  CAA2DCoordinate RA_Dec;
  if (body_num == 3) {
    CAA3DCoordinate RA_Dec_Dist = solar_system::calculate_moon_RA_Dec_Dist(JD);
    RA_Dec.X = RA_Dec_Dist.X;
    RA_Dec.Y = RA_Dec_Dist.Y;
  } else {
    CAAEllipticalPlanetaryDetails details =
	solar_system::calculate_details(body_name, JD);
    RA_Dec.X = details.ApparentGeocentricRA;
    RA_Dec.Y = details.ApparentGeocentricDeclination;
  }
  /******************************
  std::unique_ptr<Planetary_Details_View> view =
    std::unique_ptr<Planetary_Details_View>(new Plantary_Details_View(body_num) );
  ********************************/
  std::unique_ptr< Planetary_Details_View > view = 
    std::unique_ptr<Planetary_Details_View >( new Planetary_Details_View( body_num ) );


  auto lcd = check_out_main_lcd();
  {


  }
  check_in_main_lcd(std::move(lcd));
}
