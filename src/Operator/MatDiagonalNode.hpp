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

#ifndef METAAUTODIFF_MATDIAGONALNODE_HPP
#define METAAUTODIFF_MATDIAGONALNODE_HPP

#include"../Core/math/MatDiagonal.hpp"
#include"../Core/GraphNodeBase.hpp"
#include"../Core/NodeWrapper.hpp"
#include<memory>
#include<tuple>

namespace MetaAD {

template<typename InputNodeTypePara>
struct MatDiagonalNodeTrait {
    using InputNodeValueType = typename InputNodeTypePara::ValueType;
    using type = typename ad_math::mat_diagonal_trait<InputNodeValueType>::type;
};

template<typename InputNodeTypePara>
struct MatDiagonalPolicyDefault {
    using InputNodeValueType = typename InputNodeTypePara::ValueType;
    using type = typename ad_math::mat_diagonal_policy<InputNodeValueType>;
};

template<typename InputNodeTypePara, unsigned int indexPara,
         typename ValueTypePara = typename MatDiagonalNodeTrait<InputNodeTypePara>::type, 
         typename PolicyPara = typename MatDiagonalPolicyDefault<InputNodeTypePara>::type,
         bool legalPara = isNodeType<InputNodeTypePara>::value>
class MatDiagonalNodeImp;

template<typename InputNodeTypePara, unsigned int indexPara, typename ValueTypePara, typename PolicyPara>
class MatDiagonalNodeImp<InputNodeTypePara, indexPara, ValueTypePara, PolicyPara, true>
  : public GraphNodeBase<MatDiagonalNodeImp<InputNodeTypePara, indexPara, ValueTypePara, PolicyPara,true>> {
  public:
    using Base = GraphNodeBase<MatDiagonalNodeImp>;
    using InputNodeTypes = std::tuple<std::shared_ptr<InputNodeTypePara>>;
    using ValueType = ValueTypePara;
    using Policy = PolicyPara;
    using ConcreteNodeType = MatDiagonalNodeImp;
    static constexpr unsigned int index() {
        return indexPara;
    }

    MatDiagonalNodeImp() = delete;

    MatDiagonalNodeImp(std::shared_ptr<InputNodeTypePara> inputNode)
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
struct traits<MatDiagonalNodeImp<InputNodeTypePara, indexPara, ValueTypePara, PolicyPara, legalPara>> {
    using InputNodeTypes = std::tuple<std::shared_ptr<InputNodeTypePara>>;
    using ValueType = ValueTypePara;
    using Policy = PolicyPara;
    static constexpr unsigned int index = indexPara;
};

template<typename ConcreteInputNodeTypePara, unsigned int indexPara = ConcreteInputNodeTypePara::index() + 1>
auto matDiagonalImp(const std::shared_ptr<ConcreteInputNodeTypePara>& inputNodePtr) {
    using NodeType = MatDiagonalNodeImp<ConcreteInputNodeTypePara, indexPara>;
    return std::shared_ptr<NodeType>(new NodeType(inputNodePtr));
}

template<typename ConcreteInputNodeTypePara>
auto matDiagonal(const NodeWrapper<ConcreteInputNodeTypePara>& inputNode) {
    auto concreteResultNodePtr = matDiagonalImp(inputNode.pNode);
    using ConcreteResultNodeType = typename decltype(concreteResultNodePtr)::element_type;
    return NodeWrapper<ConcreteResultNodeType>(concreteResultNodePtr);
}


}


#endif
