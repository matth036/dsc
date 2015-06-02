#include "messier.h"
// #include "ngc_list_access.h"
#include "sexagesimal.h"
#include "extra_solar_transforms.h"

/* A helper function not declared in header messier.h  */
bool is_messier_with_no_ngc(int messier_number)
{
  if (messier_number < 1)
    return false;
  if (messier_number > 110)
    return false;
  if (messier_numbers::messier_to_ngc(messier_number) == -1) {
    return true;
  }
  return false;
}

/* A helper function not declared in header messier.h  */
CAA2DCoordinate messier_with_no_ngc_J2000_RA_and_Dec(int number)
{
  CAA2DCoordinate result;
  sexagesimal::Sexagesimal RA;
  sexagesimal::Sexagesimal Dec;
  switch (number) {
  case 25:                     /* M25, IC 4725   18h 31m 47s  -19:07:00 J2000   */
    RA = sexagesimal::Sexagesimal(18, 31, 47, 0);
    Dec = sexagesimal::Sexagesimal(-19, 7, 0, 0);
    result.X = RA.to_double();
    result.Y = Dec.to_double();
    return result;
  case 40:                     /* M40, Winnecke 4,  Double star in Ursa Major.   12h22m13s 58:04:59  */
    RA = sexagesimal::Sexagesimal(12, 23, 13, 0);
    Dec = sexagesimal::Sexagesimal(58, 4, 59, 0);
    result.X = RA.to_double();
    result.Y = Dec.to_double();
    return result;
    return result;
  case 45:                     /* Once thought to be missing.  */
  case 102:                    /* Once thought to be missing.  */
  default:
    return CAA2DCoordinate {
    };
  }
  return CAA2DCoordinate {
  };
}

/* Intended for pointing to Messier objects. */
CAA2DCoordinate messier_numbers::messier_J2000_RA_and_Dec(int number, bool & OK)
{
  if (is_messier_with_no_ngc(number)) {
    /*
     * Recording success only because  
     * messier_with_no_ngc_J2000_RA_and_Dec( number ); 
     * has been tested.  Test again if re-written.
     */
    OK = true;
    return messier_with_no_ngc_J2000_RA_and_Dec(number);
  }
  CAA2DCoordinate result;
  int ngc_number = messier_to_ngc(number);
  if (ngc_number == -1) {
    OK = false;
    return result;              /* returns {0.0,0.0} */
  }
  //  int index = ngc_list_access::get_index(ngc_number);
  int index = -1;
  while( index == -1 ){
    index = extra_solar_ngc::neo_get_index_fast(ngc_number);
  }
  result.X = extra_solar_ngc::get_RA_i(index).to_double();
  result.Y = extra_solar_ngc::get_Dec_i(index).to_double();
  OK = true;
  return result;
}

/* Big old switch statement.  Wouldn't hurt to proof read. */
int messier_numbers::messier_to_ngc(int messier_number)
{
  switch (messier_number) {
  case 1:
    return 1952;
  case 2:
    return 7089;
  case 3:
    return 5272;
  case 4:
    return 6121;
  case 5:
    return 5904;
  case 6:
    return 6405;
  case 7:
    return 6475;
  case 8:
    return 6523;
  case 9:
    return 6333;
  case 10:
    return 6254;
  case 11:
    return 6705;
  case 12:
    return 6218;
  case 13:
    return 6205;
  case 14:
    return 6402;
  case 15:
    return 7078;
  case 16:
    return 6611;
  case 17:
    return 6618;
  case 18:
    return 6613;
  case 19:
    return 6273;
  case 20:
    return 6514;
  case 21:
    return 6531;
  case 22:
    return 6656;
  case 23:
    return 6494;
  case 24:
    return 6603;                /* NGC 6603 and surrounding Milky Way star cloud */
  case 25:
    return -1;                  /* M25, IC 4725   18h 31m 47s  -19:07:00 J2000  4.6m 32.0' radius  */
  case 26:
    return 6694;
  case 27:
    return 6853;
  case 28:
    return 6626;
  case 29:
    return 6913;
  case 30:
    return 7099;
  case 31:
    return 224;
  case 32:
    return 221;
  case 33:
    return 598;
  case 34:
    return 1039;
  case 35:
    return 2168;
  case 36:
    return 1960;
  case 37:
    return 2099;
  case 38:
    return 1912;
  case 39:
    return 7092;
  case 40:
    return -1;                  /* M40, Winnecke 4,  Double star in Ursa Major.   12h22m13s 58:04:59  9.65m and 10.1m */
  case 41:
    return 2287;
  case 42:
    return 1976;
  case 43:
    return 1982;
  case 44:
    return 2632;
  case 45:
    return 1435;                /* Pleadies.  Number is the Merope Nebula. */
  case 46:
    return 2437;
  case 47:
    return 2422;
  case 48:
    return 2548;
  case 49:
    return 4472;
  case 50:
    return 2322;
  case 51:
    return 5194;
  case 52:
    return 7654;
  case 53:
    return 5024;
  case 54:
    return 6715;
  case 55:
    return 6809;
  case 56:
    return 6779;
  case 57:
    return 6720;
  case 58:
    return 4579;
  case 59:
    return 4621;
  case 60:
    return 4649;
  case 61:
    return 4303;
  case 62:
    return 6266;
  case 63:
    return 5055;
  case 64:
    return 4826;
  case 65:
    return 3623;
  case 66:
    return 3627;
  case 67:
    return 2682;
  case 68:
    return 4590;
  case 69:
    return 6637;
  case 70:
    return 6681;
  case 71:
    return 6838;
  case 72:
    return 6981;
  case 73:
    return 6994;
  case 74:
    return 628;
  case 75:
    return 6864;
  case 76:
    return 650;
  case 77:
    return 1068;
  case 78:
    return 2068;
  case 79:
    return 1904;
  case 80:
    return 6093;
  case 81:
    return 3031;
  case 82:
    return 3034;
  case 83:
    return 5236;
  case 84:
    return 4374;
  case 85:
    return 4382;
  case 86:
    return 4406;
  case 87:
    return 4487;
  case 88:
    return 4501;
  case 89:
    return 4552;
  case 90:
    return 4569;
  case 91:
    return 4548;                /* Thought to be an error in Messier Catalog. */
  case 92:
    return 6341;
  case 93:
    return 2447;
  case 94:
    return 4736;
  case 95:
    return 3351;
  case 96:
    return 3368;
  case 97:
    return 3587;
  case 98:
    return 4192;
  case 99:
    return 4254;
  case 100:
    return 4321;
  case 101:
    return 5457;
  case 102:
    return 5866;                /* This or a duplicate observation of M101? */
  case 103:
    return 561;
  case 104:
    return 4594;
  case 105:
    return 3379;
  case 106:
    return 4258;
  case 107:
    return 6171;
  case 108:
    return 3556;
  case 109:
    return 3992;
  case 110:
    return 205;
  default:
    return -1;
  }
}
