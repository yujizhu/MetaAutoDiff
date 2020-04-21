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

#ifndef METAAUTODIFF_VECTORIZATION_HPP
#define METAAUTODIFF_VECTORIZATION_HPP

#include"VectorizationTrait.hpp"
#include"PartialDerivativeTrait.hpp"
#include"../util/Meta.hpp"

namespace MetaAD {

namespace ad_math {

template<typename InputValueTypePara>
struct col_vectorization_policy;

template<>
struct col_vectorization_policy<ad_MatrixXd> {
    using OutputValueType = typename col_vectorization_trait<ad_MatrixXd>::type;
    using DerivativeValueType = typename ad_math::partial_derivative_trait<ad_MatrixXd, ad_MatrixXd>::type;
    /*
    template<unsigned int variableIndex> 
    static typename std::enable_if<internal::is_same_value<unsigned int, variableIndex, 0>::value,
                                   DerivativeValueType>::type
    derivative(double operand) {
        return std::cos(operand);
    }
    */

    static OutputValueType compute(const ad_MatrixXd& operand) {
        ad_MatrixXd result = operand;
        result.resize(operand.rows()*operand.cols(), 1);
        return result;
    }
};

template<typename InputValueTypePara>
struct row_vectorization_policy;

template<>
struct row_vectorization_policy<ad_MatrixXd> {
    using OutputValueType = typename row_vectorization_trait<ad_MatrixXd>::type;
    using DerivativeValueType = typename ad_math::partial_derivative_trait<ad_MatrixXd, ad_MatrixXd>::type;
    /*
    template<unsigned int variableIndex> 
    static typename std::enable_if<internal::is_same_value<unsigned int, variableIndex, 0>::value,
                                   DerivativeValueType>::type
    derivative(double operand) {
        return std::cos(operand);
    }
    */

    static OutputValueType compute(const ad_MatrixXd& operand) {
        ad_MatrixXd result = operand.transpose();
        result.resize(1, operand.rows()*operand.cols());
        return result;
    }
};


}

}

#endif