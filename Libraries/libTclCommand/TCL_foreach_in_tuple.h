#ifndef __FOREACH_IN_TUPLE_H__
#define __FOREACH_IN_TUPLE_H__

#include <tuple>
#include <string>
#include <iostream>

namespace TCL_INTERP_UTILS {

    /*
     * A trivial loop over every item in a tuple...
     */
    template<int N, class...Tn> 
    class TupleForEach {
    public:
        static int Loop(TCL_Interpreter* THIS, std::tuple<Tn...>& data,Tcl_Obj* CONST objs[]) {
            int ret = TupleForEach<N-1,Tn...>::Loop(THIS,data,objs);
            if ( ret == TCL_OK ) {
                ret = GetNativeType(THIS,objs[N+1],std::get<N>(data));
            }
            return ret;
        }
    };
    
    template<class...Tn> 
    class TupleForEach<0,Tn...> {
    public:
        static int Loop(TCL_Interpreter* THIS, std::tuple<Tn...>& data,Tcl_Obj* CONST objs[]) {
            return GetNativeType(THIS, objs[1],std::get<0>(data));
        }
    };

    template<class...Tn> 
    int PopulateTuple(TCL_Interpreter* THIS, std::tuple<Tn...>& data,Tcl_Obj* CONST objv[]) {
        return TupleForEach<sizeof...(Tn)-1,Tn...>::Loop(THIS,data,objv);
    }
}


#endif
