#!/bin/sh
# \
tclsh $0 $*
# \
exit

#
# Setup the library...
#
pkg_mkIndex -verbose $env(PROJECT_ROOT_DIR)/exports/lib
set auto_path "$auto_path $env(PROJECT_ROOT_DIR)/exports/lib"
package require Tcltime

rename proc _proc

_proc proc { name args code } {
    _proc $name $args $code
    times_install_proc $name
    trace add execution $name enter times_enter_proc_$name
    trace add execution $name leave times_leave_proc_$name
}

proc _Puts { toPrint } {
    puts $toPrint
}

proc Print { toPrint } {
    _Puts $toPrint
}

puts ">$argv<"
puts "**************************************"
source [lindex $argv 0]
puts "**************************************"

Print "Hello World!"

set profile [times_print_events]
set f [open "gathered.csv" "w"]
puts $f $profile
close $f
