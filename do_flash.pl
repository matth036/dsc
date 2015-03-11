#!/usr/bin/perl
#
# @TODO Verify that the user (whoami) is in the plugdev group.
# Use configuration files in the openocd distribution if possible.
#
use Net::Telnet;
$numArgs = $#ARGV + 1;
#
# kill any currently running instance of openocd
#
$ps_output = `ps --no-headers -C openocd -o pid | head -1`;
#
# The preceeding system command successfully captures the pid of a running openocd instance.
chop( $ps_output );
# 
if( $ps_output =~ m/^\s*\d+\s*$/ ) {  # 1 or more digits possbibly proceeded or followed by white space.
    print "Found pid of currently running openocd instance: $ps_output \n";
    print "Will kill -9 $ps_output\n";
    system( "kill -9 $ps_output" );
    sleep 1;
}else{
    print "No current openocd process to kill ...  $ps_output \n";
}
#
# If some how there are multiple instances of openocd running, 
# calling this program repeatedly with no arguments should kill them off.
#
if($numArgs != 1){
  die( "Usage $0 project_to_flash.bin \n");
}
# Fresh start with openocd 
#
print "starting openocd ... \n";
#$openocd_command = "/usr/local/bin/openocd -f ./ocd/olimex-arm-usb-ocd-h.cfg -f ./ocd/scripts/target/stm32f4x.cfg -l openocd.log &";

#$openocd_command = "/usr/local/bin/openocd -f interface/olimex-arm-usb-ocd-h.cfg -f -f interface/ftdi/olimex-arm-usb-ocd.cfg target/stm32f4x.cfg -l openocd.log &";
$openocd_command = "/usr/local/bin/openocd  -f interface/ftdi/olimex-arm-usb-ocd-h.cfg -f target/stm32f4x.cfg -l openocd.log &";
# $openocd_command = "/usr/local/bin/openocd -f ./dumb.cfg -l openocd.log &";
# /usr/local/deprecated/share/openocd/scripts/interface/ftdi/olimex-arm-usb-ocd-h.cfg

print $openocd_command;
print "\n";
system( $openocd_command );
sleep 1;
$file = $ARGV[0];
$ip = "127.0.0.1";
$port = 4444;
$timeout = 60;
$telnet = new Net::Telnet (
  Port => $port,
  Timeout=> $timeout,
  Errmode=>'die',
  Prompt =>'/>/');
print "Using telnet with Timeout = $timeout\n";
$telnet->open($ip);
print $telnet->cmd('reset halt');
print $telnet->cmd('flash probe 0');
print $telnet->cmd('stm32f2x mass_erase 0');
print $telnet->cmd('flash write_bank 0 '.$file.' 0');
print $telnet->cmd('reset halt');
print $telnet->cmd('exit');
print "Flash seems to have worked. Isn't this wonderful? \n";
