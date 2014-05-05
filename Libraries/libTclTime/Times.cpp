#include "Times.h"
#include <algorithm>
#include <iterator>
#include <sstream>
#include <util_time.h>

const std::string ProcMap::NO_SUCH_PROC = "";

Event::Event(int _procId, char _code) 
    : procId(_procId), code(_code)
{
    /*
     * This isn't as slow as you think it is.
     *
     * It doesn't make a sys-call (see strace), and my 
     * 2011 laptop can make 50million calls to gettimeofday in
     * 1.2s (approx 25 nano seconds / call)
     */
    gettimeofday(&tv,NULL);
}

std::string Event::Info() {
    std::stringstream info;
    info << "\"" << Time(tv).Timestamp() << "\"";
    if ( code == '>' ) {
        info << ",\"ENTER\"";
    } else if ( code == '<') {
        info << ",\"LEAVE\"";
    } else {
        info << ",\"UNKNOWN\"";
    }
    info << ",\"" << ProcMap::Instance().GetNameFromId(procId) << "\"";
    info << std::endl;

    return info.str();
}

ProcMap::ProcMap() : nextId(0) {}

int ProcMap::AddProc(const std::string& name) {
    nameMap[name] = nextId;
    idMap[nextId] = name;
    return nextId++;
}

const std::string& ProcMap::GetNameFromId(int id) {
    auto it = idMap.find(id);
    if ( it == idMap.end() ) {
        return NO_SUCH_PROC;
    } else {
        return it->second;
    }
}

void EventStack::GetEvents(std::vector<Event>& eventList) {
    // ( see note on performance in the header)
    eventList.resize(std::distance(eventStack.begin(), 
                                    eventStack.end()));
    copy(eventStack.begin(),eventStack.end(),eventList.begin());
}
