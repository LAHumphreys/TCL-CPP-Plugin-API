#ifndef __CALL_FROM_TUPLE__H__
#define __CALL_FROM_TUPLE__H__
/*
 * === Intoduction ===
 *   In order to be able to be able to unpack an unknown number of 
 *   elements from the tuple, we need to some how loop over:
 *       f(std::get<I>(tuple),std::get<I-1>(tuple),...);
 *   
 *   To do this with the unpacking operator we need I to be a pack 
 *   which is the same legnth as the tuple, that way when we can do this:
 *       f(std::get<I>(tuple)...);
 *   I will be unpacked, and each item will be passed as an argument.
 *
 * === Getting the packed sequence numbers ===
 *   We can trivially get the size of the tuple (sizeof...(t)) but this only
 *   gives us a scalar. 
 *   
 *   The trick is to allow the compiler's type deduction to build the integer 
 *   pack for us: 
 *      gen_seq<5>::type;  
 *      // compiler must now build the inheritance tree:
 *      // gen_seq<5> => gen_seq<4,4> => gen_seq<3,3,4> => gen_seq(2,2,3,4) 
 *      //   => gen_seq(1,1,2,3,4) => gen_seq(0,0,1,2,3,4)
 *
 * === Passing the integer pack ====
 *   Although we have now built the integer type its stuck in a template parameter. 
 *   To extract we construct a dummy object, the "Sequence", and pass it to the function
 *   which is interested in the pack. It can then ignore the Sequence, but use it's
 *   type integer pack...
 *    
 * Based on the stack-overflow thread here: 
 *    http://stackoverflow.com/questions/7858817/unpacking-a-tuple-to-call-a-matching-function-pointer
 */
namespace TCL_INTERP_UTILS
{
    /*
     * The (deduced) template parameters of this
     * empty struct will be the integer pack we
     * are going to expand
     */
    template<int ...Idxs>
    struct Sequence  { }; 


    /*
     * The pack builder recursively inherits down to
     * N=0. At each stage we prepend N-1 to the pack, so
     * starting from empty we build up (0...N-1)
     *
     * NOTE: The most dervived class instansiated by
     *       the user will have an empty in pack:
     *         PackBuilder<N>::ThePack
     */
    template<int N, int ...Idxs>
    struct PackBuilder: PackBuilder<N-1,N-1, Idxs...> {};

    /*
     * When we reach 0, stop building the pack and 
     * delcare the typedef we wanted in the first place
     */
    template<int ...Idxs>
    struct PackBuilder<0,Idxs...> {
       typedef Sequence<Idxs...> ThePack;
    };

    /*
     * Unpack a tuple...
     *
     * Use the integer pack hidden in Sequence's type to unpack 
     * the tupple. 
     *
     *  f:      Points to function to invoke
     *  t:      A tupple whose args will be passed to f
     *  args:   Additional args to pass to f
     */
    template<int... Idxs, class F, class ...Args, class ...Tn>
    int UnpackAndCall(Sequence<Idxs...>,F* f, std::tuple<Tn...>& t, Args... args) {
        return (*f)(args...,std::get<Idxs>(t)...);
    }

    /* * Use to call the function pointed to by f with the items
     * of the tuple:
     *    void func ( string&, int&,double&,double&);
     *    ...
     *    std::tuple<string,int> t;
     *    double, arg1, arg2
     *    ...
     *    CallFromTuple(func,t,arg1,arg2);
     *
     * f:    A pointer to the function to call
     * t:    The tuple to unpack
     * args: Additional arguments to tack on to the end of the call..
     */
    template<class F, class ...Args, class ...Tn>
    int CallFromTuple(F* f, std::tuple<Tn...>& t, Args... args) {
        return UnpackAndCall( typename PackBuilder<sizeof...(Tn)>::ThePack(), // Calculate the integer pack
                              f,t,args...);                                   // ... and then forward on the args...
    }
}


#endif
