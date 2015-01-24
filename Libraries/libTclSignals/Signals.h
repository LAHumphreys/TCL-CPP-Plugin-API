#ifndef __TCLTOOLS_TCL_LIB_SINGAL_SINGALR_H__
#define __TCLTOOLS_TCL_LIB_SINGAL_SINGALR_H__

#include <string>
#include <map>
#include <signal.h>
#include <functional>
#include <string>
#include "TCLInterpreter.h"

class SigAction;

class KnownSignals {
public:
	static KnownSignals& Instance();

	const std::string& SigName(int signo);

	int SigNo(const std::string& signame);

	static const char* UNKNOWN_SIGNAL;
	static const int UNKNOWN_SIGNAL_NO = -1;
private:
	using NameMap = std::map<std::string,int>;
	using ConstNameIterator = NameMap::const_iterator;

	using NoMap = std::map<int,std::string>;
	using ConstNoIterator = NoMap::const_iterator;

    NoMap   signos;
    NameMap signames;

	KnownSignals();
};

class Signal {
public:
	/**
	 * Initialise the signal object from its name.
	 *
	 * If the signal is not recognised it will be set to UNKNOWN_SIGNAL. Subsequent
	 * member calls are undefined to all except the SignalNo function;
	 *
	 * @param signal  The name of the signal to find.
	 */
	Signal(const std::string& signal);


	/**
	 * @returns The signal number represented
	 */
	const int& SignalNo() const;

	/**
	 * @returns A string representation of the signal number (e.g SIGINT)
	 */
	std::string SignalName() const;

	/**
	 * @returns A user-friendly descriptor of the signal
	 */
	std::string SignalDescription() const;
private:
	int signalNo;
};

class SigAction: public sigaction {
public:
	/**
	 * Construct the handler to call f_ptr when the signal is received;
	 */
	SigAction(TCL_Interpreter& interp, const std::string& script);

	/**
	 * Signal interrupts are handled immediately - including if the process is
	 * currently waiting on the result of a system call.
	 *
	 * The interrupt will cause the call to fail. By default these will fail when
	 * execution resumes. By enabling the restarting behaviour the system will
	 * attempt to restart these calls.
	 *
	 * This only works on certain signals, see man sigaction for more details.
	 */
	void EnableSysCallRestarts();
	void DisableSysCallRestarts();
	bool SysCallRestartEnabled();

	/**
	 * Install a handler for this signal
	 *
	 * @param signal   The
	 *
	 * @returns true on success, false otherwise
	 */
	bool InstallHandler(const Signal& signal, TCL_Interpreter& interp);

	const std::string& Script() const { return script; }
private:
	const std::string script;
};


#endif
