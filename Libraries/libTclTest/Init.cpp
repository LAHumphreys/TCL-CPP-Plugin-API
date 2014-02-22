#include "tcl.h"

#include <TCLInterpreter.h>

using namespace std;


static int HelloWorld(TCL_Interpreter& interp) {
    interp.SetResult("Hello World!");
    return TCL_OK;
}

/*
 * Expose this to the dynamic library's symbol
 * table so that it will be picked up by pkgMkIndex
 */
extern "C"
{
    int Tcltest_Init(Tcl_Interp* theInterpreter) {
       TCL_Interpreter interp(theInterpreter);

       std::function<int(TCL_Interpreter& interp)> func(HelloWorld);

       interp.AddCommand("helloWorldCommand",func);

       return interp.PackageProvide("Tcltest");
    }
}
