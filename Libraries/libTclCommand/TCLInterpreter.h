#ifndef __tcl_tools_LIBRARIES_LIBTCLCOMMAND_TCL_INTERPRETER_H__
#define __tcl_tools_LIBRARIES_LIBTCLCOMMAND_TCL_INTERPRETER_H__
#include <string>
#include <functional>


struct Tcl_Interp; 

class TCL_Interpreter {
public: 
    TCL_Interpreter(Tcl_Interp* interpreter);

    int PackageProvide(std::string name, std::string version="1.0");

    int AddCommand( std::string name, int (*fptr)(TCL_Interpreter&) );
    int AddCommand(std::string name,std::function<int(TCL_Interpreter&)>& proc);

    void SetResult(std::string result);

    Tcl_Interp* Interp() { return interp; }

private:
    Tcl_Interp* interp;

};


#endif
