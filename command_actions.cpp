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

#include "ngc_list_access.h"
#include "starlist_access.h"
#include "extra_solar_transforms.h"
#include "telescope_model.h"
#include "build_date.h"
#include "AADynamicalTime.h"
#include "horizontal_parallax.h"




void scan_for_action(std::string command)
{
  auto buffer_state = yy_scan_string(command.data());
  yylex();
  yy_delete_buffer(buffer_state);
}

/* These test functions will not appear in header files. For now. */
void information_view_test(){
  auto lcd = check_out_main_lcd();
  auto view = std::unique_ptr < Information_View > (new Information_View());
  view->set_text_1( "Alpha" );
  view->set_text_2( "Bravo" );
  view->set_text_3( "Charlie" );
  view->set_text_4( "Delta" );

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


void moon_parallax_test(){
  auto lcd = check_out_main_lcd();
  auto view = std::unique_ptr < Information_View > (new Information_View());
  double JD = JD_timestamp_pretty_good_000();
  CAA3DCoordinate ra_dec_dist = solar_system::calculate_moon_RA_Dec_Dist(JD);
  view->set_text_1( sexagesimal::Sexagesimal(ra_dec_dist.X).to_string() );
  view->set_text_2( sexagesimal::Sexagesimal(ra_dec_dist.Y).to_string() );

  sexagesimal::Sexagesimal longitude = get_backup_domain_longitude();
  sexagesimal::Sexagesimal latitude = get_backup_domain_latitude();

  CAA2DCoordinate RA_Dec_topocentric;

// No Good.
#if 0
  RA_Dec_topocentric = Equatorial2TopocentricRigorous_PJ(ra_dec_dist.X,
							   ra_dec_dist.Y,
							   ra_dec_dist.Z/solar_system::AU_kilometers,
							   longitude.to_double(),
							   latitude.to_double(),
							   0.0,
							   JD);


RA_Dec_topocentric = CAAParallax::Equatorial2Topocentric(ra_dec_dist.X,
							ra_dec_dist.Y,
							   (ra_dec_dist.Z/solar_system::AU_kilometers),
							longitude.to_double(),
							latitude.to_double(),
							0.0,
							JD);


#endif

  RA_Dec_topocentric = Equatorial2TopocentricRigorousAlternative(ra_dec_dist.X,
								 ra_dec_dist.Y,
								 ra_dec_dist.Z/solar_system::AU_kilometers,
								 longitude.to_double(),
								 latitude.to_double(),
								 0.0,
								 JD);

  view->set_text_3( sexagesimal::Sexagesimal(RA_Dec_topocentric.X).to_string() );
  view->set_text_4( sexagesimal::Sexagesimal(RA_Dec_topocentric.Y).to_string() );


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

/* Hackish forward declaration. @*/


void point_to_where_already_pointed_action();

void debug_action(){
//  information_view_test();
  //moon_parallax_test();
  point_to_where_already_pointed_action();
}

/* 
 * After locating an object it is likely not precisely centered in the eyepiece due to errors.
 * If so, after centering the object, call this function for prompts to keep it centered.
*/
void point_to_where_already_pointed_action(){
  Simple_Altazimuth_Scope* scope = get_main_simple_telescope();
  // CAA2DCoordinate  Azi_and_Alt = scope->current_topocentric_Azi_and_Alt();
  CAA2DCoordinate RA_and_Dec = scope->current_RA_and_Dec();
  std::unique_ptr< Pushto_Output_View > view = 
    std::unique_ptr<Pushto_Output_View >( new Pushto_Output_View( RA_and_Dec ) );
  view->set_label_2( "(To Hold Position)" );
  auto lcd = check_out_main_lcd();
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
  int index = ngc_list_access::get_index( ngc_num );
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
  RA = ngc_list_access::get_RA_i( index );
  Dec = ngc_list_access::get_Dec_i( index );
  RA_Dec.X = RA.to_double();
  RA_Dec.Y = Dec.to_double();
  /* For stars we would have a proper motion correction here. 
   * Possibly some open clusers in the NGC have a non-negligible proper motion. 
   * Check that out.
   */
  double JD = JD_timestamp_pretty_good_000();
  double deltaT = CAADynamicalTime::DeltaT( JD );
  RA_Dec = apply_aberration( RA_Dec, JD + deltaT/86400.0 );
  /* This precess if from J2000. */
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

/*
 *   Used to point the telescope to a star from the bright star catalog.
 *   The relevant line in flex_lexer.l is: 
 *   B{STAR}C{DIGIT}{1,4}         bsc_point_to_action( yytext, yyleng );
 *   
 *   char* yytext will have first three characters B*C which bear no
 *   information.  The next characters should be digits indicating
 *   the Bright Star Catalog number being sought.
 *
 *   
 */
void bsc_point_to_action(char *yytext, int yyleng)
{
  int bsc_num;
  int n = sscanf( yytext+3, "%d", &bsc_num);
  if( n != 1 ){
    return;
  }
  CAA2DCoordinate RA_Dec;
  int index = starlist_access::get_index( bsc_num );
  if( index < 0 ){
    /* Should write a general purpose error display to handle stuff like this.  */
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
  RA_Dec = starlist_access::proper_motion_adjusted_position( index, JD);
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

/* The (flex provided) arguments are ignored. */
void version_info_view_action( char* yytext, int yyleng ){
  auto lcd = check_out_main_lcd();
  auto view = std::unique_ptr < Information_View > (new Information_View());
  view->set_text_1( "Build Information" );
  std::string date_string;
  std::string addon;
  date_string = sexagesimal::to_string_hack( static_cast<uint32_t>(build_year()) );
  date_string += "-";
  addon = sexagesimal::to_string_hack( static_cast<uint32_t>(build_month()) );
  date_string += addon;
  date_string += "-";
  addon = sexagesimal::to_string_hack( static_cast<uint32_t>(build_day()) );
  date_string += addon;
  date_string += " ";
  addon = sexagesimal::to_string_hack( static_cast<uint32_t>(build_hour()) );
  while( addon.size() < 2 ){
    addon = " " + addon; // space pad
  }
  date_string += addon;
  date_string += ":";
  addon = sexagesimal::to_string_hack( static_cast<uint32_t>(build_minute()) );
  while( addon.size() < 2 ){
    addon = "0" + addon;  // zero pad
  }
  date_string += addon;
  date_string += ":";
  addon = sexagesimal::to_string_hack( static_cast<uint32_t>(build_second()) );
  while( addon.size() < 2 ){
    addon = "0" + addon;  // zero pad
  }
  date_string += addon;

  view->set_text_2( date_string );
  std::string compiler_string ; 
#if __GNUC__
  compiler_string = "GCC "; 
  compiler_string += sexagesimal::to_string_hack( static_cast<uint32_t>(__GNUC__) );
  compiler_string += '.';
  compiler_string += sexagesimal::to_string_hack( static_cast<uint32_t>(__GNUC_MINOR__) );
  compiler_string += '.';
  compiler_string += sexagesimal::to_string_hack( static_cast<uint32_t>(__GNUC_PATCHLEVEL__) );
  // + __GNUC__ + '.'  + __GNUC_MINOR__ + '.' + __GNUC_PATCHLEVEL__ ;
#else
  compiler_string = "Unknown Compiler";
#endif
  view->set_text_3( compiler_string );
  //   view->set_text_4( "__cplusplus = " + __cplusplus );
  std::string cpp_string = "__cplusplus = ";
  cpp_string += sexagesimal::to_string_hack( static_cast<uint32_t>( __cplusplus) );
  view->set_text_4( cpp_string );

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


void bsc_details_view_action( char *yytext, int yyleng ){
  uint32_t bsc_num;
  int n = sscanf( yytext+4, "%lu", &bsc_num);
  if( n != 1 ){
    return;
  }
  CAA2DCoordinate RA_Dec;
  int index = starlist_access::get_index( bsc_num );
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
  RA_Dec = starlist_access::proper_motion_adjusted_position( index, JD);
  double deltaT = CAADynamicalTime::DeltaT( JD );
  RA_Dec = apply_aberration( RA_Dec, JD + deltaT/86400.0 );
  RA_Dec = precession_and_nutation_correct_from_mean_eqinox( RA_Dec, JD );

  auto lcd = check_out_main_lcd();
  lcd->clear();
  lcd->noDisplay();
  MicroSecondDelay::millisecond_delay(1);
  lcd->display();
  auto view = std::unique_ptr < BSC_Details_View > (new BSC_Details_View());
  view->set_index( index );
  while( !view->is_finished() ){
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
  bool success = false;
  CAA2DCoordinate RA_Dec = calulate_RA_and_Dec(body_name, JD, success);
  if( !success ){
    return;
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

    JD = JD_timestamp_pretty_good_000();
    RA_Dec = calulate_RA_and_Dec(body_name, JD, success);
    if( success ){
      view->set_ra_and_dec( RA_Dec );
    }
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
  int index = starlist_access::get_index( bsc_num );
  if( index == -1 ){
    return;
  }
  double JD = JD_timestamp_pretty_good_000();
  CAA2DCoordinate RA_Dec = starlist_access::proper_motion_adjusted_position( index, JD);

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

void epoch_B1900_point_to(){
  epoch_point_to( solar_system::B1900_0);
}

/* CC1950 menu item */
void epoch_B1950_point_to(){
  epoch_point_to( solar_system::B1950_0);
}

/* CC2000 menu item */
void epoch_J2000_point_to(){
  epoch_point_to( solar_system::J2000_0);
}

std::string epoch_string( double JD ){
  CAADate date{JD,true};
  int32_t year = date.Year();
  std::string str = sexagesimal::to_string_hack( year );
  /* @TODO append one decimal year. */
  return str;
}


void epoch_point_to( double JD_epoch ){
  auto lcd = check_out_main_lcd();
  lcd->clear();

  sexagesimal::Sexagesimal RA;
  {
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
    RA = RA_view->get_value();
  } /* ~Sexagesimal_Input_View() should be invoked here because the unique_ptr goes out of scope. */

  sexagesimal::Sexagesimal declination;
  {
    std::unique_ptr<Sexagesimal_Input_View> Dec_view =
      std::unique_ptr<Sexagesimal_Input_View>(new Sexagesimal_Input_View{ "Declination DD:MM:SS" });
    Dec_view->set_plus_char( 'N' );
    Dec_view->set_minus_char( 'S' );
    Dec_view->set_msd_digits( 2 );
    while( !Dec_view->is_finished() ){
      lcd->setCursor(0,2);
      lcd = Dec_view->write_first_line(std::move(lcd));
      lcd->setCursor(0,3);
      lcd = Dec_view->write_second_line(std::move(lcd));
    }
    declination = Dec_view->get_value();
  } /* Again the input view is destructed. */
  CAA2DCoordinate RA_Dec;
  RA_Dec.X = RA.to_double();
  RA_Dec.Y = declination.to_double();


  MicroSecondDelay::millisecond_delay(700);

  lcd->clear();
  for( int i=0; i<10; ++i ){
    lcd->setCursor( 0,0 );
    lcd->print( RA.to_string() );
    lcd->setCursor( 0,1 );
    lcd->print( declination.to_string() );
  }
  double JD_now = JD_timestamp_pretty_good_000();
  RA_Dec = precession_and_nutation_correct_from_mean_eqinox(RA_Dec, JD_epoch, JD_now);

  lcd->setCursor( 0,2 );
  lcd->print( sexagesimal::Sexagesimal( RA_Dec.X ).to_string() );
  lcd->setCursor( 0,3 );
  lcd->print( sexagesimal::Sexagesimal( RA_Dec.Y ).to_string() );

  MicroSecondDelay::millisecond_delay(7000);

  std::unique_ptr< Pushto_Output_View > view = 
    std::unique_ptr<Pushto_Output_View >( new Pushto_Output_View( RA_Dec ) );
  view->set_label_2( "Epoch " + epoch_string( JD_epoch )  );

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



/* CC2490 */
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


/* 
 * BCC  (For Burnham's Guide.)
 * 
 * Burnham gives the coordinates in this format:
 * 22115s2119 = RA 22h 11.5m  Dec -21(deg) 19(min).
 * 06078n4844 = RA  6h 07.8m  Dec +48(deg) 44(min).
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





/*  Used? */
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
  /*******************************
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
  **********************************/
  std::unique_ptr< Planetary_Details_View > view = 
    std::unique_ptr<Planetary_Details_View >( new Planetary_Details_View( body_num ) );
  auto lcd = check_out_main_lcd();
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


void ngc_details_view_action( char* yytext, int yyleng ){
  int ngc_num;
  int n;
  n = sscanf(yytext + 3, "%d", &ngc_num);
  if (n != 1) {
    return;
  }
  std::unique_ptr< NGC_Details_View > view = 
    std::unique_ptr< NGC_Details_View >( new NGC_Details_View( ngc_num ) );
  auto lcd = check_out_main_lcd();
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
