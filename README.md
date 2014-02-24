TCL-Readline  [![Build Status](https://travis-ci.org/Grauniad/TCL-Readline.png?branch=master)](https://travis-ci.org/Grauniad/TCL-Readline)  [![Coverage Status](https://coveralls.io/repos/Grauniad/TCL-Readline/badge.png?branch=master)](https://coveralls.io/r/Grauniad/TCL-Readline?branch=master)
============

Typesafe C++ Extensions
--------------
A small, type-safe, API is provided for writing c++ extensions is provided. Using the TCL_Interpreter object standard c++ functions can be registered with TCL, and the library will take care of argument counting and type validation before the function is invoked: 


```c++
static int Repeat(TCL_Interpreter& interp, string phrase, int times) {
    stringstream result;

    for ( int i =0; i< times; i++ ) {
        result << phrase << endl;
    }

    interp.SetResult(result.str());
    return TCL_OK;
}

extern "C"
{
    int Tcltest_Init(Tcl_Interp* theInterpreter) {
       TCL_Interpreter interp(theInterpreter);
       interp.AddCommand("Repeat",Repeat);
    }
}
```

```TCL
>>> package require Tcltest
>>> puts [Repeat "Hello World!" 4]
Hello World!
Hello World!
Hello World!
Hello World!
```

Readline Integration
---------------------
An example plugin which integrates with the readline library will be provided...
