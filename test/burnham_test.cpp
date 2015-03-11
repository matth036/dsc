#include <iostream>
#include <cmath>
#include <string.h>
#include "AA2DCoordinate.h"
#include "AACoordinateTransformation.h"
#include "AADate.h"
#include "extra_solar_transforms.h"
#include "AANutation.h"
#include "AADynamicalTime.h"
#include "sexagesimal.h"
#include "navigation_star.h"

#include "solar_system.h"
#include "AASidereal.h"
#include "input_views.h"

using std::cout;
using std::endl;

void pretty_print(CAA2DCoordinate RA_Dec)
{
  cout << sexagesimal::double2sexagesimal(RA_Dec.X).
      to_dddmmxxxx_string() << "    ";
  cout << sexagesimal::double2sexagesimal(RA_Dec.Y).
      to_dddmmxxxx_string() << "   ";
  cout << "RA and Dec";
  cout << endl;
}

void dms_pretty_print(CAA2DCoordinate RA_Dec)
{
  cout << sexagesimal::double2sexagesimal(RA_Dec.X).to_dms_string() << "  ";
  cout << sexagesimal::double2sexagesimal(RA_Dec.Y).to_dms_string() << "   ";
  cout << "RA and Dec";
  cout << endl;
}

void na_pretty_print(CAA2DCoordinate ra_dec)
{
  cout << sexagesimal::double2sexagesimal(ra_dec.X).
      to_dddmmxxxx_string() << "    ";
  cout << sexagesimal::double2sexagesimal(ra_dec.Y).
      to_dddmmxxxx_string() << "   ";
  cout << "RA  and Dec";
  cout << endl;
  double siderial_hour_angle =
      CAACoordinateTransformation::MapTo0To360Range(360 - ra_dec.X * 15);
  cout << sexagesimal::double2sexagesimal(siderial_hour_angle).
      to_dddmmxxxx_string() << "    ";
  cout << sexagesimal::double2sexagesimal(ra_dec.Y).
      to_dddmmxxxx_string() << "   ";
  cout << "SHA and Dec";
  cout << endl;
}

int main(int argc, char **argv)
{
  cout << "Default test coordinates are those of star Beta Orion (Rigel)." <<
      endl;
  cout << "Burnham gives coordinats as 05121s0815" << endl;
  printf("%s Year Month date.xxx [HHMMXsDDMM] \n", argv[0]);
  //  cout << ">./nautical_almanac Year Month date.xxx" << endl;
  cout << endl;
  cout << "gcc = " << __GNUC__ << "." << __GNUC_MINOR__ << "." <<
      __GNUC_PATCHLEVEL__ << endl;
  cout << "__cplusplus = " << __cplusplus << endl;
  cout << endl;
  /* A default example date. */
  int year = 2010;
  int month = 7;
  double day = 1;
  if (argc >= 4) {
    sscanf(argv[1], "%d", &year);
    sscanf(argv[2], "%d", &month);
    sscanf(argv[3], "%lf", &day);
  }
  char char_buffer[11] = "05121s0815";
  if (argc >= 5) {
    strncpy(char_buffer, argv[4], 11);
  }
  char_buffer[10] = '\0';

  printf( "year = %d       month = %d    day = %lf\n", year, month, day );
  bool gregorian;
  CAADate test_date {
  year, month, day, gregorian = true};
  double JD = test_date.Julian();
  printf("Julian date %2.8f\n", JD);
  printf("Character Buffer = %s\n", char_buffer);
  CAADate JAN_0(year, 1, 1, gregorian = true);
  double Epoch = year + (JD - JAN_0.Julian()) / 365.25;

  double RA = 0;
  unsigned int scan_value;
  int n;
  scan_value = 0;
  n = sscanf(char_buffer, "%2u", &scan_value);
  if (n == 1) {
    RA += static_cast < double >(scan_value);
  }
  scan_value = 0;
  n = sscanf(char_buffer + 2, "%2u", &scan_value);
  if (n == 1) {
    RA += (1.0 / 60.0) * static_cast < double >(scan_value);
  }
  scan_value = 0;
  n = sscanf(char_buffer + 4, "%1u", &scan_value);
  if (n == 1) {
    RA += (1.0 / 600.0) * static_cast < double >(scan_value);
  }

  char minus_char = 's';
  double dec = 0;
  //  unsigned int scan_value;
  //  int n;
  scan_value = 0;
  n = sscanf(char_buffer + 6, "%2u", &scan_value);
  if (n == 1) {
    dec += static_cast < double >(scan_value);
  }
  scan_value = 0;
  n = sscanf(char_buffer + 8, "%2u", &scan_value);
  if (n == 1) {
    dec += (1.0 / 60.0) * static_cast < double >(scan_value);
  }
  if (char_buffer[5] == minus_char) {
    dec = -dec;
  } else {
    // do nothing
  }

  cout << RA << "    " << dec << endl;

  CAA2DCoordinate RA_Dec;
  RA_Dec.X = RA;
  RA_Dec.Y = dec;
  cout << "Epoch 1950 coordinates:" << endl;
  pretty_print(RA_Dec);
  RA_Dec = burnham_correct( RA_Dec, JD );
  cout << "Epoch " << Epoch << " coordinates:" << endl;
  pretty_print(RA_Dec);
  na_pretty_print(RA_Dec);
  dms_pretty_print(RA_Dec);

  cout << "Do Over" << endl;
  RA_Dec.X = RA;
  RA_Dec.Y = dec;
  cout << "Epoch 1950 coordinates:" << endl;
  pretty_print(RA_Dec);
  RA_Dec = precession_correct( RA_Dec, solar_system::B1950_0, solar_system::J2000_0 );
  cout << "Epoch 2000 coordinates:" << endl;
  pretty_print(RA_Dec);
  dms_pretty_print(RA_Dec);
  double deltaT = CAADynamicalTime::DeltaT( JD );
  RA_Dec = apply_aberration( RA_Dec, JD + deltaT/86400.0 );
  cout << "Epoch 2000 coordinates with aberration applied:" << endl;
  pretty_print(RA_Dec);
  dms_pretty_print(RA_Dec);
  RA_Dec = precession_and_nutation_correct_from_mean_eqinox( RA_Dec, solar_system::J2000_0, JD );
  cout << "Epoch " << Epoch << " coordinates:" << endl;
  pretty_print(RA_Dec);
  na_pretty_print(RA_Dec);
  dms_pretty_print(RA_Dec);
}
