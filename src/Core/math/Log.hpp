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

#ifndef METAAUTODIFF_LOG_HPP
#define METAAUTODIFF_LOG_HPP

#include"LogTrait.hpp"
#include"PartialDerivativeTrait.hpp"
#include"../util/Meta.hpp"

namespace MetaAD {

namespace ad_math {

template<typename InputValueTypePara>
struct log_policy;

template<>
struct log_policy<double> {
    using OutputValueType = typename log_trait<double>::type;
    using DerivativeValueType = typename ad_math::partial_derivative_trait<double, double>::type;
    template<unsigned int variableIndex> 
    static typename std::enable_if<internal::is_same_value<unsigned int, variableIndex, 0>::value,
                                   DerivativeValueType>::type
    derivative(double operand) {
        return 1/operand;
    }

    static OutputValueType compute(double operand) {
        return std::log(operand);
    }
};

template<>
struct log_policy<ad_MatrixXd> {
    using OutputValueType = typename log_trait<ad_MatrixXd>::type;
    using DerivativeValueType = typename ad_math::partial_derivative_trait<ad_MatrixXd, ad_MatrixXd>::type;
    /*
    template<unsigned int variableIndex> 
    static typename std::enable_if<internal::is_same_value<unsigned int, variableIndex, 0>::value,
                                   DerivativeValueType>::type
    derivative(const ad_MatrixXd& leftOperand, const ad_MatrixXd& rightOperand, const OutputValueType& output) {
        // 矩阵加法必然满足输入矩阵，输出矩阵的维数（行、列）相同。
        auto rowLength = output.rows();
        auto colLength = output.cols();
        assert((rowLength == leftOperand.rows() && rowLength == rightOperand.rows()
             && colLength == leftOperand.cols() && colLength == rightOperand.cols()));
        return ad_MatrixXd::Identity(rowLength*colLength, rowLength*colLength);
    }
    */

    static OutputValueType compute(const ad_MatrixXd& operand) {
        ad_MatrixXd result = operand.array().log();
        return result;
    }
};

}

}

#endif