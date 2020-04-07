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

#ifndef METAAUTODIFF_MULTNODE_HPP
#define METAAUTODIFF_MULTNODE_HPP

#include "../Core/GraphNodeBase.hpp"
#include"../Core/math/Mul.hpp"
#include<tuple>

namespace MetaAD {


template<typename LeftNodeTypePara, typename RightNodeTypePara>
struct MulNodeTrait {
    using LeftNodeValueType = typename LeftNodeTypePara::ValueType;
    using RightNodeValueType = typename RightNodeTypePara::ValueType;
    using type = typename ad_math::mul_trait<LeftNodeValueType, RightNodeValueType>::type;
};

template<typename LeftInputNodeTypePara, typename RightInputNodeTypePara>
struct MulDerivativePolicy {
    using LeftNodeValueType = typename LeftInputNodeTypePara::ValueType;
    using RightNodeValueType = typename RightInputNodeTypePara::ValueType;
    using type = typename ad_math::mul_derivative_policy<LeftNodeValueType, RightNodeValueType>;
};

template<typename LeftInputNodeTypePara, typename RightInputNodeTypePara, unsigned int indexPara,
         typename ValueTypePara = typename MulNodeTrait<LeftInputNodeTypePara, RightInputNodeTypePara>::type, 
         typename DerivativePolicyPara = typename MulDerivativePolicy<LeftInputNodeTypePara, RightInputNodeTypePara>::type,
         bool legalPara = isNodeType<LeftInputNodeTypePara>::value && isNodeType<RightInputNodeTypePara>::value>
class MulNode;

template<typename LeftInputNodeTypePara, typename RightInputNodeTypePara, unsigned int indexPara, 
         typename ValueTypePara, typename DerivativePolicyPara>
class MulNode<LeftInputNodeTypePara, RightInputNodeTypePara, indexPara, ValueTypePara, DerivativePolicyPara, true>
  : public GraphNodeBase<MulNode<LeftInputNodeTypePara, RightInputNodeTypePara, indexPara, ValueTypePara, DerivativePolicyPara,true>> {
  public:
    using Base = GraphNodeBase<MulNode>;
    using InputNodeTypes = std::tuple<LeftInputNodeTypePara*, RightInputNodeTypePara*>;
    using ValueType = ValueTypePara;
    using DerivativePolicy = DerivativePolicyPara;
    using ConcreteNodeType = MulNode;
    static constexpr unsigned int index() {
        return indexPara;
    }

    MulNode(LeftInputNodeTypePara* inputNodeL, RightInputNodeTypePara* inputNodeR)
      : Base(), inputs(std::tuple<LeftInputNodeTypePara*, RightInputNodeTypePara*>(inputNodeL, inputNodeR)) {
        output = inputNodeL->getValue() * inputNodeR->getValue();
    }

    MulNode(const MulNode& rValue) = default;
    //MulNode(const MulNode&& lvalue) = default;    //不确定要不要加移动构造函数，这要求所有的成员都具有移动构造函数
    
    // 拷贝赋值运算符，赋值运算符是否要加？如果保证计算图中的所有节点类型都不同的话，就不需要拷贝赋值操作了。
    MulNode& operator=(const MulNode& rValue) = default;

    ValueType getValue() { return output; }   
    ValueType getValue() const { return output; }   
    template<unsigned int variableIndex>
    auto partialDerivative() const {
        static_assert((variableIndex==0) || (variableIndex==1), "The partial derivative variable index must be 0 or 1.");
        auto derivativeValue =  DerivativePolicy::template derivative<variableIndex>(std::get<0>(inputs)->getValue(),
                                                                                     std::get<1>(inputs)->getValue(),
                                                                                     output);
        return derivativeValue;
        /*
        if constexpr (variableIndex == 0)
            return GradientEdge<MulNode, LeftInputNodeTypePara>(this, std::get<0>(inputs), derivativeValue);
        else if constexpr (variableIndex == 1)
            return GradientEdge<MulNode, RightInputNodeTypePara>(this, std::get<1>(inputs), derivativeValue);
        */
    }

    template<unsigned int n>
    friend struct ChainRuleExpansion;
  private:
    InputNodeTypes inputs;
    ValueType output;
};

template<typename LeftInputNodeTypePara, typename RightInputNodeTypePara, unsigned int indexPara,
         typename ValueTypePara, typename DerivativePolicyPara, bool legalPara>
struct traits<MulNode<LeftInputNodeTypePara, RightInputNodeTypePara, indexPara, ValueTypePara,
                      DerivativePolicyPara, legalPara>> {
    using InputNodeTypes = std::tuple<LeftInputNodeTypePara*, RightInputNodeTypePara*>;
    using ValueType = ValueTypePara;
    using DerivativePolicy = DerivativePolicyPara;
    static constexpr unsigned int index = indexPara;
};

template<unsigned int indexPara, typename ConcreteLeftNodeTypePara, typename ConcreteRightNodeTypePara>
auto mul(GraphNodeBase<ConcreteLeftNodeTypePara>& leftNode, 
         GraphNodeBase<ConcreteRightNodeTypePara>& rightNode) {

    // 这里必须使用getPtrFromBase成员来返回指向Derived类型的指针，因为指针类型无法实现从基类到派生类的自动转换，而
    // 该构造函数的形参类型都是指向派生类的指针，因此需要在GraphNodeBase中加上getPtrFromBase成员来返回指向派生类类型的指针
    return MulNode<ConcreteLeftNodeTypePara, ConcreteRightNodeTypePara, indexPara>(leftNode.getDerivedPtr(),
                                                                        rightNode.getDerivedPtr());

}

}

#endif