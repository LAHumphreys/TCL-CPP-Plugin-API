A TCL extension API for C++11   [![Build Status](https://travis-ci.org/Grauniad/TCL-Readline.png?branch=master)](https://travis-ci.org/Grauniad/TCL-Readline)  [![Coverage Status](https://coveralls.io/repos/Grauniad/TCL-Readline/badge.png?branch=master)](https://coveralls.io/r/Grauniad/TCL-Readline?branch=master)
============

Typesafe C++ Extensions
--------------
A small, type-safe, API is provided for writing c++ extensions is provided. 

Before invoking a function, the API will:
 * Check all arguments have been provided
 * Construct the arguments into c++ types (string, int, long and double supported by default)
 * Raise a standard error message if an incorrect type has been provided

Also provided by the API: 
 * Return results by providing the native type 
 * Throw standard TCL errors within 
 * Simple to add more supported types 

Simple Example
-----------------

```c++
int Repeat(TCL_Interpreter& interp, string phrase, int times) {
    stringstream result;

    for ( int i =0; i< times; i++ ) {
        result << phrase << endl;
    }

    interp.SetResult(result.str());
    return TCL_OK;
}

extern "C"
{
    // Standard TCL hook for pkg_mkIndex
    int Tcltest_Init(Tcl_Interp* theInterpreter) {
       TCL_Interpreter interp(theInterpreter);
       
       // Register the function with the API
       interp.AddCommand("Repeat",Repeat);
       
       // Declare the package we're providing
       return interp.PackageProvide("Tcltest");
    }
}
```

```TCL
>>> package require Tcltest
>>> Repeat "Hello World!" 4
```
```
Hello World!
Hello World!
Hello World!
Hello World!
```


Argument Checking
-----------------
Standard TCL errors are raised for missing or incorrect arguments
```TCL
>>> Repeat "Hello World!" three
```
```
expected integer but got "three"
    while executing
"Repeat "Hello World!" three"
    (file "./pluginTest.tcl" line 177)
```


Raising custom errors
-----------------
Lets amend the command with some boundrary case testing

```c++
int Repeat(TCL_Interpreter& interp, string phrase, int times) {
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
```
```TCL
>>> Repeat "Hello World!" -1
```
```
repeat must be a positive number!
    while executing
"Repeat "Hello World" -1"
    (file "./pluginTest.tcl" line 177)
```

Readline Integration
---------------------
An example plugin which integrates with the readline library will be provided...
