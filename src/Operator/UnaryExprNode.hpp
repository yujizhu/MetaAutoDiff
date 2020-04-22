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

#ifndef METAAUTODIFF_UNARYEXPRNODE_HPP
#define METAAUTODIFF_UNARYEXPRNODE_HPP

#include"../Core/math/UnaryExpr.hpp"
#include"../Core/GraphNodeBase.hpp"
#include"../Core/NodeWrapper.hpp"
#include<memory>
#include<tuple>

namespace MetaAD {

template<typename InputNodeTypePara>
struct UnaryExprNodeTrait {
    using InputNodeValueType = typename InputNodeTypePara::ValueType;
    using type = typename ad_math::unary_expr_trait<InputNodeValueType>::type;
};

template<typename InputNodeTypePara>
struct UnaryExprPolicyDefault {
    using InputNodeValueType = typename InputNodeTypePara::ValueType;
    using type = typename ad_math::unary_expr_policy<InputNodeValueType>;
};

template<typename CustomUnaryOp, typename UnaryDerivativeOp, typename InputNodeTypePara, unsigned int indexPara,
         typename ValueTypePara = typename UnaryExprNodeTrait<InputNodeTypePara>::type, 
         typename PolicyPara = typename UnaryExprPolicyDefault<InputNodeTypePara>::type,
         bool legalPara = isNodeType<InputNodeTypePara>::value>
class UnaryExprNodeImp;

template<typename CustomUnaryOp, typename UnaryDerivativeOp, typename InputNodeTypePara, unsigned int indexPara, typename ValueTypePara, typename PolicyPara>
class UnaryExprNodeImp<CustomUnaryOp, UnaryDerivativeOp, InputNodeTypePara, indexPara, ValueTypePara, PolicyPara, true>
  : public GraphNodeBase<UnaryExprNodeImp<CustomUnaryOp, UnaryDerivativeOp, InputNodeTypePara, indexPara, ValueTypePara, PolicyPara,true>> {
  public:
    using Base = GraphNodeBase<UnaryExprNodeImp>;
    using InputNodeTypes = std::tuple<std::shared_ptr<InputNodeTypePara>>;
    using ValueType = ValueTypePara;
    using Policy = PolicyPara;
    using ConcreteNodeType = UnaryExprNodeImp;
    static constexpr unsigned int index() {
        return indexPara;
    }

    UnaryExprNodeImp() = delete;

    UnaryExprNodeImp(std::shared_ptr<InputNodeTypePara> inputNode, const CustomUnaryOp& func = CustomUnaryOp(), const UnaryDerivativeOp& dFunc = UnaryDerivativeOp())
      : Base(), inputs(InputNodeTypes(inputNode)), m_functor(func),  m_dFunctor(dFunc) {
        output = Policy::compute(inputNode->getValue(), func);
    }

    //MatTransposeNodeImp(const MatTransposeNodeImp& rValue) = default;
    //MatTransposeNodeImp(const MatTransposeNodeImp&& lvalue) = default;    //不确定要不要加移动构造函数，这要求所有的成员都具有移动构造函数
    
    // 拷贝赋值运算符
    // MatTransposeNodeImp& operator=(const MatTransposeNodeImp& rValue) = default;

    ValueType getValue() { return output; }   
    ValueType getValue() const { return output; }   

    ValueType compute() const {
        ValueType result = Policy::compute(std::get<0>(inputs)->compute(), m_functor);
        return result;
    }

    template<unsigned int variableIndex>
    auto derivative() const;


    template<unsigned int n>
    friend struct ChainRuleExpansion;

  private:
    InputNodeTypes inputs;
    ValueType output;
    const CustomUnaryOp m_functor;
    const UnaryDerivativeOp m_dFunctor;
};

template<typename CustomUnaryOp, typename UnaryDerivativeOp, typename InputNodeTypePara, unsigned int indexPara,
         typename ValueTypePara, typename PolicyPara, bool legalPara>
struct traits<UnaryExprNodeImp<CustomUnaryOp, UnaryDerivativeOp, InputNodeTypePara, indexPara, ValueTypePara, PolicyPara, legalPara>> {
    using InputNodeTypes = std::tuple<std::shared_ptr<InputNodeTypePara>>;
    using ValueType = ValueTypePara;
    using Policy = PolicyPara;
    static constexpr unsigned int index = indexPara;
};

template<typename CustomUnaryOp, typename UnaryDerivativeOp, typename ConcreteInputNodeTypePara, unsigned int indexPara = ConcreteInputNodeTypePara::index() + 1>
auto unaryExprImp(const std::shared_ptr<ConcreteInputNodeTypePara>& inputNodePtr, const CustomUnaryOp& func = CustomUnaryOp(),  const UnaryDerivativeOp& dFunc = UnaryDerivativeOp()) {
    using NodeType = UnaryExprNodeImp<CustomUnaryOp, UnaryDerivativeOp, ConcreteInputNodeTypePara, indexPara>;
    return std::shared_ptr<NodeType>(new NodeType(inputNodePtr, func, dFunc));
}

template<typename CustomUnaryOp, typename UnaryDerivativeOp, typename ConcreteInputNodeTypePara>
auto unaryExpr(const NodeWrapper<ConcreteInputNodeTypePara>& inputNode, const CustomUnaryOp& func = CustomUnaryOp(),  const UnaryDerivativeOp& dFunc = UnaryDerivativeOp()) {
    auto concreteResultNodePtr = unaryExprImp(inputNode.pNode, func, dFunc);
    using ConcreteResultNodeType = typename decltype(concreteResultNodePtr)::element_type;
    return NodeWrapper<ConcreteResultNodeType>(concreteResultNodePtr);
}

template<typename CustomUnaryOp, typename UnaryDerivativeOp, typename InputNodeTypePara, unsigned int indexPara, typename ValueTypePara, typename PolicyPara>
template<unsigned int variableIndex>
auto UnaryExprNodeImp<CustomUnaryOp, UnaryDerivativeOp, InputNodeTypePara, indexPara, ValueTypePara, PolicyPara, true>::derivative() const {
    static_assert(variableIndex == 0, "The partial derivative variable index must be 0.");
    // 这里必须直接定义类型，不能用cosImp运算获得，避免多次求导过程中计算图中出现相同的类型
    using TempNodeType = UnaryExprNodeImp<UnaryDerivativeOp, UnaryDerivativeOp, InputNodeTypePara, indexPara + 1>;
    auto tempNodePtr = std::shared_ptr<TempNodeType>(new TempNodeType(std::get<0>(inputs), m_dFunctor, m_dFunctor));
    auto vecTempNodePtr = rowVectorizationImp(tempNodePtr);
    auto diagTempNodePtr = matDiagonalImp(vecTempNodePtr);
    return diagTempNodePtr;
}




}




#endif