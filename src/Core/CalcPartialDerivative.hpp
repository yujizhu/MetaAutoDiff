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

#ifndef METAAUTODIFF_CALCPARTIALDERIVATIVE_HPP
#define METAAUTODIFF_CALCPARTIALDERIVATIVE_HPP

#include<tuple>
#include"util/Meta.hpp"
#include"GraphNodeBase.hpp"
#include"GradientEdge.hpp"
#include"math/SpecialType.hpp"
#include"math/PartialDerivativeTrait.hpp"

#include"../Operator/AddNode.hpp"
#include"../Operator/MulNode.hpp"
#include"SpecialNode.hpp"


namespace MetaAD {

// 判断两个节点之间是否具有依赖关系，这部分要求计算图中的任意两个节点的类型都要不同，本框架通过构造节点对象时的index参数来显式使每个节点的类型不同
// 前置声明
template<typename HeadNodeTypePara, typename TailNodeTypePara, unsigned int n>
struct has_dependence_imp;

template<typename HeadNodeTypePara, typename TailNodeTypePara, 
         bool legal = isNodeType<HeadNodeTypePara>::value && isNodeType<TailNodeTypePara>::value>
struct has_dependence {
    static constexpr bool value = false;
};

template<typename HeadNodeTypePara, typename TaliNodeTypePara>
struct has_dependence<HeadNodeTypePara, TaliNodeTypePara, true> {
    static constexpr bool value = has_dependence_imp<HeadNodeTypePara, TaliNodeTypePara,
                                  std::tuple_size<typename HeadNodeTypePara::InputNodeTypes>::value - 1>::value;
};

// 这个偏特化不知道要不要加,这个感觉不用加，因为在计算偏导数过程中，只会计算两个不同类型节点间的偏导数，因此不会产生这个调用
template<typename NodeTypePara>
struct has_dependence<NodeTypePara, NodeTypePara, true> { // 这里的true如果改成legal则会产生ambiguous,因为偏序规则可能无法判断
    static constexpr bool value = true;
};

template<typename TailNodeTypePara>
struct has_dependence<NullTypeNode, TailNodeTypePara, true> {
    static constexpr bool value = false;
};

template<typename HeadNodeTypePara>
struct has_dependence<HeadNodeTypePara, NullTypeNode, true> {
    static constexpr bool value = false;
};

template<>
struct has_dependence<NullTypeNode, NullTypeNode,true> {
    static constexpr bool value = false;
};

template<typename HeadNodeTypePara, typename TailNodeTypePara, unsigned int n>
struct has_dependence_imp {
    //static constexpr bool value = std::is_same<typename std::tuple_element<n, typename HeadNodeTypePara::InputNodeTypes>, TailNodeTypePara>::value
    //                           || std::has_dependence<typename std::tuple_element<n, typename HeadNodeTypePara::InputNodeTypes>, TailNodeType>::value
    //                           || has_dependence_imp<HeadNodeType, TailNodeTypePara, n-1>::value;
    // 使用了定义于internal中的logic_or，实现了静态逻辑短路，其具体调用方法与||运算符略有不同，使用时请注意.
    /* //下面的实现可行，但是代码有重复
    static constexpr bool value = internal::logic_or<internal::logic_or<std::is_same<typename internal::remove_pointer<typename std::tuple_element<n, typename HeadNodeTypePara::InputNodeTypes>>::type, TailNodeTypePara>::value, // 由于has_dependence定义了两个节点类型相同的偏特化，因此这一行和下一行的逻辑好像重复了
                                                                        has_dependence<typename internal::remove_pointer<typename std::tuple_element<n, typename HeadNodeTypePara::InputNodeTypes>::type>::type, TailNodeTypePara>
                                                                           >::value, 
                                                     has_dependence_imp<HeadNodeTypePara, TailNodeTypePara, n-1>
                                                    >::value;
    */
    // 为了新的接口，这里改了！！！！！！！！老接口不能用了
    static constexpr bool value = internal::logic_or<has_dependence<typename std::tuple_element<n, typename HeadNodeTypePara::InputNodeTypes>::type::element_type, TailNodeTypePara>::value,						   
                                                     has_dependence_imp<HeadNodeTypePara, TailNodeTypePara, n-1>
                                                    >::value;
};

template<typename HeadNodeTypePara, typename TailNodeTypePara>
struct has_dependence_imp<HeadNodeTypePara, TailNodeTypePara, 0> {// 下面这个is_same好像可以不用加，因为前面有两个节点类型相同时的特化
    static constexpr bool value = internal::logic_or<std::is_same<typename std::tuple_element<0, typename HeadNodeTypePara::InputNodeTypes>, std::shared_ptr<TailNodeTypePara>>::value,
                                                     has_dependence<typename std::tuple_element<0, typename HeadNodeTypePara::InputNodeTypes>::type::element_type, TailNodeTypePara>
                                                    >::value;

};

template<unsigned int n>
struct ChainRuleExpansion;

//// For same node type
template<typename NodeTypePara>
auto
_calcPartialDerivative(const std::shared_ptr<NodeTypePara>& dependentVariable, const std::shared_ptr<NodeTypePara>& independentVariable) {
    //std::cout << "in 0" << std::endl;
    using DerivativeNodeType = internal::IdentityNode<internal::unique_index, typename NodeTypePara::ValueType>;
    return std::shared_ptr<DerivativeNodeType>(new DerivativeNodeType(dependentVariable->getValue()));
}

// For other situation
// 这个函数一定会将derivative节点计算出来，DerivativeLookup交给wrapper函数调用
template<typename DependentVariablePara, typename IndependentVariablePara>
auto _calcPartialDerivative(const std::shared_ptr<DependentVariablePara>& dependentVariable, const std::shared_ptr<IndependentVariablePara>& independentVariable) {
    static_assert((!isBaseNodeType<DependentVariablePara>::value) && (!isBaseNodeType<IndependentVariablePara>::value), "Node type couldn't be NodeBaseType!");
    if constexpr (has_dependence<DependentVariablePara, IndependentVariablePara>::value) {
        auto result = ChainRuleExpansion<std::tuple_size<typename DependentVariablePara::InputNodeTypes>::value - 1
                                             >::expansion(dependentVariable, independentVariable);
        return result;
    }
    else {
        // 因为链式法则，所以ZeroNode应该以dependentVariable的值为种子产生
        using DerivativeNodeType = internal::ZeroNode<internal::unique_index, typename DependentVariablePara::ValueType>;
        return std::shared_ptr<DerivativeNodeType>(new DerivativeNodeType(dependentVariable->getValue()));
    }
}

template<unsigned int n>
struct ChainRuleExpansion {
    template<typename DependentVariablePara, typename IndependentVariablePara>
    static auto
    expansion(std::shared_ptr<DependentVariablePara> dependentVariable,
              std::shared_ptr<IndependentVariablePara> independentVariable) {
        static_assert((n > 0) || (n == 0), "Invalid variable index!");
        if constexpr (n > 0) {
            /*
            if constexpr (std::is_same<DependentVariablePara, IndependentVariablePara>::value) {
                using DerivativeNodeType = internal::IdentityNode<internal::unique_index, typename NodeTypePara::ValueType>;
                return std::shared_ptr<DerivativeNodeType>(new DerivativeNodeType(dependentVariable->getValue()));
            }
            else {
                using InputNodeType = typename std::tuple_element<n, typename DependentVariablePara::InputNodeTypes>::type::element_type;
                if constexpr (has_dependence<InputNodeType, IndependentVariablePara>::value) {
                    auto temp1 = dependentVariable -> template derivative<n>();
                    auto temp2 = ChainRuleExpansion<std::tuple_size<typename InputNodeType::InputNodeTypes>::value>::expansion(std::get<n>(dependentVariable->inputs), independentVariable);
                    auto temp3 = mulImp(temp1, temp2);
                    auto temp4 = ChainRuleExpansion<n-1>::expansion(dependentVariable, independentVariable);
                    auto result = addImp(temp3, temp4);
                    return result;
                }
                else {
                    auto result = ChainRuleExpansion<n-1>::expansion(dependentVariable, independentVariable);
                    return result;
                }

            }
            */
           auto temp1 = dependentVariable -> template derivative<n>();
           auto temp2 = _calcPartialDerivative(std::get<n>(dependentVariable->inputs), independentVariable);
           auto temp3 = mulImp(temp1, temp2);
           auto temp4 = ChainRuleExpansion<n-1>::expansion(dependentVariable, independentVariable);
           auto result = addImp(temp3, temp4);
           return result;
        }
        else {
            /*
             if constexpr (std::is_same<DependentVariablePara, IndependentVariablePara>::value) {
                using DerivativeNodeType = internal::IdentityNode<internal::unique_index, typename NodeTypePara::ValueType>;
                return std::shared_ptr<DerivativeNodeType>(new DerivativeNodeType(dependentVariable->getValue()));
            }
            else {
                using InputNodeType = typename std::tuple_element<n, typename DependentVariablePara::InputNodeTypes>::type::element_type;
                if constexpr (has_dependence<InputNodeType, IndependentVariablePara>::value) {
                    auto temp1 = dependentVariable -> template derivative<n>();
                    auto temp2 = ChainRuleExpansion<std::tuple_size<typename InputNodeType::InputNodeTypes>::value>::expansion(std::get<n>(dependentVariable->inputs), independentVariable);
                    auto temp3 = mulImp(temp1, temp2);
                    auto temp4 = ChainRuleExpansion<n-1>::expansion(dependentVariable, independentVariable);
                    auto result = addImp(temp3, temp4);
                    return result;
                }
                else {
                    auto result = ChainRuleExpansion<n-1>::expansion(dependentVariable, independentVariable);
                    return result;
                }

            }
            */
            auto temp1 = dependentVariable -> template derivative<0>();
            auto temp2 = _calcPartialDerivative(std::get<0>(dependentVariable->inputs), independentVariable);
            auto result = mulImp(temp1, temp2);
            return result;
        }
    }
};

template<unsigned int order, typename ConcreteLeftNodeTypePara, typename ConcreteRightNodeTypePara>
auto calcPartialDerivative(const NodeWrapper<ConcreteLeftNodeTypePara>& leftNode, const NodeWrapper<ConcreteRightNodeTypePara>& rightNode) {
    auto derivativeNodePtr = _calcPartialDerivative(leftNode.pNode, rightNode.pNode);
    using DerivativeNodeType = typename decltype(derivativeNodePtr)::element_type;
    return NodeWrapper<DerivativeNodeType>(derivativeNodePtr);
}

}

#endif