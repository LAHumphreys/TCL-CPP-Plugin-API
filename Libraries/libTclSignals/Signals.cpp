#include "Signals.h"
#include <algorithm>
#include <iterator>
#include <sstream>
#include <util_time.h>
#include <signal.h>
#include <logger.h>
#include <errno.h>

const char* KnownSignals::UNKNOWN_SIGNAL = "UNKNOWN";

KnownSignals::KnownSignals() {
    signames = { 
        { "SIGHUP", SIGHUP},
        { "SIGINT", SIGINT},
        { "SIGQUIT", SIGQUIT},
        { "SIGILL", SIGILL},
        { "SIGTRAP", SIGTRAP},
        { "SIGABRT", SIGABRT},
        { "SIGIOT", SIGIOT},
        { "SIGBUS", SIGBUS},
        { "SIGFPE", SIGFPE},
        { "SIGKILL", SIGKILL},
        { "SIGUSR1", SIGUSR1},
        { "SIGSEGV", SIGSEGV},
        { "SIGUSR2", SIGUSR2},
        { "SIGPIPE", SIGPIPE},
        { "SIGALRM", SIGALRM},
        { "SIGTERM", SIGTERM},
        { "SIGSTKFLT", SIGSTKFLT},
        { "SIGCHLD", SIGCHLD},
        { "SIGCONT", SIGCONT},
        { "SIGSTOP", SIGSTOP},
        { "SIGTSTP", SIGTSTP},
        { "SIGTTIN", SIGTTIN},
        { "SIGTTOU", SIGTTOU},
        { "SIGURG", SIGURG},
        { "SIGXCPU", SIGXCPU},
        { "SIGXFSZ", SIGXFSZ},
        { "SIGVTALRM", SIGVTALRM},
        { "SIGPROF", SIGPROF},
        { "SIGWINCH", SIGWINCH},
        { "SIGIO", SIGIO},
        { "SIGPWR", SIGPWR},
        { "SIGSYS", SIGSYS},
        { "SIGUNUSED", SIGSYS}
    };

    for ( const NameMap::value_type& sig: signames) {
    	if ( sig.first != "SIGIOT" && sig.first!= "SIGUNUSED")
    	signos[sig.second] = sig.first;
    }
}

KnownSignals& KnownSignals::Instance() {
	static KnownSignals instance;
	return instance;
}

const std::string& KnownSignals::SigName(int signo) {
	ConstNoIterator it = signos.find(signo);

	static const std::string unknownSig(UNKNOWN_SIGNAL);

	if ( it != signos.end()) {
		return it->second;
	} else {
		return unknownSig;
	}
}

class InstalledSignals {
public:
	static InstalledSignals& Instance() {
		static InstalledSignals instance;
		return instance;
	}

	struct Action {
		std::string script;
		Tcl_Interp* interp;
	};

	bool InstallScript(SigAction& action, const Signal& signal, TCL_Interpreter& interp)
	{
		scripts[signal.SignalNo()] = {action.Script(),interp.Interp()};
		struct sigaction* act = &action;
        if( sigaction(signal.SignalNo(),act,NULL) == 0) {
            return true;
        } else {
            SLOG_FROM(LOG_ERROR,"Signal::InstallHandler",
                    "Failed to install handler: " << strerror(errno));
            return false;
        }
	}

	void ExecuteScript(int sig) {
		std::map<int,Action>::iterator it = scripts.find(sig);
		if ( it != scripts.end()) {
			Action& action = it->second;
            TCL_Interpreter interp(action.interp);
			interp.Eval(action.script);
		}
	}
private:
	InstalledSignals() { }

	std::map<int,Action> scripts;
};


int KnownSignals::SigNo(const std::string& signame) {
	ConstNameIterator it = signames.find(signame);

	if ( it != signames.end()) {
		return it->second;
	} else {
		return UNKNOWN_SIGNAL_NO;
	}
}

static void Dummy(int sig) {
	InstalledSignals::Instance().ExecuteScript(sig);
}

SigAction::SigAction(TCL_Interpreter& interp, const std::string& _script)
   : sigaction({
	    &Dummy,
	    SA_NODEFER,
	    0
     }),
     script(_script)
{
	EnableSysCallRestarts();
}

bool SigAction::SysCallRestartEnabled() {
	return SA_RESTART & sa_flags;
}

void SigAction::EnableSysCallRestarts() {
	sa_flags |= SA_RESTART;
}

void SigAction::DisableSysCallRestarts() {
	sa_flags &= ~SA_RESTART;
}


bool SigAction::InstallHandler(const Signal& sig, TCL_Interpreter& interp) {
	return InstalledSignals::Instance().InstallScript(*this,sig,interp);
}

Signal::Signal(const std::string& signal) {
	signalNo = KnownSignals::Instance().SigNo(signal);
}

const int& Signal::SignalNo() const {
	return signalNo;
}

std::string Signal::SignalName() const {
	return KnownSignals::Instance().SigName(signalNo);
}

std::string Signal::SignalDescription() const {
	return strsignal(signalNo);
}
