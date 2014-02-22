#include "TCLInterpreter.h"
#include "tcl.h"
#include <cstring>
#include <cstdlib>
#include <cstdio>

using namespace std;

// Function signature of the standard function call...
using TCL_CMD_FUNC = std::function<int(ClientData,Tcl_Interp* p_interp,int objc,Tcl_Obj* CONST objv[])>;

namespace {
    /*
     * Free a tcl command created on the heap. This will be called by 
     * tcl's garbage collector when a command is destroyed
     *
     * f_todelete:   The std::function object which was to be invoked
     *               when the command was called
     */
    static void FreeCommand( ClientData f_todelete) {
        TCL_CMD_FUNC* f = reinterpret_cast<TCL_CMD_FUNC*>(f_todelete);
        delete f;
    }

    /*
     * We need a function which can be called by the c code using
     * a standard function pointer. This sole purpose of this function 
     * is to forward the arguemnts to dynamically created function...
     *
     */
    static int CmdFunction ( ClientData proc,
                             Tcl_Interp* p_interp,
			     int objc,
			     Tcl_Obj* CONST objv[])
    {
        TCL_CMD_FUNC* f = reinterpret_cast<TCL_CMD_FUNC*>(proc);
	return (*f)(proc,p_interp,objc,objv);
    }
}

TCL_Interpreter::TCL_Interpreter(Tcl_Interp* interpreter)
    : interp(interpreter) 
{
}
     
int TCL_Interpreter::PackageProvide(string name, string version) {
     // Because some miss-guided C programmers think they are too cool for const-safety...
     char * packageName = const_cast<char *>(name.c_str());
     char * packageVersion = const_cast<char *>(version.c_str());

     return Tcl_PkgProvide(this->interp, packageName, packageVersion);
}

int TCL_Interpreter::AddCommand( string name,
                                 std::function<int(TCL_Interpreter&)>& proc) 
{
    /*
     * Create a wrapper function on the heap (its life is dynamic, and will be freed
     * by a call to FreeCommand made by the tcl garbage collector)
     *
     * NOTE: We're taking a copy of the proc object here (including all of its code / captures etc).
     */
    TCL_CMD_FUNC* f = new TCL_CMD_FUNC([proc] ( ClientData  clientData,
                      Tcl_Interp* theInterp,
                      int         objc,
                      Tcl_Obj* CONST objv[]) -> int 
        {
            TCL_Interpreter interp(theInterp);
            proc(interp);
            return TCL_OK;
        });
    // Because some miss-guided C programmers think they are too cool for const-safety...
    char * cmdName = const_cast<char *>(name.c_str());
    ClientData proc_toinvoke = reinterpret_cast<ClientData>(f);

    Tcl_CreateObjCommand(this->interp, cmdName, CmdFunction, proc_toinvoke, FreeCommand);

    return TCL_OK;
}

void TCL_Interpreter::SetResult(std::string result) {
     char * message = Tcl_Alloc(result.length()+1);

     message = strcpy(message,result.c_str());

     Tcl_SetResult(interp,message,TCL_DYNAMIC);
}
