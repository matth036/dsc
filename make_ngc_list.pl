#!/usr/bin/perl
use strict;
use warnings;
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
   $_ = $num;
   /(^\d{$len})(\d)(\d*$)/;
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

sub dms_xxx{
    my ($dd, $mm, $ss, $xxx) = @_;
    $_ = $dd;
    /([+-]?)(\d{1,2})/;
    my $sign = $1;
    $dd = $2;
    $dd = zero_pad_left( $dd, 2 );
    $mm = zero_pad_left( $mm, 2 );
    $ss = zero_pad_left( $ss, 2 );
    $xxx = round_canonically( $xxx, 3);
    return $sign . $dd . ":" . $mm . ":" . $ss . "." . $xxx;
}

sub sexify_to_array_fallback{
    my $input_value = shift @_ ;
    my $sign;
    my $dd;
    my $mm;
    my $ss;
    my $xxx;
    $_ = $input_value;
    if( s/([+-]?)(\d{1,3})// ){
	$sign = $1;
	$dd = $2;
    }else{
	exit(1);
    }
    if( s/([0-5]\d)// ){
	$mm = $1;
    }else{
	exit(2);
    }
    if( s/([0-5]\d)// ){
	$ss = $1;
    }else{
	exit(3);
    }
    if( s/(\d*)// ){
	$xxx = $1;
    }else{
	$xxx = 0;
    }
    $dd = zero_trim_left($dd);
    $mm = zero_trim_left($mm);
    $ss = zero_trim_left($ss);
    $xxx = round_canonically( $xxx, 3 );
    $xxx = zero_trim_left($xxx);
    return ( $sign.$dd, $mm, $ss, $xxx );
}


sub sexify_to_array{
    my $input_value = shift @_ ;
    my $hex_value;
    $_ = $input_value;
    if (  /([+-]?)(\d{1,3})([hH\ :\272\260])\ ?(\d\d)([mM\ :\'])\ ?(\d\d)([\.\"]|\'{2}|\ )(\d*)/ ){
	my $sign = $1;
	my $dd = $2;
	my $mm = $4;
	my $ss = $6;
	my $xxx = $8;
	$dd = zero_trim_left($dd);
	$mm = zero_trim_left($mm);
	$ss = zero_trim_left($ss);
	$xxx = round_canonically( $xxx, 3 );
	$xxx = zero_trim_left($xxx);
	return ( $sign.$dd, $mm, $ss, $xxx );
    }else{
	return sexify_to_array_fallback($input_value);
    }
}



sub sexify_to_constructor {
    my $input_value = shift @_ ;
    $_ = $input_value;
    if (  /([+-]?)(\d{1,3})([hH\ :\272\260])\ ?(\d\d)([mM\ :\'])\ ?(\d\d)([\.\"])(\d*)/ ){
	my $sign = $1;
	my $dd = $2;
	my $mm = $4;
	my $ss = $6;
	my $xxx = $8;
	$dd = zero_trim_left($dd);
	$mm = zero_trim_left($mm);
	$ss = zero_trim_left($ss);
	$xxx = round_canonically( $xxx, 3 );
	$xxx = zero_trim_left($xxx);
	return "sexagesimal::Sexagesimal($sign$dd, $mm, $ss, $xxx)"; 
    }else{
	return "sexagesimal::Sexagesimal(0, 0, 0, 0)"; 
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

sub print_include_guard_top{
    print "#ifndef _NGC_LIST_H";
    print "\n";
    print "#define _NGC_LIST_H";
    print "\n";
}

sub print_include_guard_bottom{
    print "\n";
    print "#endif ";
    print "    /*  _NGC_LIST_H  eof */ ";
    print "\n";
}


sub print_includes{
    print "#include \"sexagesimal.h\" ";
    print "\n";
    print "#include <stdint.h>";
    print "\n";

}


sub print_warning{
    print "   /*  WARNING: THIS IS A MACHINE GENERATED FILE. */";
    print "\n";
}

sub print_source_info{
    print "   /*   ";
    print "See script: " . $0 ; 
    print "   */";
    print "\n";
}


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
    print "float dimesion_a;";
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





my $source_file_name = "DPublic_HCNGC.txt";

my $sample_line;
my $num_objects = 0;
my @fields;

print_include_guard_top();
print "\n";
print_warning();
print "\n";
print_source_info();
print "\n";
print_includes();
print "\n";
print "\n";
print_c_plus_plus_head();
print "\n";
print "\n";

print_ngc_typedef();

print "\n";
print "\n";
open(INPUT_FILE,"<$source_file_name" );


print_array_head();
while( $_ = <INPUT_FILE> ){
    if( /^\d{1,4}\|/ ){
	++$num_objects;
	print_item $_;
	print ",\n";
    }
}
print_array_tail();

print_list_size( $num_objects );

print "\n";
print "\n";
print_c_plus_plus_tail();
print "\n";
print "\n";
print_include_guard_bottom();

