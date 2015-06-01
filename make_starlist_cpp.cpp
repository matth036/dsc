#include <stdio.h>

#include "sexagesimal.h"
#include "navigation_star.h"
#include "star_data.h"

#include <iostream>
#include <fstream>
#include <ios>
#include <memory>
#include <cctype>

using std::cout;
using std::endl;

/*****************************************************************
 * Forward declarations of funtions used in accept_bsc_number( .. )
 *****************************************************************/
uint32_t extract_bsc_number( std::string line);
bool is_bsc_blacklisted( uint32_t bsc_number );
sexagesimal::Sexagesimal extract_Dec( std::string line);
float extract_Vmag( std::string line);

/*****************************************************************
 * 
 * bool accept_bsc_number( .. )
 * Edit this function to reduce the size of the onboard catalog.
 * (To save space in flash memory.)
 *
 ****************************************************************/
bool accept_bsc_number(std::string catalog_line ){
  uint32_t bsc_number = extract_bsc_number( catalog_line );
  if( is_bsc_blacklisted( bsc_number ) ){
    return false;
  }
  if( navigation_star::is_navigation_star( bsc_number ) ){
    return true;  // Include all navigaion stars for global sight alignment.
  } 
  sexagesimal::Sexagesimal declination = extract_Dec( catalog_line );
  double declination_limit = 45.0;
  if( declination.to_double() < declination_limit ){
    // optionallly return false here to omit far south stars.
  }
  if( declination.to_double() > declination_limit ){
    // optionallly return false here to omit far north stars.
  }
  float Vmag = extract_Vmag( catalog_line );
  float Vmag_limit = 99.0;
  // float Vmag_limit = 5.1;
  if( Vmag > Vmag_limit ){
    return false;
  }
  return true;
}

void print_pod_check(){
  if( std::is_pod<star_data>() ){
    cout << "star_data is Plain Old Data." << endl;
  }else{
    cout << "star_data is NOT Plain Old Data." << endl;
  }

}

uint32_t extract_bsc_number( std::string line){
  uint32_t BSCnum;
  std::string str = line.substr(0,4);
  size_t p = 0;
  BSCnum = std::stoi(str,&p);
  while( p != 4 ){
   
  }
  return BSCnum;
}

float extract_Vmag( std::string line){
  float Vmag;
  std::string str = line.substr(102,5);
  //  sscanf (str.data(), "%5f", &Vmag);
  size_t p = 0;
  Vmag = std::stof(str,&p);
  return Vmag;
}

float extract_RApm( std::string line){
  float RApm;
  std::string str = line.substr(148,6);
  size_t p = 0;
  RApm = std::stof(str,&p);
  while( p != 6 ){
   
  }
  return RApm;
}

float extract_DECpm( std::string line){
  float DECpm;
  std::string str = line.substr(154,6);
  size_t p = 0;
  DECpm = std::stof(str,&p);
  while( p != 6 ){
   
  }
  return DECpm;
}

int32_t extract_RAhh( std::string line){
  int32_t RAhh;
  std::string str = line.substr(75,2);
  size_t p = 0;
  RAhh = std::stoi(str,&p);
  while( p != 2 ){
   
  }
  return RAhh;
}

uint8_t extract_RAmm( std::string line){
  uint8_t RAmm;
  std::string str = line.substr(77,2);
  size_t p = 0;
  RAmm = std::stoi(str,&p);
  while( p != 2 ){
   
  }
  return RAmm;
}

uint8_t extract_RAss( std::string line){
  uint8_t RAss;
  std::string str = line.substr(79,2);
  size_t p = 0;
  RAss = std::stoi(str,&p);
  while( p != 2 ){
   
  }
  return RAss;
}

float extract_RAx_float( std::string line){
  float RAx;
  std::string str = line.substr(81,2);
  size_t p = 0;
  RAx = std::stof(str,&p);
  while( p != 2 ){
   
  }
  return RAx;
}

uint32_t extract_RAx_int( std::string line){
  uint32_t RAx;
  std::string str = line.substr(82,1);
  size_t p = 0;
  RAx = std::stoi(str,&p);
  while( p != 1 ){
   
  }
  return RAx;
}

