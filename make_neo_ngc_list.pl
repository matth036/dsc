#!/usr/bin/perl
use strict;
use warnings;
#
# Global (file scope) variables used in input code gather_data().
#
# my $source_file_name = "short_list.txt";
my $source_file_name = "DPublic_HCNGC.txt";
my @AoH;  # Array of Hashes
my $num_objects = 0;
#
# Global (file scope) variables used in output code.
#
my $filename_root = "neo_whiz_bang_ngc_list";
my $header_filename = $filename_root . ".h";
my $cpp_filename = $filename_root . ".cpp";
my $header_include_guard = uc( $filename_root . "_H_" );
my $namespace_name = "flash_memory_array";
my $array_name = "ngc_list";
my $element_cpp_classname = "ngc_data";
my $indent = "  ";
#
# start of main.
print "Gathering NGC data from $source_file_name.\n";
gather_data(my $vebosity = 1);
make_header_file();
make_cpp_file();
# spew_data();
print "Number of objects = " . $num_objects;
print "\n";
exit 0;
# end of main.

#
# Stuff @AoH with data gathered from "DPublic_HCNGC.txt"
#
sub gather_data{
    my $verbosity = shift;
    open(my $in, "<", $source_file_name ) or die "can't open file " . $source_file_name;
    if( $verbosity && $verbosity > 0 ){
	print "Thanks to http://www.ngcic.org.\n";
	print "-----------------------------------------------------------------------------|\n";
    }
    $num_objects = 0;
    while( <$in> ){
	if(  /^\d{1,4}\|/ ){
	    my $file_line;
	    chomp( $file_line = $_ );
	    my %current_item = ();
	    $current_item{ NGC_number } = extract_NGC_number( $file_line );
	    my $size_string = extract_size_string( $file_line );
	    my @dimensions = parse_size_string( $size_string );
	    $current_item{ size_a } = $dimensions[0];
	    $current_item{ size_b } = $dimensions[1];
	    $current_item{ magnitude_string } = extract_magnitude_string( $file_line );
	    if( $current_item{ magnitude_string } =~ /^[+-]?\d+\.?\d*$/ ){ 
		$current_item{ magnitude } = $current_item{ magnitude_string };
	    }else{
		$current_item{ magnitude } = "99.9";
	    }
	    $current_item{ RA_string } = extract_RA_string( $file_line );
	    $current_item{ Declination_string } = clense_declination_string( extract_Declination_string( $file_line ));
	    $current_item{ RA_data } = sexagesimal_data_string( parse_RA_string( $current_item{RA_string} ) );
	    $current_item{ Declination_data } = sexagesimal_data_string( parse_Declination_string( $current_item{Declination_string} ) );
	    $current_item{ RA_numerical } = numerically_evaluate_sexagesimal( parse_RA_string( $current_item{RA_string} ) );
	    $current_item{ Declination_numerical } = numerically_evaluate_sexagesimal( parse_Declination_string( $current_item{Declination_string} ) );
	    $current_item{ object_type } = extract_object_type_string( $file_line );

	    push( @AoH, \%current_item );
	
	    ++$num_objects;
	    if( $num_objects % 100 == 0 ){
		if( $verbosity && $verbosity > 0 ){
		    print ".";
		}
	    }
	}
    }
    if( $verbosity && $verbosity > 0 ){
	print "\n";
    }
    close( $in );
}

