#!/usr/bin/env tclsh8.0

#
# Setup the library...
#
pkg_mkIndex -verbose $env(PROJECT_ROOT_DIR)/exports/lib
set auto_path "$auto_path $env(PROJECT_ROOT_DIR)/exports/lib"
package require Tcltest

#
# Simple test to see if the command actually works...
#
set result [helloWorldCommand]
if { "$result" != "Hello World!" } {
    puts "Result: $result"
    exit 1
}

set result [helloWorldCommand_ptr]
if { "$result" != "Hello World!" } {
    puts "Result: $result"
    exit 1
}

puts "All tests passed successfully"
