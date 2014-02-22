#include "tcl.h"

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>

using namespace std;


static int helloWorldCommand( ClientData clientData, 
                              Tcl_Interp *interp, 
			      int objc,
                              Tcl_Obj *CONST objv[])
{
     char * message = Tcl_Alloc(15);
     message = strcpy(message,"Hello World!");

     Tcl_SetResult(interp,message,TCL_DYNAMIC);
     return TCL_OK;
}


/*
 * Expose this to the dynamic library's symbol
 * table so that it will be picked up by pkgMkIndex
 */
extern "C"
{
    int Tcltest_Init(Tcl_Interp* theInterpreter) {
       int * clientData = NULL;
       Tcl_CmdDeleteProc* deleteProc =  NULL;

       // Because some miss-guided C programmers think they are too cool for const-safety...
       char * cmdName = const_cast<char *>("helloWorldCommand");
       char * packageName = const_cast<char *>("Tcltest");
       char * packageVersion = const_cast<char *>("1.0");

       Tcl_CreateObjCommand(theInterpreter, cmdName, helloWorldCommand, clientData,deleteProc);

       return Tcl_PkgProvide(theInterpreter, packageName, packageVersion);
    }
}