sub make_header_file{
    open( OUTFILE_H, ">" . $header_filename )  or die "Cannot open " . $header_filename . "   \n";

    my $array_size = $num_objects;
    print_include_guard_top( $header_include_guard );
    print_mechanization_warning( \*OUTFILE_H );
    print OUTFILE_H "\n";

    print OUTFILE_H "#include <inttypes.h>";
    print OUTFILE_H "\n";
    print OUTFILE_H "#include <array>";

    print OUTFILE_H "\n";
    print OUTFILE_H "\n";


    print OUTFILE_H "typedef struct";
    print OUTFILE_H "{";
    print OUTFILE_H "\n";

    print OUTFILE_H $indent;
    print OUTFILE_H "uint32_t";
    print OUTFILE_H " ";
    print OUTFILE_H "RA_data";
    print OUTFILE_H ";";
    print OUTFILE_H "\n";

    print OUTFILE_H $indent;
    print OUTFILE_H "uint32_t";
    print OUTFILE_H " ";
    print OUTFILE_H "DEC_data";
    print OUTFILE_H ";";
    print OUTFILE_H "\n";

    print OUTFILE_H $indent;
    print OUTFILE_H "uint32_t";
    print OUTFILE_H " ";
    print OUTFILE_H "NGC_number";
    print OUTFILE_H ";";
    print OUTFILE_H "\n";

    print OUTFILE_H $indent;
    print OUTFILE_H "float";
    print OUTFILE_H " ";
    print OUTFILE_H "Vmag";
    print OUTFILE_H ";";
    print OUTFILE_H "\n";

    print OUTFILE_H $indent;
    print OUTFILE_H "float";
    print OUTFILE_H " ";
    print OUTFILE_H "dimension_a";
    print OUTFILE_H ";";
    print OUTFILE_H "\n";

    print OUTFILE_H $indent;
    print OUTFILE_H "float";
    print OUTFILE_H " ";
    print OUTFILE_H "dimension_b";
    print OUTFILE_H ";";
    print OUTFILE_H "\n";

    print OUTFILE_H "} ";
    print OUTFILE_H $element_cpp_classname;
    print OUTFILE_H ";";
    print OUTFILE_H "\n";
    print OUTFILE_H "\n";
    print OUTFILE_H "namespace $namespace_name {\n";
    print OUTFILE_H $indent;
    print OUTFILE_H "extern const std::array< ";
    print OUTFILE_H $element_cpp_classname;
    print OUTFILE_H ", ";
    print OUTFILE_H $array_size;
    print OUTFILE_H " > ";
    print OUTFILE_H $array_name;
    print OUTFILE_H ";";
    print OUTFILE_H "\n";
    print OUTFILE_H "}\n";
    print OUTFILE_H "\n";
    print_include_guard_bottom( $header_include_guard );

    close( OUTFILE_H );
}

sub make_cpp_file{
    open( OUTFILE_CPP, ">" . $cpp_filename )  or die "Cannot open " . $cpp_filename . "   \n";

    my $array_size = $num_objects;
    print_mechanization_warning( \*OUTFILE_CPP );

    print OUTFILE_CPP "#include ";
    print OUTFILE_CPP "\"";
    print OUTFILE_CPP $header_filename;
    print OUTFILE_CPP "\"";
    print OUTFILE_CPP "\n";
    
    print OUTFILE_CPP "#include ";
    print OUTFILE_CPP "<";
    print OUTFILE_CPP "array";
    print OUTFILE_CPP ">";
    print OUTFILE_CPP "\n";
    print OUTFILE_CPP "\n";
    print OUTFILE_CPP "const std::array< ";
    print OUTFILE_CPP $element_cpp_classname;
    print OUTFILE_CPP ", ";
    print OUTFILE_CPP $array_size;
    print OUTFILE_CPP " > ";
    print OUTFILE_CPP $namespace_name;
    print OUTFILE_CPP "::";
    print OUTFILE_CPP $array_name;
    print OUTFILE_CPP "{\n";
    for( my $index = 0; $index < $num_objects; ++$index ){
	print OUTFILE_CPP $indent;
	print OUTFILE_CPP "/* ";
	print OUTFILE_CPP "index = " . $index;
	print OUTFILE_CPP "   ";
	print OUTFILE_CPP "NGC ";
	print OUTFILE_CPP $AoH[$index]{NGC_number};

	print OUTFILE_CPP " */";
	print OUTFILE_CPP "\n";
	print OUTFILE_CPP $indent;
	# print OUTFILE_CPP "uint32_t ";
	print OUTFILE_CPP $AoH[$index]{RA_data};
	print OUTFILE_CPP ",\n";

	print OUTFILE_CPP $indent;
	# print OUTFILE_CPP "uint32_t ";
	print OUTFILE_CPP $AoH[$index]{Declination_data};
	print OUTFILE_CPP ",\n";

	print OUTFILE_CPP $indent;
	# print OUTFILE_CPP "uint32_t ";
	print OUTFILE_CPP $AoH[$index]{NGC_number};
	print OUTFILE_CPP ",\n";

	print OUTFILE_CPP $indent;
	print OUTFILE_CPP "(float) ";
	print OUTFILE_CPP $AoH[$index]{magnitude};
	print OUTFILE_CPP ",";
	print OUTFILE_CPP "               /* Vmag */";
	print OUTFILE_CPP "\n";


	print OUTFILE_CPP $indent;
	print OUTFILE_CPP "(float) ";
	print OUTFILE_CPP $AoH[$index]{size_a};
	print OUTFILE_CPP ",";
	print OUTFILE_CPP "               /* dimension_a */";
	print OUTFILE_CPP "\n";


	print OUTFILE_CPP $indent;
	print OUTFILE_CPP "(float) ";
	print OUTFILE_CPP $AoH[$index]{size_b};
	print OUTFILE_CPP ",";
	print OUTFILE_CPP "               /* dimension_b */";
	print OUTFILE_CPP "\n";


    }
    print OUTFILE_CPP "\n";
    print OUTFILE_CPP "\n";
    print OUTFILE_CPP "\n";
    print OUTFILE_CPP "\n";    
    print OUTFILE_CPP "};\n";
    print OUTFILE_CPP "\n";

    close( OUTFILE_CPP );
}

