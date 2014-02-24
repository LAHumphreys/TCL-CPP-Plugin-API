#ifndef __tcl_tools_LIBRARIES_LIBTCLCOMMAND_TCL_INTERPRETER_H__
#define __tcl_tools_LIBRARIES_LIBTCLCOMMAND_TCL_INTERPRETER_H__
#include <string>
#include <functional>


struct Tcl_Interp; 

struct TCL_Exception {
    std::string msg;
};

class TCL_Interpreter {
public: 
    TCL_Interpreter(Tcl_Interp* interpreter);

    int PackageProvide(std::string name, std::string version="1.0");

    /*
     * Except a function taking sensible types and create a TCL 
     * command from it...
     */
    template<class ...T>
    int AddCommand( std::string name, int (*fptr)(TCL_Interpreter&, T... args ) );

    int AddCommand( std::string name, int (*fptr)(TCL_Interpreter&) );
    int AddCommand(std::string name,std::function<int(TCL_Interpreter&)>& proc);

    void SetResult(std::string result);
    void SetResult(double result);
    void SetResult(int result);
    void SetResult(long result);

    /*
     * Raise an error, called by a tcl command to immediately 
     * exit and return an error.
     */
    void RaiseError(std::string error);

    /*
     * Register an error message, but then return to normal flow.
     *
     * It is the responsibility of the calling command to return 
     * TCL_ERROR in order for the error to be registered by TCL
     */
    void SetError( std::string error);

    Tcl_Interp* Interp() { return interp; }

private:
    Tcl_Interp* interp;

};

#include "TCLInterpreter.hpp"


#endif
