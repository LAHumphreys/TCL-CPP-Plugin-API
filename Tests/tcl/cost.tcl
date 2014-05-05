#!/bin/sh
# \
$PROJECT_ROOT_DIR/tcl8.0.5/bin/tclsh8.0 $0
# \
exit

#
# Setup the library...
#
pkg_mkIndex -verbose $env(PROJECT_ROOT_DIR)/exports/lib
set auto_path "$auto_path $env(PROJECT_ROOT_DIR)/exports/lib"
package require Tcltime

proc GetSecondItem { items } {
    return [lindex $items 1]
}

proc GetSecondItem_Direct { items } {
    return [lindex $items 1]
}

proc GetSecondItem_Indirect { items } {
    return [lindex $items 1]
}

puts "Doing 1 million lindexes..."
set start [times_now]
for { set i 0 } { $i < 1e6 } { incr i } {
    GetSecondItem {onen two three}
}
set end [times_now]
puts "done"

set dur [times_diff "$start" "$end"]
puts "Start: $start"
puts "End: $end"
puts "Duration (ms): $dur"
puts "Cost (us): [expr $dur / 1e3]"

puts "Doing 1 million lindexes with indirect calls..."
times_install_proc "dummy"
set start [times_now]
for { set i 0 } { $i < 1e6 } { incr i } {
    times_enter_proc 0
    GetSecondItem_Indirect {onen two three}
    times_leave_proc 0
}
set end [times_now]
puts "done"
set dur [times_diff "$start" "$end"]
puts "Start: $start"
puts "End: $end"
puts "Duration (ms): $dur"
puts "Cost (us): [expr $dur / 1e3]"

puts "Doing 1 million lindexes with direct trace calls..."
times_install_proc "GetSecondItem_Direct"
trace add execution GetSecondItem_Direct enter times_enter_proc_GetSecondItem_Direct
trace add execution GetSecondItem_Direct leave times_leave_proc_GetSecondItem_Direct
set start [times_now]
for { set i 0 } { $i < 1e6 } { incr i } {
    GetSecondItem_Direct {onen two three}
}
set end [times_now]
puts "done"
set dur [times_diff "$start" "$end"]
puts "Start: $start"
puts "End: $end"
puts "Duration (ms): $dur"
puts "Cost (us): [expr $dur / 1e3]"

puts "Doing 1 million lindexes with direct function calls..."
times_install_proc "GetSecondItem"
set start [times_now]
for { set i 0 } { $i < 1e6 } { incr i } {
    times_enter_proc_GetSecondItem
    GetSecondItem {onen two three}
    times_leave_proc_GetSecondItem
}
set end [times_now]
puts "done"
set dur [times_diff "$start" "$end"]
puts "Start: $start"
puts "End: $end"
puts "Duration (ms): $dur"
puts "Cost (us): [expr $dur / 1e3]"

puts [times_print_events]
