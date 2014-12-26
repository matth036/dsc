#include "navigation_star.h"

int
 navigation_star::is_navigation_star(int bscnum)
{
  int i;
  for (i = 0; i <= NAVIGATION_LIST_MAX; ++i) {
    if (bscnum == navigation_star::nav2bsc[i]) {
      return 1;
    }
  }
  return 0;
}

int navigation_star::get_navigation_star_num(int bscnum)
{
  int i;
  for (i = 0; i <= NAVIGATION_LIST_MAX; ++i) {
    if (bscnum == nav2bsc[i]) {
      return i;
    }
  }
  return -1;
}

std::string navigation_star::get_navigation_star_name(uint32_t nav_num)
{
  switch( nav_num ){
  case 0:
    return "Polaris";
  case 1: 
    return "Alpheratz";
  case 2:
    return "Ankaa";
  case 3:
    return "Schedar";
  case 4:
    return "Diphda";
  case 5:
    return "Achernar";
  case 6:
    return "Hamal";
  case 7:
    return "Acamar";
  case 8:
    return "Menkar";
  case 9:
    return "Mirfak";
  case 10:
    return "Aldebaran";
  case 11:
    return "Rigel";
  case 12:
    return "Capella";
  case 13:
    return "Bellatrix";
  case 14:
    return "Elnath";
  case 15:
    return "Alnilam";
  case 16:
    return "Betelgeuse";
  case 17:
    return "Canopus";
  case 18:
    return "Sirius";
  case 19:
    return "Adhara";
  case 20: 
    return "Procyon";
  case 21:
    return "Pollux";
  case 22:
    return "Avior";
  case 23:
    return "Suhail";
  case 24:
    return "Miaplacidus";
  case 25:
    return "Alphard";
  case 26:
    return "Regulus";
  case 27:
    return "Dubhe";
  case 28:
    return "Denebola";
  case 29:
    return "Gienah";  
  case 30:
    return "Acrux";
  case 31: 
    return "Gacrux";
  case 32:
    return "Alioth";
  case 33:
    return "Spica";
  case 34:
    return "Alkaid";
  case 35:
    return "Hadar";
  case 36:
    return "Menkent";
  case 37:
    return "Arcturus";
  case 38:
    return "Rigil Kentaurus";
  case 39:
    return "Zubenelgenubi";
  case 40:
    return "Kochab";
  case 41:
    return "Alphecca";
  case 42:
    return "Antares";
  case 43:
    return "Atria";
  case 44:
    return "Sabik";
  case 45:
    return "Shaula";
  case 46:
    return "Rasalhague";
  case 47:
    return "Eltanin";
  case 48:
    return "Kaus Australis";
  case 49:
    return "Vega";
  case 50:
    return "Nunki";
  case 51:
    return "Altair";
  case 52:
    return "Peacock";
  case 53:
    return "Deneb";
  case 54: 
    return "Enif"; 
  case 55:
    return "Al Na\'ir";
  case 56:
    return "Fomalhaut"; 
  case 57:
    return "Markab";	
  default:
    return "Fall Through Error";
  }
}


std::string get_dso_description( int catalog_num){
  switch( catalog_num ){
  case 0:
    return "Polaris";

  default:
    return "Fall Through Error";

  }

}
