#pragma once

#include "../Tools/Int2Type.hpp"
#include "../Tools/OneTwo.hpp"
#include <type_traits>

template<class RetType, class... Params>
class SignatureChecker
{   // compiler's error message humanizer;
    // too much specific thing - usage case see at 'Hierarchy2Params' class;
    typedef Int2Type<false> Begin;

    template<class PassedAsFunctor, class Operator, class NextStep>
    struct Check
    {
        static void doIt(Int2Type<true>) {} // <-----------------------------------------------------+
                                            //                                                       |
        static void doIt(Int2Type<false>)   //                                                       |
        {   //                                                                                       |
            // 'test' is declared only, not defined ever...                                          |
            One test(Operator);                                     //                               |
            Two test(...);                                          //                               |
            // ...and needed for sfinae-detector at the next line;                                   |
            using IsCallOperatorInside = Int2Type<sizeof(test(&PassedAsFunctor::operator()))    //   |
                                                  == sizeof(One)>;                              //   |
                                                    //                                               |
            NextStep::doIt(IsCallOperatorInside()); // compile-time 'if':                            |
                                                    // if - there's an appliable operator inside     |
                                                    // the passed functor - then exit the checking   |
                                                    // and enjoy; ---------------------------------->+
                                                    // else - go to next check (or fail if no case
                                                    // have matched);
        }
    };

    template<class PassedAsFunctor, class DefinedByContextTypesOperator>
    struct FailOnFalse
    {
        static void doIt(Int2Type<true>) {}

        static void doIt(Int2Type<false>)
        {
            struct Inaccesible {};
            static_assert(std::is_same<DefinedByContextTypesOperator, Inaccesible>::value,
                          "\n\n\tSingature of a functor does not match the context types requirements!"
                          "\n\tFor details see the following error message. \n");
            DefinedByContextTypesOperator definedByContextTypesOperator = &PassedAsFunctor::operator();
        }
    };

    public:

    template<class F>
    SignatureChecker(F *f2Check)
    {
        using FGood = RetType(Params...);

        static_assert(std::is_same<FGood, F>::value,
                      "\n\n\tSignature of a function does not match the context types requirements!"
                      "\n\tFor details see the following error message. \n");
        FGood *fGood = f2Check;
    }

    template<class PassedAsFunctor>
    SignatureChecker(PassedAsFunctor)
    {
        // sadly, doesn'n detect errors whithin std::bind-wrapped functions;
        // just whithin simple functors with operator() and lambdas;
        //
        // such a compile-time pipe of 'Check::doIt' overloaded calls that does nothing in runtime,
        // but breaks the build if a signature of passed functor doesn't match the one that required;
        Check<PassedAsFunctor,                      RetType(PassedAsFunctor::*)(Params...),
              Check<PassedAsFunctor,                RetType(PassedAsFunctor::*)(Params...) const,
                    Check<PassedAsFunctor,          RetType(PassedAsFunctor::*)(Params...) volatile,
                          Check<PassedAsFunctor,    RetType(PassedAsFunctor::*)(Params...) const volatile,

                                FailOnFalse<PassedAsFunctor,
                                            RetType(PassedAsFunctor::*)(Params...)
                                           >
                               >
                         >
                   >
             >
             ::doIt(Begin());
    }
};
