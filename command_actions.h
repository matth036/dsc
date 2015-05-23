#ifndef _COMMAND_ACTIONS_H
#define _COMMAND_ACTIONS_H

#include <string>

void version_info_view_action( char* yytext, int yyleng );
void ngc_point_to_action( char* yytext, int yyleng );
void bsc_point_to_action( char* yytext, int yyleng );
void bsc_details_view_action( char* yytext, int yyleng );
void solar_system_point_to_action( char* yytext, int yyleng );
void almanac_star_point_to_action( char* yytext, int yyleng );
void planetary_details_view_action( char* yytext, int yyleng );
void ngc_details_view_action( char* yytext, int yyleng );

void epoch_point_to( double JD_epoch );
void RA_and_Declination_dialog();
void epoch_J2000_point_to();
void epoch_B1950_point_to();
void epoch_B1900_point_to();
void Burnham_Handbook_Point_To();
void point_to_where_already_pointed_action();
void messier_catalog_point_to( char* yytext, int yyleng );



void angular_coordinate_view();


void navigation_star_menu_test();
void sexagesimal_test_dialog();
void float_input_test();
float prompt_for_float( std::string prompt_text, char first_digit );

void display_current_time_view();
void add_seconds_dialog();
void subtract_seconds_dialog();
void long_lat_dialog();
void countdown_to_Saint_Patricks_Day();

void proximate_stars_view();
void proximate_ngc_object_view();
void proximate_navigation_stars_view();
void proximate_messier_objects_view();

void debug_action();


/* Used ? */
void default_action( char* yytext, int yyleng );



#endif  /*  _COMMAND_ACTIONS_H  */
