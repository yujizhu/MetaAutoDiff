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

#ifndef METAAUTODIFF_VALUENODE_HPP
#define METAAUTODIFF_VALUENODE_HPP

#include"GraphNodeBase.hpp"
#include"math/ValueTrait.hpp"
#include<memory>
#include<Tuple>

namespace MetaAD {


template<typename InputNodeTypePara, typename ValueTypePara, unsigned int indexPara>
class ValueNode
  : public GraphNodeBase<ValueNode<InputNodeTypePara, ValueTypePara, indexPara>> {
  public:
    using Base = GraphNodeBase<ValueNode>;
    using InputNodeTypes = std::tuple<std::shared_ptr<InputNodeTypePara>>;
    using ValueType = ValueTypePara;
    using ConcreteNodeType = ValueNode;
    ValueNode(std::shared_ptr<InputNodeTypePara> inputNode) : Base(), InputNodeTypes(inputNode), output(inputNode->getValue()) {} 
    ValueNode(const ValueType& valuePara) : Base(), output(valuePara) {} 
    ValueNode() = default;
    operator ValueType() const { return output; }
    ValueType getValue() { return output; }
    ValueType getValue() const { return output; }
    ValueType compute() const { return output; }
    void resetValue(const ValueType& operand) {
        output = operand;
    }
    
    static constexpr unsigned int index() {
        return indexPara;
    }
    //加这个是为了实现_calcPartialDerivative中，两节点相同情况下，矩阵的特殊情况，即matrix1 + matrix2情况下求_calcPartialDerivativ(matrix1, matrix1)的问题
    template<unsigned int variableIndex>
    auto derivative() const {
        static_assert((variableIndex==0), "The partial derivative variable index must be 0.");
        if constexpr (ad_math::is_scalar<ValueType>::value) {
                using DerivativeNodeType = internal::IdentityNode<internal::unique_index, ValueType>;//ValueNode<NullTypeNode, typename LeftInputNodeTypePara::ValueType, internal::unique_index>;
                auto derivativeNodePtr = std::shared_ptr<DerivativeNodeType>(new DerivativeNodeType(1));
                return derivativeNodePtr;
        }
        else if constexpr (ad_math::is_matrix<ValueType>::value) {
            if constexpr (variableIndex == 0) {
                auto row = output.rows();
                auto col = output.cols();
                using DerivativeNodeType = internal::IdentityNode<internal::unique_index, ValueType>;//ValueNode<NullTypeNode, typename LeftInputNodeTypePara::ValueType, internal::unique_index>;
                auto derivativeNodePtr = std::shared_ptr<DerivativeNodeType>(new DerivativeNodeType(ValueType::Identity(row*col, row*col)));
                return derivativeNodePtr;
            }
        }
    }
    template<unsigned int n>
    friend struct ChainRuleExpansion;

  private:
    ValueType output;
};

template<typename InputNodeTypePara, typename ValueTypePara, unsigned int indexPara>
struct traits<ValueNode<InputNodeTypePara, ValueTypePara, indexPara>> {
    using InputNodeTypes = std::tuple<std::shared_ptr<InputNodeTypePara>>;
    using ValueType = ValueTypePara;
    static constexpr unsigned int index = indexPara;
};

}

#endif