/********************/

int16_t extract_DECdd( std::string line){
  int16_t DEChh;
  std::string str = line.substr(83,3);
  size_t p = 0;
  DEChh = std::stoi(str,&p);
  while( p != 3 ){
   
  }
  return DEChh;
}

uint8_t extract_DECmm( std::string line){
  uint8_t DECmm;
  std::string str = line.substr(86,2);
  size_t p = 0;
  DECmm = std::stoi(str,&p);
  while( p != 2 ){
   
  }
  return DECmm;
}

uint8_t extract_DECss( std::string line){
  uint8_t DECss;
  std::string str = line.substr(88,2);
  size_t p = 0;
  DECss = std::stoi(str,&p);
  while( p != 2 ){
   
  }
  return DECss;
}

bool is_bsc_blacklisted( uint32_t bsc_number ){
  /* These catalog entries fail extract_Vmag( std::string line )  */
  switch( bsc_number ){
  case 92:   // Nova 1572
  case 95:   // ?
  case 182:  // M31
  case 1057: // Nova 1901
  case 1841: // Nova 1891
  case 2472: // Nova 1903
  case 2496: // NGC 2281
  case 3515: // M67 
  case 3671: // NGC 2801
  case 6309: // Nova 1848
  case 6515: // Nova 1604
  case 7189: // Nova 1899
  case 7539: // Nova 1670
  case 8296: // Nova 1876
    return true;
  }
  return false;
}


sexagesimal::Sexagesimal extract_RA( std::string line){
  sexagesimal::Sexagesimal RA{
    extract_RAhh( line ),
      extract_RAmm( line ),
      extract_RAss( line ),
      static_cast<uint16_t>(100*extract_RAx_int( line ))
      };
  return RA;
}

sexagesimal::Sexagesimal extract_Dec( std::string line){
  sexagesimal::Sexagesimal Dec{
    extract_DECdd( line ),
      extract_DECmm( line ),
      extract_DECss( line ),
      static_cast<uint16_t>( 0 )
      };
  return Dec;
}

std::vector<star_data> make_vector_catalog(){
  std::vector<star_data> vect_cat;
  std::string source_filename = "catalog";
  std::ifstream src;
  src.open( source_filename );
  std::string current_line;

  while( src ){
    getline( src, current_line );
    if( current_line.size() > 0 ){
      uint32_t bsc_number = extract_bsc_number( current_line );
      if( accept_bsc_number( current_line ) ){
	sexagesimal::Sexagesimal RA = extract_RA( current_line );
	sexagesimal::Sexagesimal Dec = extract_Dec( current_line );
	float mag = extract_Vmag( current_line );
	float ra_pm = extract_RApm( current_line );
	float dec_pm = extract_DECpm( current_line );
	vect_cat.push_back(
			   star_data{
			     RA.get_binary_data(),
			       Dec.get_binary_data(),
			       ra_pm,
			       dec_pm,
			       mag,
			       static_cast<int32_t>(bsc_number)
			       }
			   );

      }
    }
  }
  // cout << "make_vector_catalog(): size = " << vect_cat.size() << endl;
  src.close();
  return std::move( vect_cat );
}

void stream_array_item_no_bracket( star_data data, uint32_t index, std::ostream& destination ){
  sexagesimal::Sexagesimal RA;
  RA.set_binary_data( data.RAdata );
  sexagesimal::Sexagesimal Dec;
  Dec.set_binary_data( data.DECdata );
  std::string indent = "  ";

  destination << indent << "/*  ";
  destination << "BSC ";
  destination << data.BSCnum;
  destination << "   ";
  destination << RA.to_string();
  destination << "   ";
  destination << Dec.to_dms_string();
  destination << "   Vmag = ";
  destination << data.magnitude;
  destination << "  index = ";
  destination << index;
  destination << "  ";

  if (  navigation_star::is_navigation_star( data.BSCnum ) ){
    int navnum = navigation_star::get_navigation_star_num(data.BSCnum);
    destination << "   naviagation star ";
    destination << navnum;
    destination << "  ";
    destination << navigation_star::get_navigation_star_name(navnum);
  }
  destination << "  */";
  destination << endl;
  destination << indent << indent;
  destination << "0x";
  destination << std::hex;
  destination << std::hex << data.RAdata;
  destination << "," << endl;
  destination << indent << indent;
  destination << "0x";
  destination << std::hex << data.DECdata;
  destination << "," << endl;
  destination << indent << indent;
  destination << std::dec;
  destination << data.RApm;
  destination << "," << endl;
  destination << indent << indent;
  destination << data.DECpm;
  destination << "," << endl;
  destination << indent << indent;
  destination << data.magnitude;
  destination << "," << endl;
  destination << indent << indent;
  destination << data.BSCnum;
  destination << "," << endl;
}

