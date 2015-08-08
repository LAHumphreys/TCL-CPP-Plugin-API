#include "tcl.h"
#include <Times.h>

#include <TCLInterpreter.h>
#include <sstream>
#include <util_time.h>
#include <functional>

using namespace std;


static EventStack callStack;

static int InstallProc(TCL_Interpreter& interp, string name) {
    int id = ProcMap::Instance().AddProc(name);

    /*
     * It would be simpler to simply have a global Enter / Leave proc
     * that took an id, but this way we remove the machinery requried
     * to interpret / validate the arguments at call-time
     */
    std::function<int(TCL_Interpreter&)> enter_proc(
            [=] ( TCL_Interpreter& interp ) -> int 
            {
                callStack.AddEvent(id,Event::ENTER);
                return TCL_OK;
            });
    std::function<int(TCL_Interpreter&)> leave_proc ( 
            [=] ( TCL_Interpreter& interp ) -> int 
            {
                callStack.AddEvent(id,Event::LEAVE);
                return TCL_OK;
            });
    interp.AddCommand("times_enter_proc_"+string(name),enter_proc);
    interp.AddCommand("times_leave_proc_"+string(name),leave_proc);
    interp.SetResult(id);
    return TCL_OK;
}

static int EnterProc(TCL_Interpreter& interp, int id) {
    callStack.AddEvent(id, Event::ENTER);
    return TCL_OK;
}

static int LeaveProc(TCL_Interpreter& interp, int id) {
    callStack.AddEvent(id, Event::LEAVE);
    return TCL_OK;
}

static int CallStack(TCL_Interpreter& interp) {
    stringstream stack;
    std::vector<Event> events;
    callStack.GetEvents(events);
    for ( std::vector<Event>::reverse_iterator it = events.rbegin(); it != events.rend(); ++it ) {
        stack << it->Info();
    }
    interp.SetResult(stack.str());
    return TCL_OK;
}

static int Now(TCL_Interpreter& interp) {
    interp.SetResult(Time().Timestamp());
    return TCL_OK;
}

static int Diff(TCL_Interpreter& interp, std::string time1, std::string time2) {
    stringstream result;
    result << double(Time(time2).DiffUSecs(Time(time1))) / double(1000);
    interp.SetResult(result.str());
    return TCL_OK;
}


/*
 * Expose this (without mangling)to the dynamic library's symbol
 * table so that it will be picked up by pkgMkIndex
 */
extern "C"
{
    int Tcltime_Init(Tcl_Interp* theInterpreter) {
       TCL_Interpreter interp(theInterpreter);

       interp.AddCommand("times_install_proc",InstallProc);
       interp.AddCommand("times_enter_proc",EnterProc);
       interp.AddCommand("times_leave_proc",LeaveProc);
       interp.AddCommand("times_print_events",CallStack);
       interp.AddCommand("times_now",Now);
       interp.AddCommand("times_diff",Diff);

       return interp.PackageProvide("Tcltime");
    }
}
