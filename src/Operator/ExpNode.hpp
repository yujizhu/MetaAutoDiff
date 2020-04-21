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

#ifndef METAAUTODIFF_EXPNODE_HPP
#define METAAUTODIFF_EXPNODE_HPP

#include"../Core/math/Exp.hpp"
#include"../Core/GraphNodeBase.hpp"
#include"../Core/NodeWrapper.hpp"
#include"../Core/math/ValueTrait.hpp"
#include"VectorizationNode.hpp"
#include"MatDiagonalNode.hpp"
#include<memory>
#include<tuple>

namespace MetaAD {

template<typename InputNodeTypePara>
struct ExpNodeTrait {
    using InputNodeValueType = typename InputNodeTypePara::ValueType;
    using type = typename ad_math::exp_trait<InputNodeValueType>::type;
};

template<typename InputNodeTypePara>
struct ExpPolicyDefault {
    using InputNodeValueType = typename InputNodeTypePara::ValueType;
    using type = typename ad_math::exp_policy<InputNodeValueType>;
};

template<typename InputNodeTypePara, unsigned int indexPara,
         typename ValueTypePara = typename ExpNodeTrait<InputNodeTypePara>::type, 
         typename PolicyPara = typename ExpPolicyDefault<InputNodeTypePara>::type,
         bool legalPara = isNodeType<InputNodeTypePara>::value>
class ExpNodeImp;

template<typename InputNodeTypePara, unsigned int indexPara, typename ValueTypePara, typename PolicyPara>
class ExpNodeImp<InputNodeTypePara, indexPara, ValueTypePara, PolicyPara, true>
  : public GraphNodeBase<ExpNodeImp<InputNodeTypePara, indexPara, ValueTypePara, PolicyPara,true>> {
  public:
    using Base = GraphNodeBase<ExpNodeImp>;
    using InputNodeTypes = std::tuple<std::shared_ptr<InputNodeTypePara>>;
    using ValueType = ValueTypePara;
    using Policy = PolicyPara;
    using ConcreteNodeType = ExpNodeImp;
    static constexpr unsigned int index() {
        return indexPara;
    }

    ExpNodeImp() = delete;

    ExpNodeImp(std::shared_ptr<InputNodeTypePara> inputNode)
      : Base(), inputs(InputNodeTypes(inputNode)) {
        output = Policy::compute(inputNode->getValue());
    }

    //MatTransposeNodeImp(const MatTransposeNodeImp& rValue) = default;
    //MatTransposeNodeImp(const MatTransposeNodeImp&& lvalue) = default;    //不确定要不要加移动构造函数，这要求所有的成员都具有移动构造函数
    
    // 拷贝赋值运算符
    // MatTransposeNodeImp& operator=(const MatTransposeNodeImp& rValue) = default;

    ValueType getValue() { return output; }   
    ValueType getValue() const { return output; }   

    ValueType compute() const {
        ValueType result = Policy::compute(std::get<0>(inputs)->compute());
        return result;
    }

    template<unsigned int variableIndex>
    auto derivative() const;

    template<unsigned int n>
    friend struct ChainRuleExpansion;

  private:
    InputNodeTypes inputs;
    ValueType output;
};

template<typename InputNodeTypePara, unsigned int indexPara,
         typename ValueTypePara, typename PolicyPara, bool legalPara>
struct traits<ExpNodeImp<InputNodeTypePara, indexPara, ValueTypePara, PolicyPara, legalPara>> {
    using InputNodeTypes = std::tuple<std::shared_ptr<InputNodeTypePara>>;
    using ValueType = ValueTypePara;
    using Policy = PolicyPara;
    static constexpr unsigned int index = indexPara;
};

template<typename ConcreteInputNodeTypePara, unsigned int indexPara = ConcreteInputNodeTypePara::index() + 1>
auto expImp(const std::shared_ptr<ConcreteInputNodeTypePara>& inputNodePtr) {
    using NodeType = ExpNodeImp<ConcreteInputNodeTypePara, indexPara>;
    return std::shared_ptr<NodeType>(new NodeType(inputNodePtr));
}

template<typename ConcreteInputNodeTypePara>
auto exp(const NodeWrapper<ConcreteInputNodeTypePara>& inputNode) {
    auto concreteResultNodePtr = expImp(inputNode.pNode);
    using ConcreteResultNodeType = typename decltype(concreteResultNodePtr)::element_type;
    return NodeWrapper<ConcreteResultNodeType>(concreteResultNodePtr);
}

template<typename InputNodeTypePara, unsigned int indexPara, typename ValueTypePara, typename PolicyPara>
template<unsigned int variableIndex>
auto ExpNodeImp<InputNodeTypePara, indexPara, ValueTypePara, PolicyPara, true>::derivative() const {
    static_assert(variableIndex == 0, "The partial derivative variable index must be 0.");
    if constexpr (ad_math::is_scalar<typename InputNodeTypePara::ValueType>::value) {
        using DerivativeNodeType = ExpNodeImp<InputNodeTypePara, indexPara + 1>; // +1是为了不让求导节点与原结果节点的类型相同
        auto derivativeNodePtr = std::shared_ptr<DerivativeNodeType>(new DerivativeNodeType(std::get<0>(inputs)));
        return derivativeNodePtr;
    }
    else if constexpr (ad_math::is_matrix<typename InputNodeTypePara::ValueType>::value) {
        // 这里必须直接定义类型，不能用cosImp运算获得，避免多次求导过程中计算图中出现相同的类型
        using TempNodeType = ExpNodeImp<InputNodeTypePara, indexPara + 1>;
        auto tempNodePtr = std::shared_ptr<TempNodeType>(new TempNodeType(std::get<0>(inputs)));
        auto vecTempNodePtr = rowVectorizationImp(tempNodePtr);
        auto diagTempNodePtr = matDiagonalImp(vecTempNodePtr);
        return diagTempNodePtr;
    }
}

}

#endif