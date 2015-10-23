#pragma once
/*
    Use free. Be free. Be lock-free!!

    Copyright (C) 2014, 2015  Konstantin U. Zozoulia

    candid.71 -at- mail -dot- ru

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "../Tools/Int2Type.hpp"
#include "../Tools/OneTwo.hpp"
#include <type_traits>

namespace TricksAndThings {

template<class Ret, class... Params>
class SignatureChecker
{   // compiler's error message humanizer;
    // too much specific thing - usage case see at 'Hierarchy2Params' class;
    typedef Int2Type<false> Begin;

    template<class PassedAsFunctor, class Operator, class NextStep>
    struct Check
    {   // see (*)
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
        static void doIt(Int2Type<true>) {} // ...and do not fail on true;

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

    template<class PassedAsFunctionPtr>
    SignatureChecker(PassedAsFunctionPtr *f2Check)
    {
        using FGood = Ret(Params...);

        static_assert(std::is_same<FGood, PassedAsFunctionPtr>::value,
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
        typedef PassedAsFunctor F;  // shortcut;
        // (*)
        // such a compile-time pipe of 'Check::doIt' overloaded calls that does nothing in runtime,
        // but breaks the build if a signature of passed functor doesn't match the one that required;
        //
        // purpose: to discard 'cv', 'lvalue ref' and 'rvalue ref' qualifiers from
        //          F::operator()(Params...) signature - to check it's return type
        //          and it's argument types only;
        Check<F, Ret(F::*)(Params...),
              Check<F, Ret(F::*)(Params...) const,
                    Check<F, Ret(F::*)(Params...) volatile,
                          Check<F, Ret(F::*)(Params...) const volatile,
                                Check<F, Ret(F::*)(Params...) &,
                                      Check<F, Ret(F::*)(Params...) const &,
                                            Check<F, Ret(F::*)(Params...) volatile &,
                                                  Check<F, Ret(F::*)(Params...) const volatile &,
                                                        Check<F, Ret(F::*)(Params...) &&,
                                                              Check<F, Ret(F::*)(Params...) const &&,
                                                                    Check<F, Ret(F::*)(Params...) volatile &&,
                                                                          Check<F, Ret(F::*)(Params...) const volatile &&,

                                                                                FailOnFalse<PassedAsFunctor,
                                                                                            Ret(F::*)(Params...)
                                                                                           >
                                                                               >
                                                                         >
                                                                   >
                                                             >
                                                       >
                                                 >
                                           >
                                     >
                               >
                         >
                   >
             >
             ::doIt(Begin());
    }
};

}
