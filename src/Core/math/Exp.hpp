/* Copyright (c) 2020 Yu Jizhu. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */


#ifndef METAAUTODIFF_EXP_HPP
#define METAAUTODIFF_EXP_HPP

#include"ExpTrait.hpp"
#include"PartialDerivativeTrait.hpp"
#include"../util/Meta.hpp"

namespace MetaAD {

namespace ad_math {

template<typename InputValueTypePara>
    struct exp_policy;

    template<>
    struct exp_policy<double> {
        using OutputValueType = typename exp_trait<double>::type;
        using DerivativeValueType = typename ad_math::partial_derivative_trait<double, double>::type;
        template<unsigned int variableIndex> 
        static typename std::enable_if<internal::is_same_value<unsigned int, variableIndex, 0>::value,
                                       DerivativeValueType>::type
        derivative(double operand) {
            return std::exp(operand);
        }

        static OutputValueType compute(double operand) {
            return std::exp(operand);
        }
    };

}

}

#endif