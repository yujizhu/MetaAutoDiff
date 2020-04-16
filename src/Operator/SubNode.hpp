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

#ifndef METAAUTODIFF_SUBNODE_HPP
#define METAAUTODIFF_SUBNODE_HPP

#include"../Core/math/Sub.hpp"
#include"../Core/ValueNode.hpp"
#include"../Core/NodeWrapper.hpp"
#include<tuple>

namespace MetaAD {

template<typename LeftNodeTypePara, typename RightNodeTypePara>
struct SubNodeTrait {
    using LeftNodeValueType = typename LeftNodeTypePara::ValueType;
    using RightNodeValueType = typename RightNodeTypePara::ValueType;
    using type = typename ad_math::sub_trait<LeftNodeValueType, RightNodeValueType>::type;
};

template<typename LeftInputNodeTypePara, typename RightInputNodeTypePara>
struct SubPolicyDefault {
    using LeftNodeValueType = typename LeftInputNodeTypePara::ValueType;
    using RightNodeValueType = typename RightInputNodeTypePara::ValueType;
    using type = typename ad_math::sub_policy<LeftNodeValueType, RightNodeValueType>;
};

template<typename LeftInputNodeTypePara, typename RightInputNodeTypePara, unsigned int indexPara,
         typename ValueTypePara = typename SubNodeTrait<LeftInputNodeTypePara, RightInputNodeTypePara>::type, 
         typename PolicyPara = typename SubPolicyDefault<LeftInputNodeTypePara, RightInputNodeTypePara>::type,
         bool legalPara = isNodeType<LeftInputNodeTypePara>::value && isNodeType<RightInputNodeTypePara>::value>
class SubNodeImp;

template<typename LeftInputNodeTypePara, typename RightInputNodeTypePara, unsigned int indexPara, 
         typename ValueTypePara, typename PolicyPara>
class SubNodeImp<LeftInputNodeTypePara, RightInputNodeTypePara, indexPara, ValueTypePara, PolicyPara, true>
  : public GraphNodeBase<SubNodeImp<LeftInputNodeTypePara, RightInputNodeTypePara, indexPara, ValueTypePara, PolicyPara,true>> {
  public:
    using Base = GraphNodeBase<SubNodeImp>;
    using InputNodeTypes = std::tuple<std::shared_ptr<LeftInputNodeTypePara>, std::shared_ptr<RightInputNodeTypePara>>;
    using ValueType = ValueTypePara;
    using Policy = PolicyPara;
    using ConcreteNodeType = SubNodeImp;
    static constexpr unsigned int index() {
        return indexPara;
    }

    SubNodeImp() = delete;

    SubNodeImp(std::shared_ptr<LeftInputNodeTypePara> inputNodeL, std::shared_ptr<RightInputNodeTypePara> inputNodeR)
      : Base(), inputs(InputNodeTypes(inputNodeL, inputNodeR)) {
        output = inputNodeL->getValue() - inputNodeR->getValue();
    }

    //SubNodeImp(const SubNodeImp& rValue) = default;
    //SubNodeImp(const SubNodeImp&& lvalue) = default;    //不确定要不要加移动构造函数，这要求所有的成员都具有移动构造函数
    
    // 拷贝赋值运算符
    // SubNodeImp& operator=(const SubNodeImp& rValue) = default;

    ValueType getValue() { return output; }   
    ValueType getValue() const { return output; }   

    ValueType compute() const {
        ValueType result = std::get<0>(inputs)->compute() - std::get<1>(inputs)->compute();
        return result;
    }

    template<unsigned int variableIndex>
    auto partialDerivative() const {
        static_assert((variableIndex==0) || (variableIndex==1), "The partial derivative variable index must be 0 or 1.");
        auto derivativeValue =  Policy::template derivative<variableIndex>(std::get<0>(inputs)->getValue(),
                                                                                     std::get<1>(inputs)->getValue(),
                                                                                     output);
        return derivativeValue;
        /*
        if constexpr (variableIndex == 0)
            return GradientEdge<SubNodeImp, LeftInputNodeTypePara>(this, std::get<0>(inputs), derivativeValue);
        else if constexpr (variableIndex == 1)
            return GradientEdge<SubNodeImp, RightInputNodeTypePara>(this, std::get<1>(inputs), derivativeValue);
        */	
    }
    
    // Another implementation of derivation for first order derivation.
    template<unsigned int variableIndex>
    auto derivative() const {
        static_assert((variableIndex==0) || (variableIndex==1), "The partial derivative variable index must be 0 or 1.");
        if constexpr (ad_math::is_scalar<typename LeftInputNodeTypePara::ValueType>::value 
                   && ad_math::is_scalar<typename RightInputNodeTypePara::ValueType>::value) {
            if constexpr (variableIndex == 0) {
                using NodeType = ValueNode<NullTypeNode, typename LeftInputNodeTypePara::ValueType, internal::unique_index>;
                return std::shared_ptr<NodeType>(new NodeType(1));
            }
            else {
                using NodeType = ValueNode<NullTypeNode, typename RightInputNodeTypePara::ValueType, internal::unique_index>;
                return std::shared_ptr<NodeType>(new NodeType(-1));
            }
        }
        else if constexpr (ad_math::is_matrix<typename LeftInputNodeTypePara::ValueType>::value
                        && ad_math::is_matrix<typename RightInputNodeTypePara::ValueType>::value) {
            if constexpr (variableIndex == 0) {
                auto row = (std::get<0>(inputs)->getValue()).rows();
                auto col = (std::get<0>(inputs)->getValue()).cols();
                using DerivativeNodeType = ValueNode<NullTypeNode, typename LeftInputNodeTypePara::ValueType, internal::unique_index>;
                auto derivativeNodePtr = std::shared_ptr<DerivativeNodeType>(new DerivativeNodeType(LeftInputNodeTypePara::ValueType::Identity(row*col, row*col)));
                return derivativeNodePtr;
            }
            else {
                auto row = (std::get<1>(inputs)->getValue()).rows();
                auto col = (std::get<1>(inputs)->getValue()).cols();
                using DerivativeNodeType = ValueNode<NullTypeNode, typename RightInputNodeTypePara::ValueType, internal::unique_index>;
                auto derivativeNodePtr = std::shared_ptr<DerivativeNodeType>(new DerivativeNodeType(-RightInputNodeTypePara::ValueType::Identity(row*col, row*col)));
                return derivativeNodePtr;
            }
        }
    }

    template<unsigned int n>
    friend struct ChainRuleExpansion;

  private:
    InputNodeTypes inputs;
    ValueType output;
};

template<typename LeftInputNodeTypePara, typename RightInputNodeTypePara, unsigned int indexPara,
         typename ValueTypePara, typename PolicyPara, bool legalPara>
struct traits<SubNodeImp<LeftInputNodeTypePara, RightInputNodeTypePara, indexPara, ValueTypePara,
                      PolicyPara, legalPara>> {
    using InputNodeTypes = std::tuple<std::shared_ptr<LeftInputNodeTypePara>, std::shared_ptr<RightInputNodeTypePara>>;
    using ValueType = ValueTypePara;
    using Policy = PolicyPara;
    static constexpr unsigned int index = indexPara;
};


template<typename ConcreteLeftNodeTypePara, typename ConcreteRightNodeTypePara,
         unsigned int indexPara = ConcreteLeftNodeTypePara::index() + ConcreteRightNodeTypePara::index()>
typename std::enable_if<isNodeType<ConcreteLeftNodeTypePara>::value && isNodeType<ConcreteRightNodeTypePara>::value
                   && (!isBaseNodeType<ConcreteLeftNodeTypePara>::value) && (!isBaseNodeType<ConcreteRightNodeTypePara>::value),
                       std::shared_ptr<SubNodeImp<ConcreteLeftNodeTypePara, ConcreteRightNodeTypePara, indexPara>>
                       >::type
subImp(const std::shared_ptr<ConcreteLeftNodeTypePara>& leftNode, const std::shared_ptr<ConcreteRightNodeTypePara>& rightNode) {
    static_assert(isNodeType<ConcreteLeftNodeTypePara>::value && isNodeType<ConcreteRightNodeTypePara>::value
             && (!isBaseNodeType<ConcreteLeftNodeTypePara>::value) && (!isBaseNodeType<ConcreteRightNodeTypePara>::value),
                 "The Inputs is not node type or input node types are NodeBaseType which isn't allowed.");
    using NodeType = SubNodeImp<ConcreteLeftNodeTypePara, ConcreteRightNodeTypePara, indexPara>;
    return std::shared_ptr<NodeType>(new NodeType(leftNode, rightNode));
}

template<typename ConcreteLeftNodeTypePara, typename ConcreteRightNodeTypePara>
auto operator-(const NodeWrapper<ConcreteLeftNodeTypePara>& leftNode,
               const NodeWrapper<ConcreteRightNodeTypePara>& rightNode) {
    auto concreteResultNodePtr = subImp(leftNode.pNode, rightNode.pNode);
    using ConcreteResultNodeType = typename decltype(concreteResultNodePtr)::element_type;
    return NodeWrapper<ConcreteResultNodeType>(concreteResultNodePtr);
}

}

#endif