#!/bin/sh
# \
$PROJECT_ROOT_DIR/tcl8.0.5/bin/tclsh8.0 $0
# \
exit

#
# Helper proc for validating error messages
#
proc DemandError { cmd errorString } {
    if { [catch $cmd result] } {
        #Got an error - good!
        if { ![regexp $errorString $result] } {
            puts "$cmd ($errorString):  Wrong error! - $result"
            exit 1
        }
    } else {
        puts "$cmd ($errorString) did not raise an error!"
        exit 1
    }
}

#
# Setup the library...
#
pkg_mkIndex -verbose $env(PROJECT_ROOT_DIR)/exports/lib
set auto_path "$auto_path $env(PROJECT_ROOT_DIR)/exports/lib"
package require Tcltime

#
# Check we can add procs...
#
set result [times_install_proc "hello_world"]
if { $result != 0 } {
    puts "First: $result"
    exit 1
}
set result [times_install_proc "hello_world2"]
if { $result != 1 } {
    puts "Second: $result"
    exit 1
}


times_enter_proc 1
times_enter_proc 0
times_leave_proc 0
times_leave_proc 1

puts [times_print_events]
