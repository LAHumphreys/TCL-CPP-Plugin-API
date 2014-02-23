#include "TCLInterpreter.h"

using namespace std;
using namespace TCL_INTERP_UTILS;

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
                                 int (*fptr)(TCL_Interpreter&) ) 
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
            TCL_Interpreter interp(theInterp);
            (*fptr)(interp);
            return TCL_OK;
        });

    return CreateCommand(this,name,f);

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
    return CreateCommand(this,name,f);
}

void TCL_Interpreter::SetResult(std::string result) {
     char * message = Tcl_Alloc(result.length()+1);

     message = strcpy(message,result.c_str());

     Tcl_SetResult(interp,message,TCL_DYNAMIC);
}

void TCL_Interpreter::SetResult(double result) {
     Tcl_SetObjResult(interp,Tcl_NewDoubleObj(result));
}

void TCL_Interpreter::SetResult(int result) {
     Tcl_SetObjResult(interp,Tcl_NewIntObj(result));
}

void TCL_Interpreter::SetResult(long result) {
     Tcl_SetObjResult(interp,Tcl_NewLongObj(result));
}

void TCL_Interpreter::RaiseError(std::string error) {
    throw TCL_Exception({std::move(error)});
}

void TCL_Interpreter::SetError(std::string error) {
    Tcl_AppendResult(interp, error.c_str(), (char *)NULL);
}
