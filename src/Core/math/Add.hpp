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
#include"MatrixWrapper.hpp"

namespace MetaAD {

namespace ad_math {
    template<typename LeftValueTypePara, typename RightValueTypePara>
    struct add_derivative_policy;

    template<>
    struct add_derivative_policy<double, double> {//这么做模板偏特化不知道对不对，特别是这里省略了默认模板实参
        using OutputValueType = typename add_trait<double, double>::type;
        template<unsigned int variableIndex> 
        static typename std::enable_if<internal::is_same_value<unsigned int, variableIndex, 0>::value
                                    || internal::is_same_value<unsigned int, variableIndex, 1>::value, 
                                       OutputValueType>::type   //这里虽然逻辑上没有错误，但是与正常习惯不同，这里的偏导数类型应该与对哪个输入变量进行偏导有关，因此应该使用partial_derivative_trait，但是由于在这个特化情况下，两个偏导的类型相同，因此为方便，这样编写代码
        derivative(double input1, double input2, OutputValueType output) {
            return 1;
        }
        /*
        template<unsigned int variableIndex>
        static typename std::enable_if<internal::is_same_value<unsigned int, variableIndex, 1>::value, 
                                       OutputValueType>::type 
        _derivative(T1 input1,T2 input2, T output) {
            return 1; 
        };
        */
    };


    template<>
        struct add_derivative_policy<ad_MatrixXd, ad_MatrixXd> {//这么做模板偏特化不知道对不对，特别是这里省略了默认模板实参
        using OutputValueType = typename add_trait<ad_MatrixXd, ad_MatrixXd>::type;
        template<unsigned int variableIndex> 
        static typename std::enable_if<internal::is_same_value<unsigned int, variableIndex, 0>::value
                                    || internal::is_same_value<unsigned int, variableIndex, 1>::value,  
                                       OutputValueType>::type //这里虽然逻辑上没有错误，但是与正常习惯不同，这里的偏导数类型应该与对哪个输入变量进行偏导有关，因此应该使用partial_derivative_trait，但是由于在这个特化情况下，两个偏导的类型相同，因此为方便，这样编写代码
        derivative(const ad_MatrixXd& inputL, const ad_MatrixXd& inputR, const OutputValueType& output) {
            // 矩阵加法必然满足输入矩阵，输出矩阵的维数（行、列）相同。
            auto rowLength = output.rows();
            auto colLength = output.cols();
            assert((rowLength == inputL.rows() && rowLength == inputR.rows()
             && colLength == inputL.cols() && colLength == inputR.cols()));
            return ad_MatrixXd::Identity(rowLength*colLength, rowLength*colLength);
        }
        
        /* // 由于Matrix加法要求两个输入矩阵维数相同，且它对第一个输入和对第二个输入的偏导数相同，因此只实现一个
        // 下面的偏导数分支也可以用if constexpr来实现
        template<unsigned int variableIndex>
        static typename std::enable_if<internal::is_same_value<unsigned int, variableIndex, 1>::value,
                                       OutputValueType>::type 
        _derivative(T1 input1,T2 input2, T output) {
        	auto rowLength = output.rows();
        	auto colLength = output.cols();
            return ad_MatrixXd::Identity(return Matrix::Identity(rowLength*colLength, rowLength*colLength);); 
        };
        */
    };
}

}

#endif




