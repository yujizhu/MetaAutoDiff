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

#ifndef METAAUTODIFF_ARRAYMULNODE_HPP
#define METAAUTODIFF_ARRAYMULNODE_HPP

#include"../Core/math/ArrayMul.hpp"
//#include"../Core/ValueNode.hpp"
#include"../Core/SpecialNode.hpp"
#include"../Core/NodeWrapper.hpp"
#include"../Core/math/ValueTrait.hpp"
#include"VectorizationNode.hpp"
#include"MatDiagonalNode.hpp"
#include<tuple>

namespace MetaAD {

template<typename LeftNodeTypePara, typename RightNodeTypePara>
struct ArrayMulNodeTrait {
    using LeftNodeValueType = typename LeftNodeTypePara::ValueType;
    using RightNodeValueType = typename RightNodeTypePara::ValueType;
    using type = typename ad_math::array_mul_trait<LeftNodeValueType, RightNodeValueType>::type;
};

template<typename LeftInputNodeTypePara, typename RightInputNodeTypePara>
struct ArrayMulPolicyDefault {
    using LeftNodeValueType = typename LeftInputNodeTypePara::ValueType;
    using RightNodeValueType = typename RightInputNodeTypePara::ValueType;
    using type = typename ad_math::array_mul_policy<LeftNodeValueType, RightNodeValueType>;
};

template<typename LeftInputNodeTypePara, typename RightInputNodeTypePara, unsigned int indexPara,
         typename ValueTypePara = typename ArrayMulNodeTrait<LeftInputNodeTypePara, RightInputNodeTypePara>::type, 
         typename PolicyPara = typename ArrayMulPolicyDefault<LeftInputNodeTypePara, RightInputNodeTypePara>::type,
         bool legalPara = isNodeType<LeftInputNodeTypePara>::value && isNodeType<RightInputNodeTypePara>::value>
class ArrayMulNodeImp;

template<typename LeftInputNodeTypePara, typename RightInputNodeTypePara, unsigned int indexPara, 
         typename ValueTypePara, typename PolicyPara>
class ArrayMulNodeImp<LeftInputNodeTypePara, RightInputNodeTypePara, indexPara, ValueTypePara, PolicyPara, true>
  : public GraphNodeBase<ArrayMulNodeImp<LeftInputNodeTypePara, RightInputNodeTypePara, indexPara, ValueTypePara, PolicyPara,true>> {
  public:
    using Base = GraphNodeBase<ArrayMulNodeImp>;
    using InputNodeTypes = std::tuple<std::shared_ptr<LeftInputNodeTypePara>, std::shared_ptr<RightInputNodeTypePara>>;
    using ValueType = ValueTypePara;
    using Policy = PolicyPara;
    using ConcreteNodeType = ArrayMulNodeImp;
    static constexpr unsigned int index() {
        return indexPara;
    }

    ArrayMulNodeImp(std::shared_ptr<LeftInputNodeTypePara> inputNodeL, std::shared_ptr<RightInputNodeTypePara> inputNodeR)
      : Base(), inputs(InputNodeTypes(inputNodeL, inputNodeR)) {
        output = Policy::compute(inputNodeL->getValue(), inputNodeR->getValue());
    }

    //ArrayMulNodeImp(const ArrayMulNodeImp& rValue) = default;
    //ArrayMulNodeImp(const ArrayMulNodeImp&& lvalue) = default;    //不确定要不要加移动构造函数，这要求所有的成员都具有移动构造函数
    
    // 拷贝赋值运算符
    //ArrayMulNodeImp& operator=(const ArrayMulNodeImp& rValue) = default;

    ValueType getValue() { return output; }   
    ValueType getValue() const { return output; }   

    ValueType compute() const{
        ValueType result = std::get<0>(inputs)->compute() + std::get<1>(inputs)->compute();
        return result;
    }
    /*
    template<unsigned int variableIndex>
    auto partialDerivative() const {
        static_assert((variableIndex==0) || (variableIndex==1), "The partial derivative variable index must be 0 or 1.");
        auto derivativeValue =  Policy::template derivative<variableIndex>(std::get<0>(inputs)->getValue(),
                                                                                     std::get<1>(inputs)->getValue(),
                                                                                     output);
        return derivativeValue;
    }
    */

    // Another implementation of derivation for first order derivation.
    template<unsigned int variableIndex>
    auto derivative() const {
        static_assert((variableIndex==0) || (variableIndex==1), "The partial derivative variable index must be 0 or 1.");
        if constexpr (ad_math::is_matrix<typename LeftInputNodeTypePara::ValueType>::value
                        && ad_math::is_matrix<typename RightInputNodeTypePara::ValueType>::value) {
            if constexpr (variableIndex == 0) {
                auto vecDerivativeNodePtr = rowVectorizationImp(std::get<1>(inputs));
                auto diagDerivativeNodePtr = matDiagonalImp(vecDerivativeNodePtr);
                return diagDerivativeNodePtr;
            }
            else {
                auto vecDerivativeNodePtr = rowVectorizationImp(std::get<0>(inputs));
                auto diagDerivativeNodePtr = matDiagonalImp(vecDerivativeNodePtr);
                return diagDerivativeNodePtr;
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
struct traits<ArrayMulNodeImp<LeftInputNodeTypePara, RightInputNodeTypePara, indexPara, ValueTypePara, PolicyPara, legalPara>> {
    using InputNodeTypes = std::tuple<std::shared_ptr<LeftInputNodeTypePara>, std::shared_ptr<RightInputNodeTypePara>>;
    using ValueType = ValueTypePara;
    using Policy = PolicyPara;
    static constexpr unsigned int index = indexPara;
};

template<typename ConcreteLeftNodeTypePara, typename ConcreteRightNodeTypePara,
         unsigned int indexPara = ConcreteLeftNodeTypePara::index() + ConcreteRightNodeTypePara::index()>
auto arrayMulImp(const std::shared_ptr<ConcreteLeftNodeTypePara>& leftNode, const std::shared_ptr<ConcreteRightNodeTypePara>& rightNode) {
    using NodeType = ArrayMulNodeImp<ConcreteLeftNodeTypePara, ConcreteRightNodeTypePara, indexPara>;
    return std::shared_ptr<NodeType>(new NodeType(leftNode, rightNode));
}

template<typename ConcreteLeftNodeTypePara, typename ConcreteRightNodeTypePara>
auto arrayMul(const NodeWrapper<ConcreteLeftNodeTypePara>& leftNode, 
              const NodeWrapper<ConcreteRightNodeTypePara>& rightNode) {
    auto concreteResultNodePtr = arrayMulImp(leftNode.pNode, rightNode.pNode);
    using ConcreteResultNodeType = typename decltype(concreteResultNodePtr)::element_type;
    return NodeWrapper<ConcreteResultNodeType>(concreteResultNodePtr);
}



}


#endif