sub spew_data{
    for( my $index = 0; $index < $num_objects; ++$index ){
	print $AoH[$index]{NGC_number};
	print "  ";
	print $AoH[$index]{object_type};
	print "  ";
	print $AoH[$index]{magnitude};
	print "  ";
	print $AoH[$index]{RA_data};
	print "  ";
	print $AoH[$index]{Declination_data};
	print "  ";
	print $AoH[$index]{RA_string};
	print "  ";
	print $AoH[$index]{Declination_string};
	print "  ";
	print $AoH[$index]{ size_a };
	print "  ";
	print $AoH[$index]{ size_b };
	#print "  ";
	#print $AoH[$index]{ magnitude }*$AoH[$index]{ size_b }*$AoH[$index]{ size_b };
	
	# print "  ";
	# print $AoH[$index]{magnitude_string};
	print "\n";
    }
}

sub parse_RA_string{
    my $RA_string = shift;
    my $HH;
    my $mm;
    my $ss;
    my $x;
#
#
#                           /-- Two initial decimals followed by 'h' and a space.  Potentially optional leading zeros are always present.
#                           |
    if( $RA_string =~ s/^((\d\d)h\s)// ){
	$HH = $2;
    }else{
	die;
    }
#
    if( $RA_string =~ s/^((\d\d)m\s)// ){
	$mm = $2;
    }else{
	die;
    }
#
#                            /-- Two initial decimals followed by a literal .
#                            |
#                            |          /-- Usually one sometimes two decimals followed by 's' at the end of the string.
#                            |          |
    if( $RA_string =~ s/^((\d\d)[.](\d{1,2})s$)// ){
	$ss = $2;
	$x = $3;
    }else{
	print "   -->";
	print $RA_string;
	print "<--  Dying ... ";
	print "\n";
	die;
    }
    my $xxx = zero_pad_right( $x, 3 );
    return ( $HH, $mm, $ss, $xxx );
}

