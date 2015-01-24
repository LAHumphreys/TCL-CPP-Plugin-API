#include "tcl.h"
#include <Signals.h>

#include <TCLInterpreter.h>
#include <sstream>
#include <util_time.h>
#include <functional>

using namespace std;

static void HelloWorld(int) {
	cout << "Hello World!" << endl;
	return;
}
static int InstallSignalHandler(TCL_Interpreter& interp, string name, string script) {
	Signal signal(name);
	SigAction handle(interp, script);
	handle.InstallHandler(signal,interp);
	interp.SetResult(signal.SignalNo());
	return interp.Eval("puts \"Installed command: " + name + "\"");
}

/*
 * Expose this (without mangling)to the dynamic library's symbol
 * table so that it will be picked up by pkgMkIndex
 */
extern "C"
{
    int Tclsignals_Init(Tcl_Interp* theInterpreter) {
       TCL_Interpreter interp(theInterpreter);

       interp.AddCommand("SigAction",InstallSignalHandler);

       return interp.PackageProvide("Tclsignals");
    }
}
