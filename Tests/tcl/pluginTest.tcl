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
package require Tcltest

#
# Simple test to see if the commands actually work...
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

#
# Check we can create a command that expects a
# double
#

#
# 1. Check arg count...
#
DemandError "HalfIt" "wrong # args:"
DemandError "HalfIt 2.0 3.0" "wrong # args:"

#
# 2. Type checking...
#
DemandError "HalfIt hello" "expected floating-point number but got"
DemandError "HalfIt {one two}" "expected floating-point number but got"

#
# 3. Check the result
#
set result [HalfIt 2.0]

#
# 3. Check command error...
#
DemandError "HalfIt 0.0" "HalfIt: can't half 0!"

if { $result != 1.0 } {
   puts "HalfIt, Result (1.0) : $result"
   exit 1
}

#
# Check we can create a command that expects a
# int
#

#
# 1. Check arg count...
#
DemandError "DoubleIt" "wrong # args:"
DemandError "DoubleIt 2 3" "wrong # args:"

#
# 2. Type checking...
#
DemandError "DoubleIt hello" "expected integer but got"
DemandError "DoubleIt {one two}" "expected integer but got"
DemandError "DoubleIt 2.0" "expected integer but got"

#
# 3. Check the result
#
set result [DoubleIt 2]

if { $result != 4 } {
   puts "DoubleIt, Result (4.0) : $result"
   exit 1
}

#
# 3. Check command error...
#
DemandError "DoubleIt 0" "DoubleIt: can't double 0!"

#
# Check we can create a command that expects a
# long
#

#
# 1. Check arg count...
#
DemandError "SquareIt" "wrong # args:"
DemandError "SquareIt 2 3" "wrong # args:"

#
# 2. Type checking...
#
DemandError "SquareIt hello" "expected integer but got"
DemandError "SquareIt {one two}" "expected integer but got"
DemandError "SquareIt 2.0" "expected integer but got"

#
# 3. Check the result
#
set result [SquareIt 3]

if { $result != 9 } {
   puts "SquareIt, Result (9.0) : $result"
   exit 1
}

#
# 3. Check command error...
#
DemandError "SquareIt 0" "SquareIt: can't square 0!"

#
# Check we can create a command that expects a
# string
#

#
# 1. Check arg count...
#
DemandError "SayHi" "wrong # args:"
DemandError "SayHi 2 3" "wrong # args:"

#
# 3. Check the result
#
set result [SayHi "Luke"]

if { $result != "Hi, Luke" } {
   puts "SquareIt, Result (Hi, Luke) : $result"
   exit 1
}

#
# 3. Check command error...
#
DemandError "SayHi -" "SayHi: You need to give me a name!"

#
# Check we can create a command that expects 
# multiple Types
#
DemandError "Repeat" "wrong # args:"
DemandError "Repeat string" "wrong # args:"
DemandError "Repeat string 1 1" "wrong # args:"

#
# 2. Type checking...
#    (remember in TCL, everything is a string...)
#
DemandError "Repeat 1 string" "expected integer"
DemandError "Repeat 1 1.0" "expected integer"

#
# 3. Check command error...
#
DemandError {Repeat "Hello World!" -1} "must be a positive"


set expected "Hello World!\nHello World!\nHello World!\nHello World!\n"

set result [Repeat "Hello World!" 4]

if {  $result != $expected } {
   puts "Repeat, Result ($expected) : >$result<"
   exit 1
}
