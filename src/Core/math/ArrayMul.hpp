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

#ifndef METAAUTODIFF_ARRAYMUL_HPP
#define METAAUTODIFF_ARRAYMUL_HPP

#include"ArrayMulTrait.hpp"
#include"../util/Meta.hpp"
#include"PartialDerivativeTrait.hpp"

namespace MetaAD {

namespace ad_math {

template<typename LeftValueTypePara, typename RightValueTypePara>
struct array_mul_policy;

template<>
struct array_mul_policy<ad_MatrixXd, ad_MatrixXd> {
    using OutputValueType = typename array_mul_trait<ad_MatrixXd, ad_MatrixXd>::type;
    using DerivativeValueType = typename ad_math::partial_derivative_trait<ad_MatrixXd, ad_MatrixXd>::type;
    /*
    template<unsigned int variableIndex> 
    static typename std::enable_if<internal::is_same_value<unsigned int, variableIndex, 0>::value,
                                   DerivativeValueType>::type
    derivative(const ad_MatrixXd& leftOperand, const ad_MatrixXd& rightOperand, const OutputValueType& output) {
        auto rowLength = output.rows();
        auto colLength = output.cols();
        assert((rowLength == leftOperand.rows() && rowLength == rightOperand.rows()
             && colLength == leftOperand.cols() && colLength == rightOperand.cols()));
        
    }
    
    template<unsigned int variableIndex>
    static typename std::enable_if<internal::is_same_value<unsigned int, variableIndex, 1>::value, 
                                   DerivativeValueType>::type 
    derivative(const ad_MatrixXd& leftOperand, const ad_MatrixXd& rightOperand, const OutputValueType& output) {
        return 1; 
    };
    */
  
    static OutputValueType compute(const ad_MatrixXd& leftOperand, const ad_MatrixXd& rightOperand) {
        assert((leftOperand.rows() == rightOperand.rows()
             && leftOperand.cols() == rightOperand.cols()));
        return leftOperand.array() * rightOperand.array();
    }
};

}

}


#endif