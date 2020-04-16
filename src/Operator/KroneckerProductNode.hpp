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

#ifndef METAAUTODIFF_KRONECKERPRODUCTNODE_HPP
#define METAAUTODIFF_KRONECKERPRODUCTNODE_HPP

#include"../Core/math/KroneckerProduct.hpp"
#include"../Core/GraphNodeBase.hpp"
#include"../Core/NodeWrapper.hpp"
#include<memory>
#include<tuple>

namespace MetaAD {

template<typename LeftNodeTypePara, typename RightNodeTypePara>
struct KroneckerProductNodeTrait {
    using LeftNodeValueType = typename LeftNodeTypePara::ValueType;
    using RightNodeValueType = typename RightNodeTypePara::ValueType;
    using type = typename ad_math::kronecker_product_trait<LeftNodeValueType, RightNodeValueType>::type;
};

template<typename LeftInputNodeTypePara, typename RightInputNodeTypePara>
struct KroneckerProductPolicyDefault {
    using LeftNodeValueType = typename LeftInputNodeTypePara::ValueType;
    using RightNodeValueType = typename RightInputNodeTypePara::ValueType;
    using type = typename ad_math::kronecker_product_policy<LeftNodeValueType, RightNodeValueType>;
};

template<typename LeftInputNodeTypePara, typename RightInputNodeTypePara, unsigned int indexPara,
         typename ValueTypePara = typename KroneckerProductNodeTrait<LeftInputNodeTypePara, RightInputNodeTypePara>::type, 
         typename PolicyPara = typename KroneckerProductPolicyDefault<LeftInputNodeTypePara, RightInputNodeTypePara>::type,
         bool legalPara = isNodeType<LeftInputNodeTypePara>::value && isNodeType<RightInputNodeTypePara>::value>
class KroneckerProductNodeImp;

template<typename LeftInputNodeTypePara, typename RightInputNodeTypePara, unsigned int indexPara, 
         typename ValueTypePara, typename PolicyPara>
class KroneckerProductNodeImp<LeftInputNodeTypePara, RightInputNodeTypePara, indexPara, ValueTypePara, PolicyPara, true>
  : public GraphNodeBase<KroneckerProductNodeImp<LeftInputNodeTypePara, RightInputNodeTypePara, indexPara, ValueTypePara, PolicyPara,true>> {
  public:
    using Base = GraphNodeBase<KroneckerProductNodeImp>;
    using InputNodeTypes = std::tuple<std::shared_ptr<LeftInputNodeTypePara>, std::shared_ptr<RightInputNodeTypePara>>;
    using ValueType = ValueTypePara;
    using DerivativePolicy = PolicyPara;
    using ConcreteNodeType = KroneckerProductNodeImp;
    static constexpr unsigned int index() {
        return indexPara;
    }
    KroneckerProductNodeImp(std::shared_ptr<LeftInputNodeTypePara> inputNodeL, std::shared_ptr<RightInputNodeTypePara> inputNodeR)
      : Base(), inputs(InputNodeTypes(inputNodeL, inputNodeR)) {
        output = PolicyPara::compute(inputNodeL->getValue(), inputNodeR->getValue());
    }

    ValueType getValue() { return output; }   
    ValueType getValue() const { return output; }   

    ValueType compute() {
        ValueType result = PolicyPara::compute(std::get<0>(inputs)->compute(), std::get<1>(inputs)->compute());
        return result;
    }

    template<unsigned int n>
    friend struct ChainRuleExpansion;

  private:
    InputNodeTypes inputs;
    ValueType output;
};

template<typename LeftInputNodeTypePara, typename RightInputNodeTypePara, unsigned int indexPara,
         typename ValueTypePara, typename PolicyPara, bool legalPara>
struct traits<KroneckerProductNodeImp<LeftInputNodeTypePara, RightInputNodeTypePara, indexPara, ValueTypePara,
                      PolicyPara, legalPara>> {
    using InputNodeTypes = std::tuple<std::shared_ptr<LeftInputNodeTypePara>, std::shared_ptr<RightInputNodeTypePara>>;
    using ValueType = ValueTypePara;
    using Policy = PolicyPara;
    static constexpr unsigned int index = indexPara;
};

template<typename ConcreteLeftNodeTypePara, typename ConcreteRightNodeTypePara,
         unsigned int indexPara = ConcreteLeftNodeTypePara::index() + ConcreteRightNodeTypePara::index()>
auto kroneckerProductImp(const std::shared_ptr<ConcreteLeftNodeTypePara>& leftNode, const std::shared_ptr<ConcreteRightNodeTypePara>& rightNode) {
    using NodeType = KroneckerProductNodeImp<ConcreteLeftNodeTypePara, ConcreteRightNodeTypePara, indexPara>;
    return std::shared_ptr<NodeType>(new NodeType(leftNode, rightNode));
}

template<typename ConcreteLeftNodeTypePara, typename ConcreteRightNodeTypePara>
auto kroneckerProduct(const NodeWrapper<ConcreteLeftNodeTypePara>& leftNode, 
                      const NodeWrapper<ConcreteRightNodeTypePara>& rightNode) {
    auto concreteResultNodePtr = kroneckerProductImp(leftNode.pNode, rightNode.pNode);
    using ConcreteResultNodeType = typename decltype(concreteResultNodePtr)::element_type;
    return NodeWrapper<ConcreteResultNodeType>(concreteResultNodePtr);
}

}

#endif