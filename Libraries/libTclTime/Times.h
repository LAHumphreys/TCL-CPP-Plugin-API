#ifndef __TCLTOOLS_TCL_LIB_TIME_TIMER_H__
#define __TCLTOOLS_TCL_LIB_TIME_TIMER_H__
#include <sys/time.h>
#include <forward_list>
#include <unordered_map>
#include <string>
#include <vector>

class Event {
public:
    Event () : Event(-1,' ') {}
    Event(int procId, char code);
    struct timeval tv;
    int    procId;
    char   code;
    std::string Info();
    const static char ENTER = '>';
    const static char LEAVE = '<';
};

class ProcMap {
public:
    static ProcMap& Instance() {
        static ProcMap instance;
        return instance;
    }

    int AddProc(const std::string& name);

    /*
     * Return the name of the proc with id, if it is not known to the map
     * return NO_SUCH_PROC
     */
    const std::string& GetNameFromId(int id);
private:
    ProcMap();
    const static std::string NO_SUCH_PROC;
    int nextId;
    std::unordered_map<std::string,int> nameMap;
    std::unordered_map<int,std::string> idMap;
};

class EventStack { 
public:
    void AddEvent ( int procId, char code ) {
        eventStack.emplace_front(procId, code);
    }

    /*
     * Populate eventList with the event, in the reverse order 
     * they occured.
     * (eventList[0] will contain the last event)
     *
     */
    void GetEvents(std::vector<Event>& eventList);

private:
    /*
     * We want a miminmal and constant impact at insertion time,
     * (to avoid skewing the results), hence the forward_list.
     *
     * This comes with the annoyance of having to manually reverse the
     * sequence with a copy later, but we choose to take the perfomance
     * impact at the point where we no loger care.
     */
    std::forward_list<Event> eventStack;
};

#endif