sub parse_Declination_string{
    my $Dec_string = shift;
    my $sDD;
    my $mm;
    my $ss;
    my $x;
#
#                               /-- A plus or minus sign. The potentially optional plus is always present.
#                               |
#                               |   /-- Two decimal digits.
#                               |   |
    if(    $Dec_string =~ s/^([+-]\d\d)// ){
	$sDD = $1;
    }else{
	print "   -->";
	print $Dec_string;
	print "<--  Dying ... ";
	print "\n";	
	die;
    }
#                           /-- Unknown character at the start.  Probably a degree symbol.
#                           |    
#                           |    /-- Two decimal digits.
#                           |    |
#                           |    |         /-- A space.
#                           |    |         |
    if( $Dec_string =~ s/^(.\ (\d\d)([\'.])\s)// ){  #In one case, NGC 467, the apostrophe is a period. (So, is the next number decimal minutes or seconds?)
	$mm = $2;
    }else{
	print "   -->";
	print $Dec_string;
	print "<--  Dying ... ";
	print "\n";
	die;
    }
#
#                           /-- Two initial decimal digits.
#                           |
#                           |      /-- A very few cases have decimal seconds.
#                           |      |
#                           |      |           /-- Sometimes there are two apostophies instead of a double quote. Sometimes nothing.
#                           |      |           |
    if( $Dec_string =~ s/^(\d\d)[.(\d+)]?[\"|\'\']?// ){
	$ss = $1;
	if( $2 ){
	    $x = $2;
	}else{
	    $x = "0";
	}
    }else{
	print "   -->";
	print $Dec_string;
	print "<--  Dying ... ";
	print "\n";
	die;
    }
    my $xxx = zero_pad_right( $x, 3 );
    return ($sDD, $mm, $ss, $xxx);  # Degrees (signed), minutes (arc), seconds (arc), decimal seconds.
    # return $sDD . " | " . $mm . " | " . $ss . " | " . $x;
}

#
# Note decimal seconds should be three digits with leading zeros if needed.
#
sub numerically_evaluate_sexagesimal{
    my $DD = shift;
    my $mm = shift;
    my $ss = shift;
    my $xxx = shift;
    my $value = $ss . "." . $xxx;
    $value = $mm + $value/60.0;
    $value = $DD + $value/60.0;
    return $value;
}

sub parse_size_string{
    my $string = shift;
    my @fields = split( /[X|\&]/, $string );
    my $value_0;
    my $value_1;
    if( scalar(@fields) == 1 ){
	$value_0 = $value_1 = parse_angular_dimension( $fields[0] );
    }elsif( scalar(@fields) == 2 ){
	$value_0 = parse_angular_dimension( $fields[0] );
	$value_1 = parse_angular_dimension( $fields[1] );
    }elsif( scalar(@fields) >=3 ){
	$value_1 = pop( @fields );
	$value_0 = pop( @fields );
	$value_1 =~ s/\s//g;
	$value_0 =~ s/\s//g;
	$value_1 = parse_angular_dimension( $value_1 );
	$value_0 = parse_angular_dimension( $value_0 );
    }else{
	print "\n --->";
	print $string;
	print "<---\n";
	die;
    }
    return ( $value_0, $value_1 );
}

sub parse_angular_dimension{
    my $item = shift;
    if( $item =~  /^([1-9]\d*)(\')$/ ){ # An integer followed by '.
	return $1;   
    }
    elsif( $item =~ /((\d*)([.])(\d))\'$/ ){ # A float followed by '
	return $1;
    }
    elsif( $item =~  /^([1-9]\d*)(\")$/ ){  # An integer followed by ".
	return ($1/60.0);
    }
    elsif( $item =~ /((\d*)([.])(\d))\"$/ ){ # A float followed by ".
	return ($1/60.0);
    }

    return "0";
}

sub extract_NGC_number{
    my $line = shift;
    my @fields = split( /\|/, $line );
    return $fields[0];
}

sub extract_RA_string{
    my $line = shift;
    my @fields = split( /\|/, $line );
    return $fields[5];
}

sub extract_Declination_string{
    my $line = shift;
    my @fields = split( /\|/, $line );
    return $fields[6];
}

sub extract_object_type_string{
    my $line = shift;
    my @fields = split( /\|/, $line );
    return $fields[13];
}

sub extract_magnitude_string{
    my $line = shift;
    my @fields = split( /\|/, $line );
    my $mag = $fields[17];
    return $mag;
}

sub extract_size_string{
    my $line = shift;
    my @fields = split( /\|/, $line );
    return $fields[15];
}

#
# Slow but effective.
# The system call of ./sex_to_hex is the slow part.
#
sub sexagesimal_data_string{
    my $hex_value;
    my $dd = shift;
    my $mm = shift;
    my $ss = shift;
    my $xxx = shift;
    $xxx = zero_pad_right( $xxx, 3 );
    $hex_value = `./sex_to_hex $dd $mm $ss $xxx`;
    chop( $hex_value );
    return "0x" . $hex_value; 
}

# The symbol after degees appears as \272 in emacs. Presumably a degree sign.
sub clense_declination_string{
    my $string = shift;
    $string =~ s/^([+-]\d\d)(.\ )(\d)/$1d\ $3/;
    return $string;
}

#
# zero_pad_right($num, $len) appends zeros to the right of $num 
#
sub zero_pad_right {
    my ($num, $len) = @_;
    if( $len  < length $num ){
	return; # Return with no value, creating an error.
    }
    return $num . '0' x ( $len - length $num );
}
#
# zero_pad_left($num, $len) appends zeros to the left of $num 
#
sub zero_pad_left {
    my ($num, $len) = @_;
    if( $len  < length $num ){
	return; # Return with no value, creating an error.
    }
    return '0' x ( $len - length $num ) . $num;
}

sub zero_trim_left {
    $_ = shift @_;
    /(^0*)(\d*$)/;	
    my $result = $2;
    if( $result eq "" ){
	return 0;
    }else{
	return $result;
    }
}

sub zero_trim_right {
    $_ = shift @_;
    /(^\d*[1-9])(0*$)/;	
    my $result = $1;
    if( $result eq "" ){
	return 0;
    }else{
	return $result;
    }    
}

sub round_canonically {
   my ($num, $len) = @_;
   $num = zero_pad_right( $num, $len + 2 );
   $num =~ /(^\d{$len})(\d)(\d*$)/;
   my $front_digits = $1;
   my $next_digit = $2;
   my $back_digits = $3;
   my $round_up = zero_trim_left( $front_digits );
   ++$round_up;
   $round_up = zero_pad_left( $round_up, $len );
   #
   #  The correct return value is either $round_up or $front_digits.
   #  Use conditionals to cover all the stupid cases.
   #
   if( ($next_digit == 5) && ($back_digits == 0) ){
       if( $round_up % 2 == 0 ){
	   return $round_up;
       }else{
	   return $front_digits;
       }
   }elsif(  ($next_digit == 5) && ($back_digits > 0) ){
       return $round_up;
   }elsif( $next_digit > 5 ){
       return $round_up;
   }else{
       return $front_digits;
   }
}

#
# Slow but mostly effective.
# The system call of ./sex_to_hex is the slow part.
#
sub sexify_to_data {
    my $hex_value;
    my @elements = sexify_to_array( shift @_ );
    if( @elements ){
	$hex_value = `./sex_to_hex $elements[0] $elements[1] $elements[2] $elements[3]`;
    }else{
	$hex_value = 0;
    }
    chop( $hex_value );
    return "0x" . $hex_value; 
}


sub make_numeric{
    my $number = shift@_;
    if( $number eq "\205" ){
	return 0.0;
    }
    return ($number + 0.0);
}

sub decompose_dimensions{
    my @value = split( "X",shift @_);
    if( $value[0] eq "\205" ){
	return (0.0,0.0);
    }
    if( @value == 1 ){
    # If the catalog gives only one dimension, duplicate it.
	$value[1] = $value[0];
    }
    if( @value == 0 ){
	return (0.0,0.0);
    }
    my $item;
    foreach $item ( @value ){
	$_ = $item;
	/(\d*\.?\d*[Ee]?\d+?)/;
	if( $1 ){
	    $item = $1 + 0.0;
	}else{
	    $item = 0.0;
	}
    }
    return @value;
}

sub print_includes{
    print "#include \"sexagesimal.h\" ";
    print "\n";
    print "#include <stdint.h>";
    print "\n";

}

sub print_mechanization_warning{
    my $FILE = shift;
    print $FILE "\n";
    print $FILE "/* WARNING: THIS IS A MACHINE GENERATOR FILE   see $0. */\n";
    print $FILE "\n";
}


sub print_include_guard_top{
    my $text = shift;
    print OUTFILE_H "#ifndef ";
    print OUTFILE_H $text;
    print OUTFILE_H "\n";
    print OUTFILE_H "#define ";
    print OUTFILE_H $text;
    print OUTFILE_H "\n";
}

sub print_include_guard_bottom{
    my $text = shift;
    print OUTFILE_H "#endif  // ";
    print OUTFILE_H $text;
    print OUTFILE_H "\n";
}

sub print_source_info{
    print "   /*   ";
    print "See script: " . $0 ; 
    print "   */";
    print "\n";
}

#
# Keep this. Modify to output to file.
#
sub print_ngc_typedef{
    my $tab = "    ";
    print $tab;
    print "typedef struct{";
    print "\n";

    print $tab;
    print $tab;
    print "uint32_t RA_data;";
    print "\n";

    print $tab;
    print $tab;
    print "uint32_t DEC_data;";
    print "\n";

    print $tab;
    print $tab;
    print "uint32_t NGC_number;";
    print "\n";

    print $tab;
    print $tab;
    print "float Vmag;";
    print "\n";

    print $tab;
    print $tab;
    print "float dimension_a;";
    print "\n";

    print $tab;
    print $tab;
    print "float dimension_b;";
    print "\n";

    print $tab;
    print "}ngc_data;";
    print "\n";
}

sub print_array_head{
    print "static const ngc_data ngc_list[] = {";
    print "\n";
} 

sub print_array_tail{
    print "};";
    print "\n";
} 

sub print_list_size{
    my $size = shift @_;
    print "/*  Number of NGC objects = ";
    print $size;
    print "  */";
    print "\n";
    print "#ifdef __cplusplus";
    print "\n";
    print "static int32_t constexpr NGC_LIST_SIZE =  " . $size . ";";
    print "\n";
    print "#else";
    print "\n";
    print "static const int32_t NGC_LIST_SIZE =  " . $size . ";";
    print "\n";
    print "#endif";
    print "\n";
}

sub print_c_plus_plus_head{
    print "#ifdef __cplusplus";
    print "\n";
    print "extern \"C\"  ";
    print "{";
    print "\n";
    print "#endif";
    print "\n";
}

sub print_c_plus_plus_tail{
    print "#ifdef __cplusplus";
    print "\n";
    print "}";
    print "\n";
    print "#endif";
    print "\n";
}


sub print_item {
    my $line = shift @_;
    my @fields = split( /\|/, $line );
    my $ngc_num = $fields[0];
    my $Vmag = $fields[17];
    
    my $tab = "    ";
    
    print $tab;
    print "{    /* NGC $ngc_num */\n";
    print $tab;
    print $tab;
    print sexify_to_data( $fields[5] );
    print ",";
    print "          /*  RA = ";
    print $fields[5];
    print "  */";
    print "\n";


    print $tab;
    print $tab;
    print sexify_to_data( $fields[6] );
    print ",";
    print "            /* DEC = ";
    print $fields[6];
    print "  */";
    print "\n";

    print $tab;
    print $tab;
    print "(uint32_t) ";
    print $ngc_num;
    print ",\n";

    print $tab;
    print $tab;
    print "(float) ";
    print make_numeric( $Vmag );
    print ",";
    print "                                         /*  Vmag  */";
    print "\n";

    my @dimensions = decompose_dimensions( $fields[15] );

    print $tab;
    print $tab;
    print "(float) ";
    print $dimensions[0];
    print ",";
    print "\n";

    print $tab;
    print $tab;
    print "(float) ";
    print $dimensions[1];
    print "                              /* Dimension = ";
    print $fields[15];
    print "  */";
    print "\n";


    print $tab;
    print "}";
}



