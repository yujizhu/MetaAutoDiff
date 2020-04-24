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

#ifndef METAAUTODIFF_ARRAYDIVNODE_HPP
#define METAAUTODIFF_ARRAYDIVNODE_HPP

#include"../Core/math/ArrayDiv.hpp"
//#include"../Core/ValueNode.hpp"
#include"../Core/SpecialNode.hpp"
#include"../Core/NodeWrapper.hpp"
#include"../Core/math/ValueTrait.hpp"
#include"ArrayMulNode.hpp"
#include<tuple>

namespace MetaAD {

template<typename LeftNodeTypePara, typename RightNodeTypePara>
struct ArrayDivNodeTrait {
    using LeftNodeValueType = typename LeftNodeTypePara::ValueType;
    using RightNodeValueType = typename RightNodeTypePara::ValueType;
    using type = typename ad_math::array_div_trait<LeftNodeValueType, RightNodeValueType>::type;
};

template<typename LeftInputNodeTypePara, typename RightInputNodeTypePara>
struct ArrayDivPolicyDefault {
    using LeftNodeValueType = typename LeftInputNodeTypePara::ValueType;
    using RightNodeValueType = typename RightInputNodeTypePara::ValueType;
    using type = typename ad_math::array_div_policy<LeftNodeValueType, RightNodeValueType>;
};

template<typename LeftInputNodeTypePara, typename RightInputNodeTypePara, unsigned int indexPara,
         typename ValueTypePara = typename ArrayDivNodeTrait<LeftInputNodeTypePara, RightInputNodeTypePara>::type, 
         typename PolicyPara = typename ArrayDivPolicyDefault<LeftInputNodeTypePara, RightInputNodeTypePara>::type,
         bool legalPara = isNodeType<LeftInputNodeTypePara>::value && isNodeType<RightInputNodeTypePara>::value>
class ArrayDivNodeImp;

template<typename LeftInputNodeTypePara, typename RightInputNodeTypePara, unsigned int indexPara, 
         typename ValueTypePara, typename PolicyPara>
class ArrayDivNodeImp<LeftInputNodeTypePara, RightInputNodeTypePara, indexPara, ValueTypePara, PolicyPara, true>
  : public GraphNodeBase<ArrayDivNodeImp<LeftInputNodeTypePara, RightInputNodeTypePara, indexPara, ValueTypePara, PolicyPara,true>> {
  public:
    using Base = GraphNodeBase<ArrayDivNodeImp>;
    using InputNodeTypes = std::tuple<std::shared_ptr<LeftInputNodeTypePara>, std::shared_ptr<RightInputNodeTypePara>>;
    using ValueType = ValueTypePara;
    using Policy = PolicyPara;
    using ConcreteNodeType = ArrayDivNodeImp;
    static constexpr unsigned int index() {
        return indexPara;
    }

    ArrayDivNodeImp(std::shared_ptr<LeftInputNodeTypePara> inputNodeL, std::shared_ptr<RightInputNodeTypePara> inputNodeR)
      : Base(), inputs(InputNodeTypes(inputNodeL, inputNodeR)) {
        output = Policy::compute(inputNodeL->getValue(), inputNodeR->getValue());
    }

    //ArrayDivNodeImp(const ArrayDivNodeImp& rValue) = default;
    //ArrayDivNodeImp(const ArrayDivNodeImp&& lvalue) = default;    //不确定要不要加移动构造函数，这要求所有的成员都具有移动构造函数
    
    // 拷贝赋值运算符
    //ArrayDivNodeImp& operator=(const ArrayDivNodeImp& rValue) = default;

    ValueType getValue() { return output; }   
    ValueType getValue() const { return output; }   

    ValueType compute() const{
        ValueType result = Policy::compute(std::get<0>(inputs)->compute(), std::get<1>(inputs)->compute());
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
    auto derivative() const;

    template<unsigned int n>
    friend struct ChainRuleExpansion;
  private:
    InputNodeTypes inputs;
    ValueType output;
};

template<typename LeftInputNodeTypePara, typename RightInputNodeTypePara, unsigned int indexPara,
         typename ValueTypePara, typename PolicyPara, bool legalPara>
struct traits<ArrayDivNodeImp<LeftInputNodeTypePara, RightInputNodeTypePara, indexPara, ValueTypePara,
                      PolicyPara, legalPara>> {
    using InputNodeTypes = std::tuple<std::shared_ptr<LeftInputNodeTypePara>, std::shared_ptr<RightInputNodeTypePara>>;
    using ValueType = ValueTypePara;
    using Policy = PolicyPara;
    static constexpr unsigned int index = indexPara;
};

template<typename ConcreteLeftNodeTypePara, typename ConcreteRightNodeTypePara,
         unsigned int indexPara = ConcreteLeftNodeTypePara::index() + ConcreteRightNodeTypePara::index()>
auto arrayDivImp(const std::shared_ptr<ConcreteLeftNodeTypePara>& leftNode, const std::shared_ptr<ConcreteRightNodeTypePara>& rightNode) {
    using NodeType = ArrayDivNodeImp<ConcreteLeftNodeTypePara, ConcreteRightNodeTypePara, indexPara>;
    return std::shared_ptr<NodeType>(new NodeType(leftNode, rightNode));
}

template<typename ConcreteLeftNodeTypePara, typename ConcreteRightNodeTypePara>
auto arrayDiv(const NodeWrapper<ConcreteLeftNodeTypePara>& leftNode, 
              const NodeWrapper<ConcreteRightNodeTypePara>& rightNode) {
    auto concreteResultNodePtr = arrayDivImp(leftNode.pNode, rightNode.pNode);
    using ConcreteResultNodeType = typename decltype(concreteResultNodePtr)::element_type;
    return NodeWrapper<ConcreteResultNodeType>(concreteResultNodePtr);
}

template<typename LeftInputNodeTypePara, typename RightInputNodeTypePara, unsigned int indexPara, 
         typename ValueTypePara, typename PolicyPara>
template<unsigned int variableIndex>
auto ArrayDivNodeImp<LeftInputNodeTypePara, RightInputNodeTypePara, indexPara, ValueTypePara, PolicyPara, true>::derivative() const {
    static_assert((variableIndex==0) || (variableIndex==1), "The partial derivative variable index must be 0 or 1.");
    if constexpr (ad_math::is_matrix<typename LeftInputNodeTypePara::ValueType>::value
                        && ad_math::is_matrix<typename RightInputNodeTypePara::ValueType>::value) {
        if constexpr (variableIndex == 0) {
            auto row = (std::get<0>(inputs)->getValue()).rows();
            auto col = (std::get<0>(inputs)->getValue()).cols();
            using OnesMatrixNodeType = ValueNode<NullTypeNode, ad_MatrixXd, internal::unique_index>;
            auto onesMatrixNodePtr = std::shared_ptr<OnesMatrixNodeType>
                                          (new OnesMatrixNodeType(RightInputNodeTypePara::ValueType::Ones(row, col)));
            auto numeratorNodePtr = std::get<1>(inputs);
            auto derivativeNodePtr = arrayDivImp(onesMatrixNodePtr, numeratorNodePtr);
            auto vecDerivativeNodePtr = rowVectorizationImp(derivativeNodePtr);
            auto diagDerivativeNodePtr = matDiagonalImp(vecDerivativeNodePtr);
            return diagDerivativeNodePtr;
        }
        else {
            auto row = (std::get<1>(inputs)->getValue()).rows();
            auto col = (std::get<1>(inputs)->getValue()).cols();
            using negIdentityNodeType = ValueNode<NullTypeNode, ad_MatrixXd, internal::unique_index>;
            auto onesMatrixNodePtr = std::shared_ptr<negIdentityNodeType>
                                          (new negIdentityNodeType(-RightInputNodeTypePara::ValueType::Identity(row, row)));
            auto denominatorNodePtr = std::get<0>(inputs);
            auto numeratorNodePtr = std::get<1>(inputs);
            auto derivativeNodePtr = arrayDivImp(mulImp(onesMatrixNodePtr, denominatorNodePtr), arrayMulImp(numeratorNodePtr, numeratorNodePtr));
            auto vecDerivativeNodePtr = rowVectorizationImp(derivativeNodePtr);
            auto diagDerivativeNodePtr = matDiagonalImp(vecDerivativeNodePtr);
            return diagDerivativeNodePtr;
        }
    }
    
    

    
    
}


}



#endif