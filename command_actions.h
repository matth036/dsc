#ifndef _COMMAND_ACTIONS_H
#define _COMMAND_ACTIONS_H


void ngc_point_to_action( char* yytext, int yyleng );
void bsc_point_to_action( char* yytext, int yyleng );
void bsc_details_view_action( char* yytext, int yyleng );

void bsc_point_to_action_dangerously(char *yytext, int yyleng);

void solar_system_point_to_action( char* yytext, int yyleng );
void almanac_star_point_to_action( char* yytext, int yyleng );
void planetary_details_view_action( char* yytext, int yyleng );
void ngc_details_view_action( char* yytext, int yyleng );

void RA_and_Declination_dialog();
void Burnham_Handbook_Point_To();
void angular_coordinate_view();


void default_action( char* yytext, int yyleng );

void navigation_star_menu_test();
void sexagesimal_test_dialog();

void display_current_time_view();
void add_seconds_dialog();
void subtract_seconds_dialog();
void long_lat_dialog();
void countdown_to_Saint_Patricks_Day();

void debug_action();





#endif  /*  _COMMAND_ACTIONS_H  */
