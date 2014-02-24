#include "tcl.h"
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <type_traits>
#include <tuple>

// Function signature of the standard function call...
using TCL_CMD_FUNC = std::function<int(ClientData,Tcl_Interp* p_interp,int objc,Tcl_Obj* CONST objv[])>;

namespace TCL_INTERP_UTILS {
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

    /*
     * Register a new procedure with TCL. Also register the call back to FreeCommand
     * to clean up afterwards...
     */
    static int CreateCommand(TCL_Interpreter* THIS, std::string name, TCL_CMD_FUNC* f) {
        char * cmdName = const_cast<char *>(name.c_str());
        ClientData proc_toinvoke = reinterpret_cast<ClientData>(f);
    
        Tcl_CreateObjCommand(THIS->Interp(), cmdName, CmdFunction, proc_toinvoke, FreeCommand);
    
        return TCL_OK;
    }

    /*
     * Type conversions we support...
     */
    int GetNativeType(TCL_Interpreter* THIS, Tcl_Obj* source, double& native);

    int GetNativeType(TCL_Interpreter* THIS, Tcl_Obj* source, int& native);

    int GetNativeType(TCL_Interpreter* THIS, Tcl_Obj* source, long& native);

    int GetNativeType(TCL_Interpreter* THIS, Tcl_Obj* source, std::string& native);
}

#include "TCL_foreach_in_tuple.h"
#include "TCL_call_from_tuple.h"

template <class ...T>
int TCL_Interpreter::AddCommand( std::string name,
                                 int (*fptr)(TCL_Interpreter&, T...) ) 
{

    /*
     * Create a wrapper function on the heap (its life is dynamic, and will be freed
     * by a call to FreeCommand made by the tcl garbage collector)
     */
    TCL_CMD_FUNC* f = new TCL_CMD_FUNC([fptr] ( ClientData  clientData,
                      Tcl_Interp* theInterp,
                      int         objc,
                      Tcl_Obj* CONST objv[]) -> int 
        {
            int ret = TCL_OK;
            TCL_Interpreter interp(theInterp);

            std::tuple<T...> args;

            // Check we have the right number of args...
            if ( objc != (sizeof...(T)+1) ) {
                Tcl_WrongNumArgs(theInterp, objc,objv,NULL);
                ret = TCL_ERROR;
            }

            // Initialise the args...
            if ( ret == TCL_OK ) {
                ret = TCL_INTERP_UTILS::PopulateTuple(&interp, args, objv);
            }

            if ( ret == TCL_OK ) {
                try {
                    ret = TCL_INTERP_UTILS::CallFromTuple(fptr,args,interp);
                } catch ( TCL_Exception& error ) {
                    interp.SetError(error.msg);
                    ret = TCL_ERROR;
                }
            }
            return ret;
        });

    return TCL_INTERP_UTILS::CreateCommand(this,name,f);
}
