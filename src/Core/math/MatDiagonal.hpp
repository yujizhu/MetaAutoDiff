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

#ifndef METAAUTODIFF_MATDIAGONAL_HPP
#define METAAUTODIFF_MATDIAGONAL_HPP

#include"MatDiagonalTrait.hpp"
#include"PartialDerivativeTrait.hpp"
#include"../util/Meta.hpp"

namespace MetaAD {

namespace ad_math {

template<typename InputValueTypePara>
struct mat_diagonal_policy;

template<>
struct mat_diagonal_policy<ad_MatrixXd> {
    using OutputValueType = typename mat_diagonal_trait<ad_MatrixXd>::type;
    using DerivativeValueType = typename ad_math::partial_derivative_trait<ad_MatrixXd, ad_MatrixXd>::type;
    /*
    template<unsigned int variableIndex> 
    static typename std::enable_if<internal::is_same_value<unsigned int, variableIndex, 0>::value,
                                   DerivativeValueType>::type
    derivative(double operand) {
        return std::cos(operand);
    }
    */

    static OutputValueType compute(const ad_MatrixXd operand) {
        // 要求输入矩阵必须是列向量
        assert(operand.cols() == 1);
        ad_MatrixXd result = operand.asDiagonal();
        return result;
    }
};

}

}



#endif