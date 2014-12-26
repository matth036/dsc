#include <stdio.h>
#include "sexagesimal.h"
#include "navigation_star.h"
#include "star_data.h"

#include <iostream>

/*  Edit this to produce a star list of acceptable size. */
int
doUseThisLine (char *catalog_line)
{
  uint32_t RAhh;
  uint32_t RAmm;
  uint32_t RAss;
  uint32_t RAx;

  uint32_t BSCnum;
  int32_t DECdd;
  uint32_t DECmm;
  uint32_t DECss;

  float pmRA;
  float pmDE;
  float Vmag;

  sscanf (catalog_line + 1, "%4u", &BSCnum);
  sscanf (catalog_line + 103, "%5f", &Vmag);
  sscanf (catalog_line + 76, "%2u", &RAhh);
  sscanf (catalog_line + 78, "%2u", &RAmm);
  sscanf (catalog_line + 80, "%2u", &RAss);
  sscanf (catalog_line + 83, "%1u", &RAx);
  sscanf (catalog_line + 84, "%3d", &DECdd);
  sscanf (catalog_line + 87, "%2u", &DECmm);
  sscanf (catalog_line + 89, "%2u", &DECss);
  sscanf (catalog_line + 149, "%6f", &pmRA);
  sscanf (catalog_line + 155, "%6f", &pmDE);
  if( RAhh==0 && RAmm==0 && RAss==0  && DECmm==0 && DECss==0 ){
    return 0;
  }

  if (navigation_star::is_navigation_star (BSCnum))
    {    /* Use this conditional if all navigational are to be included. */
      return 1;
    }
  if (DECdd < -45 )
    {   /* Optionally use this conditional to omit stars too far south.  */
      // return 0;			
    }
  /*
    with Vmag < 3.5 
    STARLIST_SIZE = 297;

    with Vmag < 4.01
    STARLIST_SIZE = 530;

    with Vmag < 5.5
    STARLIST_SIZE = 2859;
   */
  if (  Vmag <= 5.5  )
    {	/*  Use this conditional to include only brighter stars. */
      return 1;
    }
  /*
  with no restriction on Vmag
  STARLIST_SIZE = 9108
  */
  //return 1;
  return 0;
}




int main ()
{
  printf ("#ifndef STARTLIST_H\n");
  printf ("#define STARTLIST_H\n");
  printf ("#include \"star_data.h\"  ");
  printf ( "\n\n" );
  printf ("/*   ");
  printf( " THIS IS A MACHINE GENERATED FILE " );
  printf (" */ ");
  printf ( "\n\n\n" );

  printf( "#ifdef __cplusplus\n" );
  printf( "extern \"C\" {\n" );
  printf( "#endif\n" );

  star_data current_star;
  sexagesimal::Sexagesimal current_RA;
  sexagesimal::Sexagesimal current_DEC;

  uint32_t RAhh;
  uint32_t RAmm;
  uint32_t RAss;
  uint32_t RAx;

  uint32_t BSCnum;
  int32_t DECdd;
  uint32_t DECmm;
  uint32_t DECss;

  float pmRA;
  float pmDE;
  float Vmag;
  FILE *bst_file = fopen ("catalog", "r");
  char current_line[256];
  char next_line[256];
  current_line[0] = 'z';	// Start at current_line[1] to match indices in Bright Star Catalog notes.
  printf ("static const star_data starlist[] = { \n");
  uint32_t lines_read = 0;  // Not accuratly updated.  Remove.
  uint32_t count = 0;
  char* have_current_line = 0; 
  char* have_next_line = 0;
  // int index = 0;
  have_current_line = fgets (current_line + 1, 255, bst_file);
  while( !doUseThisLine( have_current_line ) ){
    have_current_line = fgets (current_line + 1, 255, bst_file);
  }
  have_next_line = fgets (next_line + 1, 255, bst_file);
  while( have_next_line && !doUseThisLine( next_line ) ){
    char* have_next_line = fgets (next_line + 1, 255, bst_file);
  }

  
    do{
      sscanf (current_line + 1, "%4u", &BSCnum);
      sscanf (current_line + 103, "%5f", &Vmag);
      sscanf (current_line + 76, "%2u", &RAhh);
      sscanf (current_line + 78, "%2u", &RAmm);
      sscanf (current_line + 80, "%2u", &RAss);
      sscanf (current_line + 83, "%1u", &RAx);
      sscanf (current_line + 84, "%3d", &DECdd);
      sscanf (current_line + 87, "%2u", &DECmm);
      sscanf (current_line + 89, "%2u", &DECss);
      sscanf (current_line + 149, "%6f", &pmRA);
      sscanf (current_line + 155, "%6f", &pmDE);

      current_DEC.setData (DECdd, DECmm, DECss, 0);
      current_RA.setData(RAhh, RAmm, RAss, RAx * 100 );

      current_star.RAdata = current_RA.get_data();
      current_star.DECdata = current_DEC.get_data();
      current_star.RApm = pmRA;
      current_star.DECpm = pmDE;
      current_star.magnitude = Vmag;
      current_star.BSCnum = BSCnum;
      std::cout << "/*  " << std::endl;
      std::cout << " *  Bright Star Catalog coordinates: " << std::endl;
      std::cout << " *  RA = " << current_RA.to_string()  <<  std::endl;
      std::cout << " * DEC = " << current_DEC.to_string() <<  std::endl;
      std::cout << " */ " << std::endl;
      // printf( "*/  \n" );
      int i;
      have_current_line = have_next_line;
      for( i=0; i<256; ++i ){
	current_line[i] = next_line[i];
      }

      have_next_line = fgets ( next_line + 1, 255, bst_file);
      while( have_next_line && !doUseThisLine( next_line ) ){
	have_next_line = fgets (next_line + 1, 255, bst_file);
      }
      if( navigation_star::is_navigation_star( BSCnum )  ){
	std::cout << "{  /* " ;
	std::cout << "index = " << count << "  BSC number = " << BSCnum; 
	int navstar_num = navigation_star::get_navigation_star_num(BSCnum);
	std::cout << "  navigation star # " << navstar_num << ", ";
	std::cout << "   " << navigation_star::get_navigation_star_name(navstar_num) ;
	std::cout << "  */ " << std::endl;
      }else{
	std::cout << "{  /* " ;
	std::cout << "index = " << count << "     BSC number = " << BSCnum; 
	std::cout << "  */ " << std::endl;
      }
      ++count;
      printf ("0x%08X,\n", current_star.RAdata ) ;
      printf ("0x%08X,\n", current_star.DECdata );
      printf ("%-+ff,\n", current_star.RApm);
      printf ("%-+ff,\n", current_star.DECpm);
      printf ("%-+ff,\n", current_star.magnitude);
      printf ("%d", current_star.BSCnum);
      if ( have_current_line )
	{
	  printf ("},\n");
	}
      else
	{
	  printf ("}\n");
	}
    }while(have_current_line);

    /* Print end of file stuff */
  printf ("};\n");
  printf ("\n");
  printf( "static int32_t constexpr STARLIST_SIZE = %d;\n", count );
  printf ("\n");

  fclose (bst_file);

  printf( "#ifdef __cplusplus\n" );
  printf( "}\n" );
  printf( "#endif\n" );

  printf ("#endif    /* eof  STARLIST_H */ \n");
  return 0;
}
