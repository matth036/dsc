/********************************************************************

Command interpreter for keypad

 1 2 3 A
 4 5 6 B
 7 8 9 C
 * 0 # D

Where 
 # behaves as enter (which initiates the request for the command.)
 D behaves as backspace.
(So, # and D will not appear in commands)

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

CA{DIGIT}{1,4}               ngc_point_to_action( yytext, yyleng );

B{STAR}C{DIGIT}{1,4}         bsc_point_to_action( yytext, yyleng );

B{DIGIT}{1}                  solar_system_point_to_action(  yytext, yyleng );

A{STAR}{DIGIT}{1,2}          almanac_star_point_to_action( yytext, yyleng );

AAA2490                      RA_and_Declination_dialog();
AAA1950                      Burnham_Handbook_Point_To();

AAA555                       display_current_time_view();
AAA556                       add_seconds_dialog();
AAA554                       subtract_seconds_dialog();

AAA58                        navigation_star_menu_test();
AAA360                       sexagesimal_test_dialog();

AAA720                       long_lat_dialog();
AAA0317                      countdown_to_Saint_Patricks_Day();

AAA234                       alignment_sight_test();
AAA456                       make_optimized_solution_test();

AAA789                       pre_fab_example_test();
AAA987                       clear_sight_data();

.*                           return 0;
%%

/*
 * main() is in main.cpp
 *
 * yylex() is called in the function 
 * void scan_for_action( std::string ) 
 * implemented in command_action.cpp 
 */
