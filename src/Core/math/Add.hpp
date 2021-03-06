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



#ifndef METAAUTODIFF_ADD_HPP
#define METAAUTODIFF_ADD_HPP

#include"../util/Meta.hpp"
#include"PartialDerivativeTrait.hpp"
#include"AddTrait.hpp"

namespace MetaAD {

namespace ad_math {

template<typename LeftValueTypePara, typename RightValueTypePara>
struct add_policy;

template<>
struct add_policy<double, double> {
    using OutputValueType = typename add_trait<double, double>::type;
    using DerivativeValueType = typename ad_math::partial_derivative_trait<double, double>::type;
    template<unsigned int variableIndex> 
    static typename std::enable_if<internal::is_same_value<unsigned int, variableIndex, 0>::value
                                || internal::is_same_value<unsigned int, variableIndex, 1>::value, 
                                   DerivativeValueType>::type
    derivative(double leftOperand, double rightOperand, OutputValueType output) {
        return 1;
    }
    /*
    template<unsigned int variableIndex>
    static typename std::enable_if<internal::is_same_value<unsigned int, variableIndex, 1>::value, 
                                   DerivativeValueType>::type 
    derivative(double leftOperand, double rightOperand, OutputValueType output) {
        return 1; 
    };
    */

    static OutputValueType compute(double leftOperand, double rightOperand) {
        return leftOperand + rightOperand;
    }
};


template<>
struct add_policy<ad_MatrixXd, ad_MatrixXd> {
    using OutputValueType = typename add_trait<ad_MatrixXd, ad_MatrixXd>::type;
    using DerivativeValueType = typename ad_math::partial_derivative_trait<ad_MatrixXd, ad_MatrixXd>::type;
    template<unsigned int variableIndex> 
    static typename std::enable_if<internal::is_same_value<unsigned int, variableIndex, 0>::value
                                || internal::is_same_value<unsigned int, variableIndex, 1>::value,  
                                   DerivativeValueType>::type
    derivative(const ad_MatrixXd& leftOperand, const ad_MatrixXd& rightOperand, const OutputValueType& output) {
        // 矩阵加法必然满足输入矩阵，输出矩阵的维数（行、列）相同。
        auto rowLength = output.rows();
        auto colLength = output.cols();
        assert((rowLength == leftOperand.rows() && rowLength == rightOperand.rows()
             && colLength == leftOperand.cols() && colLength == rightOperand.cols()));
        return ad_MatrixXd::Identity(rowLength*colLength, rowLength*colLength);
    }
        
    /* // 由于Matrix加法要求两个输入矩阵维数相同，且它对第一个输入和对第二个输入的偏导数相同，因此只实现一个
    // 下面的偏导数分支也可以用if constexpr来实现
        template<unsigned int variableIndex>
    static typename std::enable_if<internal::is_same_value<unsigned int, variableIndex, 1>::value,
                                   OutputValueType>::type 
    derivative(const ad_MatrixXd& leftOperand, const ad_MatrixXd& rightOperand, const OutputValueType& output) {
    	auto rowLength = output.rows();
     	auto colLength = output.cols();
        return ad_MatrixXd::Identity(rowLength*colLength, rowLength*colLength); 
    };
    */
    static OutputValueType compute(const ad_MatrixXd& leftOperand, const ad_MatrixXd& rightOperand) {
        assert((leftOperand.rows() == rightOperand.rows() && leftOperand.cols() == rightOperand.cols()));
        return leftOperand + rightOperand;
    }
};

}

}

#endif




