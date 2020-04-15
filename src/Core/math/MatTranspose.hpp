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

#ifndef METAAUTODIFF_MATTRANSPOSE_HPP
#define METAAUTODIFF_MATTRANSPOSE_HPP

#include"../util/Meta.hpp"
#include"PartialDerivativeTrait.hpp"
#include"MatTransposeTrait.hpp"

namespace MetaAD {

namespace ad_math {
    template<typename InputValueTypePara>
    struct mat_transpose_policy;

    template<>
    struct mat_transpose_policy<ad_MatrixXd> {
        using OutputValueType = typename mat_transpose_trait<ad_MatrixXd>::type;
        using DerivativeValueType = typename ad_math::partial_derivative_trait<ad_MatrixXd, ad_MatrixXd>::type;
        
        static OutputValueType compute(ad_MatrixXd operand) {
            OutputValueType result = ad_matTranspose(operand);
            return result;
        }
    };

}

}


#endif