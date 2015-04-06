#ifndef _COMMAND_ACTIONS_H
#define _COMMAND_ACTIONS_H


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

void angular_coordinate_view();


void navigation_star_menu_test();
void sexagesimal_test_dialog();

void display_current_time_view();
void add_seconds_dialog();
void subtract_seconds_dialog();
void long_lat_dialog();
void countdown_to_Saint_Patricks_Day();

void debug_action();


/* Used ? */
void default_action( char* yytext, int yyleng );



#endif  /*  _COMMAND_ACTIONS_H  */