void make_header_file( std::string file_basename,  std::string namespace_name, std::string array_name, size_t array_size ){
  std::string filename = file_basename + ".h";
  std::string include_guard = file_basename + "_H_";
  for( uint i=0; i<include_guard.size(); ++i ){
    include_guard[i] = toupper( include_guard[i] );
  }
  std::string tab = "  ";
  std::ofstream file_h;
  //  cout << "header file name = " << filename << endl;
  file_h.open( filename );
  file_h << "#ifndef " << include_guard << endl;
  file_h << "#define " << include_guard << endl;
  file_h << endl;
  file_h << "#include <array>" << endl;
  file_h << "#include \"star_data.h\"" << endl;
  file_h << endl;
  file_h << "/*  THIS IS A MACHINE GENERATED FILE */\n";
  file_h << endl;
  file_h << "namespace ";
  file_h << namespace_name << "{" << endl;

  file_h << tab;
  file_h << "extern const ";
  file_h << "std::array< ";
  file_h << "star_data";
  file_h << ", ";
  file_h << array_size;
  file_h << " > ";
  file_h << array_name;
  file_h << ";";
  file_h << endl;

  file_h << "}" << endl;

  file_h << endl;
  file_h << "#endif  //  " << include_guard << endl;
  file_h.close();
}

void make_cpp_file( std::string file_basename,  std::string namespace_name, std::string array_name, std::vector<star_data> &catalog ){
  std::string filename = file_basename + ".cpp";
  std::string header_filename = file_basename + ".h";
  std::string tab = "  ";
  std::ofstream file_cpp;
  file_cpp.open( filename );

  file_cpp << "#include ";
  file_cpp << "\"";
  file_cpp << header_filename;
  file_cpp << "\"";
  file_cpp << endl;
  file_cpp << endl;
  file_cpp << endl;
  file_cpp << "/*  THIS IS A MACHINE GENERATED FILE */\n";
  file_cpp << endl;


  file_cpp << endl << endl;
  file_cpp << "/*  ";

  file_cpp << namespace_name;
  file_cpp << "::";
  file_cpp << array_name;
  file_cpp << ".size() = ";
  file_cpp << catalog.size();
  file_cpp << "  */" << endl;

  file_cpp << endl;

  file_cpp << "const std::array";
  file_cpp << "< ";
  file_cpp << "star_data";
  file_cpp << ", ";
  file_cpp << catalog.size();
  file_cpp << " > ";

  file_cpp << namespace_name;
  file_cpp << "::";
  file_cpp << array_name;

  file_cpp << "{";
  file_cpp << endl;
  uint32_t index = 0;
  for( auto item=catalog.begin(); item!=catalog.end(); ++item, ++index ){
    //    print_array_item_no_bracket( *item );
    stream_array_item_no_bracket( *item, index, file_cpp );
  }
  file_cpp << endl;
  file_cpp << "};";
  file_cpp << endl;

  file_cpp.close();
}


void make_header_and_cpp_files( std::string file_basename,  std::string namespace_name, std::string array_name ){
  auto bsc_catalog = make_vector_catalog();
  make_header_file( file_basename, namespace_name, array_name, bsc_catalog.size() );
  make_cpp_file( file_basename, namespace_name, array_name, bsc_catalog );
}


int main ()
{
  make_header_and_cpp_files( "neo_bsc_starlist", "flash_memory_array","bsc_array" );
  //  print_pod_check();
  return 0;
}
