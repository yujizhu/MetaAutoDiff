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

#ifndef METAAUTODIFF_UNARYEXPR_HPP
#define METAAUTODIFF_UNARYEXPR_HPP

#include"UnaryExprTrait.hpp"
#include"PartialDerivativeTrait.hpp"
#include"../util/Meta.hpp"

namespace MetaAD {

namespace ad_math {

template<typename InputValueTypePara>
struct unary_expr_policy;

template<>
struct unary_expr_policy<ad_MatrixXd> {
    using OutputValueType = typename unary_expr_trait<ad_MatrixXd>::type;
    using DerivativeValueType = typename ad_math::partial_derivative_trait<ad_MatrixXd, ad_MatrixXd>::type;
    
    template<typename CustomUnaryOp>
    static OutputValueType compute(const ad_MatrixXd& operand, const CustomUnaryOp& func = CustomUnaryOp()) {
        ad_MatrixXd result = operand.unaryExpr(func);
        return result;
    }
    
};

}

}

#endif