#include "tcl.h"

#include <TCLInterpreter.h>
#include <sstream>

using namespace std;

static int Repeat(TCL_Interpreter& interp, string phrase, int times) {
    stringstream result;

    if ( times < 0 ) {
        interp.RaiseError("repeat must be a positive number!");
    }

    for ( int i =0; i< times; i++ ) {
        result << phrase << endl;
    }

    interp.SetResult(result.str());
    return TCL_OK;
}

//TODO: unsigned...

static int HalfIt(TCL_Interpreter& interp, double num) {
    if ( num == 0.0 ) {
        interp.RaiseError("HalfIt: can't half 0!");
    } else {
        interp.SetResult(num/2.0);
    }
    return TCL_OK;
}

static int DoubleIt(TCL_Interpreter& interp, int num) {
    if ( num == 0 ) {
        interp.RaiseError("DoubleIt: can't double 0!");
    } else {
        interp.SetResult(num*2);
    }
    return TCL_OK;
}

static int SquareIt(TCL_Interpreter& interp, long num) {
    if ( num == 0 ) {
        interp.RaiseError("SquareIt: can't square 0!");
    } else {
        interp.SetResult(num*num);
    }
    return TCL_OK;
}

static int SayHi(TCL_Interpreter& interp, std::string name) {
    if ( name == "-" ) {
        interp.RaiseError("SayHi: You need to give me a name!");
    } else {
        interp.SetResult("Hi, " + name);
    }
    return TCL_OK;
}

static int HelloWorld(TCL_Interpreter& interp) {
    interp.SetResult("Hello World!");
    return TCL_OK;
}

/*
 * Expose this (without mangling)to the dynamic library's symbol
 * table so that it will be picked up by pkgMkIndex
 */
extern "C"
{
    int Tcltest_Init(Tcl_Interp* theInterpreter) {
       TCL_Interpreter interp(theInterpreter);

       std::function<int(TCL_Interpreter& interp)> func(HelloWorld);

       interp.AddCommand("helloWorldCommand",func);
       interp.AddCommand("helloWorldCommand_ptr",HelloWorld);
       interp.AddCommand("HalfIt",HalfIt);
       interp.AddCommand("DoubleIt",DoubleIt);
       interp.AddCommand("SquareIt",SquareIt);
       interp.AddCommand("SayHi",SayHi);
       interp.AddCommand("Repeat",Repeat);

       return interp.PackageProvide("Tcltest");
    }
}
