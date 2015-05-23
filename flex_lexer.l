/********************************************************************

Command interpreter for keypad

 1 2 3 A
 4 5 6 B
 7 8 9 C
 * 0 # D

Where 
 # behaves as enter (which initiates the request for the command.)
 D behaves as backspace.
(So, # and D will not appear in commands.)

********************************************************************/

%{
#include "command_actions.h"
#include "sight_data_command_actions.h"
#include <string>
%}

%option ecs
%option meta-ecs
%option noyywrap
%option never-interactive
%pointer

DIGIT		[0-9]
ALPHA		[ABC]
SPECIAL         [#D]
STAR            [\*]
NUMBER          {DIGIT}+
WORD            {ALPAH}+

%%
ABAB                         version_info_view_action( yytext, yyleng );

CA{DIGIT}{1,4}               ngc_point_to_action( yytext, yyleng );
ACA{DIGIT}{1,4}              ngc_details_view_action( yytext, yyleng );
CA                           proximate_ngc_object_view();

B{STAR}C{DIGIT}{1,4}         bsc_point_to_action( yytext, yyleng );
AB{STAR}C{DIGIT}{1,4}        bsc_details_view_action( yytext, yyleng );
B{STAR}C                     proximate_stars_view();

B{DIGIT}{1}                  solar_system_point_to_action(  yytext, yyleng );
AB{DIGIT}{1}                 planetary_details_view_action( yytext, yyleng );

A{STAR}{DIGIT}{1,2}          almanac_star_point_to_action( yytext, yyleng );
A{STAR}                      proximate_navigation_stars_view();

CC2490                       RA_and_Declination_dialog();  /* Celestial Coordinates point to. 0-24 Hours right ascension, (+-)0-90 Degrees declination. */
BCC                          Burnham_Handbook_Point_To();  /* Burnham Celestial Coordinates point to.  */
CC2000                       epoch_J2000_point_to();
CC1950                       epoch_B1950_point_to();
CC1900                       epoch_B1900_point_to();

AAC{DIGIT}{1,3}              messier_catalog_point_to( yytext, yyleng ); /* Point to specified Messier Catalog object. (Two A's kinda look like M)  */
AAC                          proximate_messier_objects_view();

BAC00                        point_to_where_already_pointed_action();    /* Maintian celestial position. */

AAA555                       display_current_time_view();
AAA556                       add_seconds_dialog();
AAA554                       subtract_seconds_dialog();

AAA090                       angular_coordinate_view();

AAA58                        navigation_star_menu_test();
AAA360                       sexagesimal_test_dialog();

AAA720                       long_lat_dialog();
AAA0317                      countdown_to_Saint_Patricks_Day();

AAA234                       alignment_sight_test();
AAA456                       make_optimized_solution_test();
BBB456                       shift_180_then_optimize_solution();

AAA789                       pre_fab_example_test();
AAA987                       clear_sight_data();

AAA8686                      debug_action();



.*                           return 0;
%%

/*
 * main() is in main.cpp
 *
 * yylex() is called in the function 
 * void scan_for_action( std::string ) 
 * implemented in command_action.cpp 
 */
