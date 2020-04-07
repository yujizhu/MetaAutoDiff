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



#ifndef METAAUTODIFF_SUB_HPP
#define METAAUTODIFF_SUB_HPP

#include"../util/Meta.hpp"
#include"PartialDerivativeTrait.hpp"
#include"SubTrait.hpp"

namespace MetaAD {

namespace ad_math {
    template<typename LeftValueTypePara, typename RightValueTypePara>
    struct sub_derivative_policy;

    template<>
    struct sub_derivative_policy<double, double> {
        using OutputValueType = typename sub_trait<double, double>::type;
        using DerivativeValueType = typename partial_derivative_trait<double, double>::type;
        template<unsigned int variableIndex> 
        static typename std::enable_if<internal::is_same_value<unsigned int, variableIndex, 0>::value,
                                       DerivativeValueType>::type
        derivative(double leftOperand, double rightOperand, OutputValueType output) {
            return 1;
        }
        
        template<unsigned int variableIndex>
        static typename std::enable_if<internal::is_same_value<unsigned int, variableIndex, 1>::value, 
                                       DerivativeValueType>::type 
        derivative(double leftOperand, double rightOperand, OutputValueType output) {
            return -1; 
        };
    };


    template<>
        struct sub_derivative_policy<ad_MatrixXd, ad_MatrixXd> {
        using OutputValueType = typename sub_trait<ad_MatrixXd, ad_MatrixXd>::type;
        using DerivativeValueType = typename partial_derivative_trait<ad_MatrixXd, ad_MatrixXd>::type;
        template<unsigned int variableIndex> 
        static typename std::enable_if<internal::is_same_value<unsigned int, variableIndex, 0>::value, 
                                       DerivativeValueType>::type
        derivative(const ad_MatrixXd& leftOperand, const ad_MatrixXd& rightOperand, const OutputValueType& output) {
            // 矩阵减法必然满足输入矩阵，输出矩阵的维数（行、列）相同。
            auto rowLength = output.rows();
            auto colLength = output.cols();
            assert((rowLength == leftOperand.rows() && rowLength == rightOperand.rows()
                 && colLength == leftOperand.cols() && colLength == rightOperand.cols()));
            return ad_MatrixXd::Identity(rowLength*colLength, rowLength*colLength);
        }
        
        // 下面的偏导数分支也可以用if constexpr来实现，但要求编译器支持C++17
        template<unsigned int variableIndex>
        static typename std::enable_if<internal::is_same_value<unsigned int, variableIndex, 1>::value,
                                       DerivativeValueType>::type 
        derivative(const ad_MatrixXd& leftOperand, const ad_MatrixXd& rightOperand, const OutputValueType& output) {
        	auto rowLength = output.rows();
        	auto colLength = output.cols();
            assert((rowLength == leftOperand.rows() && rowLength == rightOperand.rows()
                 && colLength == leftOperand.cols() && colLength == rightOperand.cols()));
            DerivativeValueType result = -ad_MatrixXd::Identity(rowLength*colLength, rowLength*colLength); 
            return result; 
        };
    };
}

}

#endif




