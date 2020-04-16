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

#ifndef METAAUTODIFF_ADDNODE_HPP
#define METAAUTODIFF_ADDNODE_HPP

#include"../Core/math/Add.hpp"
//#include"../Core/ValueNode.hpp"
#include"../Core/SpecialNode.hpp"
#include"../Core/NodeWrapper.hpp"
#include"../Core/math/ValueTrait.hpp"
#include<tuple>

namespace MetaAD {

template<typename LeftNodeTypePara, typename RightNodeTypePara>
struct AddNodeTrait {
    using LeftNodeValueType = typename LeftNodeTypePara::ValueType;
    using RightNodeValueType = typename RightNodeTypePara::ValueType;
    using type = typename ad_math::add_trait<LeftNodeValueType, RightNodeValueType>::type;
};

template<typename LeftInputNodeTypePara, typename RightInputNodeTypePara>
struct AddPolicyDefault {
    using LeftNodeValueType = typename LeftInputNodeTypePara::ValueType;
    using RightNodeValueType = typename RightInputNodeTypePara::ValueType;
    using type = typename ad_math::add_policy<LeftNodeValueType, RightNodeValueType>;
};

template<typename LeftInputNodeTypePara, typename RightInputNodeTypePara, unsigned int indexPara,
         typename ValueTypePara = typename AddNodeTrait<LeftInputNodeTypePara, RightInputNodeTypePara>::type, 
         typename PolicyPara = typename AddPolicyDefault<LeftInputNodeTypePara, RightInputNodeTypePara>::type,
         bool legalPara = isNodeType<LeftInputNodeTypePara>::value && isNodeType<RightInputNodeTypePara>::value>
class AddNodeImp;

template<typename LeftInputNodeTypePara, typename RightInputNodeTypePara, unsigned int indexPara, 
         typename ValueTypePara, typename PolicyPara>
class AddNodeImp<LeftInputNodeTypePara, RightInputNodeTypePara, indexPara, ValueTypePara, PolicyPara, true>
  : public GraphNodeBase<AddNodeImp<LeftInputNodeTypePara, RightInputNodeTypePara, indexPara, ValueTypePara, PolicyPara,true>> {
  public:
    using Base = GraphNodeBase<AddNodeImp>;
    using InputNodeTypes = std::tuple<std::shared_ptr<LeftInputNodeTypePara>, std::shared_ptr<RightInputNodeTypePara>>;
    using ValueType = ValueTypePara;
    using Policy = PolicyPara;
    using ConcreteNodeType = AddNodeImp;
    static constexpr unsigned int index() {
        return indexPara;
    }

    AddNodeImp(std::shared_ptr<LeftInputNodeTypePara> inputNodeL, std::shared_ptr<RightInputNodeTypePara> inputNodeR)
      : Base(), inputs(InputNodeTypes(inputNodeL, inputNodeR)) {
        output = inputNodeL->getValue() + inputNodeR->getValue();
    }

    //AddNodeImp(const AddNodeImp& rValue) = default;
    //AddNode(const AddNode&& lvalue) = default;    //不确定要不要加移动构造函数，这要求所有的成员都具有移动构造函数
    
    // 拷贝赋值运算符
    //AddNodeImp& operator=(const AddNodeImp& rValue) = default;

    ValueType getValue() { return output; }   
    ValueType getValue() const { return output; }   

    ValueType compute() const{
        ValueType result = std::get<0>(inputs)->compute() + std::get<1>(inputs)->compute();
        return result;
    }

    template<unsigned int variableIndex>
    auto partialDerivative() const {
        static_assert((variableIndex==0) || (variableIndex==1), "The partial derivative variable index must be 0 or 1.");
        auto derivativeValue =  Policy::template derivative<variableIndex>(std::get<0>(inputs)->getValue(),
                                                                                     std::get<1>(inputs)->getValue(),
                                                                                     output);
        return derivativeValue;
    }

    // Another implementation of derivation for first order derivation.
    template<unsigned int variableIndex>
    auto derivative() const {
        static_assert((variableIndex==0) || (variableIndex==1), "The partial derivative variable index must be 0 or 1.");
        if constexpr (ad_math::is_scalar<typename LeftInputNodeTypePara::ValueType>::value 
                   && ad_math::is_scalar<typename RightInputNodeTypePara::ValueType>::value) {
            if constexpr (variableIndex == 0) {
                // 这里用IdendityNode来实现，为了链式法则中index不要累加的过高
                // 这里有个问题，按道理来说IdendityNode是用ValueNode实现的，因此这里原来的DerivativeNodeType与现在的应该是等价的，
                // 但是没有触发IdendityNode版本的mulImp()重载函数,这是为什么？
                // 这两个类型其实是不一样的，因为原来ValueNode是以double为ValueType的，IdentityNode是以IdentityType为数据类型的。
                using DerivativeNodeType = internal::IdentityNode<internal::unique_index, typename LeftInputNodeTypePara::ValueType>;//ValueNode<NullTypeNode, typename LeftInputNodeTypePara::ValueType, internal::unique_index>;
                auto derivativeNodePtr = std::shared_ptr<DerivativeNodeType>(new DerivativeNodeType(1));
                return derivativeNodePtr;
            }
            else {
                using DerivativeNodeType = internal::IdentityNode<internal::unique_index, typename RightInputNodeTypePara::ValueType>;//ValueNode<NullTypeNode, typename RightInputNodeTypePara::ValueType, internal::unique_index>;
                auto derivativeNodePtr = std::shared_ptr<DerivativeNodeType>(new DerivativeNodeType(1));
                return derivativeNodePtr;
            }
        }
        else if constexpr (ad_math::is_matrix<typename LeftInputNodeTypePara::ValueType>::value
                        && ad_math::is_matrix<typename RightInputNodeTypePara::ValueType>::value) {
            if constexpr (variableIndex == 0) {
                auto row = (std::get<0>(inputs)->getValue()).rows();
                auto col = (std::get<0>(inputs)->getValue()).cols();
                // 这里还是得用这种方式，如果链式法则过程中出现了连续两个IdentityNode想乘，这个框架会返回左操作数，因此有可能会丢失维度，例如
                // matrix1 + matrix2 ，使用连式法则时，会返回与matrix1行列相同(m,n)的单位阵，实际上应该是(mn, mn)。
                // 这里就出现了问题，对矩阵自己对自己求导，维度会出现问题。
                using DerivativeNodeType = internal::IdentityNode<internal::unique_index, typename LeftInputNodeTypePara::ValueType>;;//ValueNode<NullTypeNode, typename LeftInputNodeTypePara::ValueType, internal::unique_index>;
                auto derivativeNodePtr = std::shared_ptr<DerivativeNodeType>(new DerivativeNodeType(LeftInputNodeTypePara::ValueType::Identity(row*col, row*col)));
                return derivativeNodePtr;
            }
            else {
                auto row = (std::get<1>(inputs)->getValue()).rows();
                auto col = (std::get<1>(inputs)->getValue()).cols();
                using DerivativeNodeType = internal::IdentityNode<internal::unique_index, typename RightInputNodeTypePara::ValueType>;//ValueNode<NullTypeNode, typename RightInputNodeTypePara::ValueType, internal::unique_index>;
                auto derivativeNodePtr = std::shared_ptr<DerivativeNodeType>(new DerivativeNodeType(RightInputNodeTypePara::ValueType::Identity(row*col, row*col)));
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
struct traits<AddNodeImp<LeftInputNodeTypePara, RightInputNodeTypePara, indexPara, ValueTypePara,
                      PolicyPara, legalPara>> {
    using InputNodeTypes = std::tuple<std::shared_ptr<LeftInputNodeTypePara>, std::shared_ptr<RightInputNodeTypePara>>;
    using ValueType = ValueTypePara;
    using Policy = PolicyPara;
    static constexpr unsigned int index = indexPara;
};

template<typename LeftInputNodeTypePara, typename RightInputNodeTypePara, unsigned int indexPara>
using AddNode_TMP = NodeWrapper<AddNodeImp<LeftInputNodeTypePara, RightInputNodeTypePara, indexPara>>;

template<typename ConcreteLeftNodeTypePara, typename ConcreteRightNodeTypePara,
         unsigned int indexPara = traits<ConcreteLeftNodeTypePara>::index + traits<ConcreteRightNodeTypePara>::index>
typename std::enable_if<(!internal::isZeroNode<ConcreteLeftNodeTypePara>::value) && (!internal::isZeroNode<ConcreteRightNodeTypePara>::value),
                        std::shared_ptr<AddNodeImp<ConcreteLeftNodeTypePara, ConcreteRightNodeTypePara, indexPara>>>::type
addImp(const std::shared_ptr<ConcreteLeftNodeTypePara>& leftNode, const std::shared_ptr<ConcreteRightNodeTypePara>& rightNode) {
    
    using NodeType = AddNodeImp<ConcreteLeftNodeTypePara, ConcreteRightNodeTypePara, indexPara>;
    return std::shared_ptr<NodeType>(new NodeType(leftNode, rightNode));//这里必须要访问NodeWrapper的pNode成员，因此我将该成员设为了public， 但是这样暴露得了内部实现，应该怎么做。

}

template<typename ConcreteLeftNodeTypePara, typename ConcreteRightNodeTypePara,
         unsigned int indexPara = traits<ConcreteLeftNodeTypePara>::index + traits<ConcreteRightNodeTypePara>::index>
typename std::enable_if<(internal::isZeroNode<ConcreteLeftNodeTypePara>::value) && (internal::isZeroNode<ConcreteRightNodeTypePara>::value),
                        std::shared_ptr<ConcreteLeftNodeTypePara>>::type
addImp(const std::shared_ptr<ConcreteLeftNodeTypePara>& leftNode, const std::shared_ptr<ConcreteRightNodeTypePara>& rightNode) {
    
    using NodeType = internal::ZeroNode<internal::unique_index,
                              typename ConcreteLeftNodeTypePara::ValueType>;
    return leftNode; 
    // 这里我选择直接返回任意一个ZeroNode节点，而不是用leftNode和rightNode的getValue()值生成一个新的ZeroNode节点，因为这两个
    // getValue()有可能无法相加，例如两个矩阵但是行列不匹配。

}

template<typename ConcreteLeftNodeTypePara, typename ConcreteRightNodeTypePara,
         unsigned int indexPara = traits<ConcreteRightNodeTypePara>::index>
typename std::enable_if<internal::isZeroNode<ConcreteLeftNodeTypePara>::value && (!internal::isZeroNode<ConcreteRightNodeTypePara>::value),
                        std::shared_ptr<ConcreteRightNodeTypePara>>::type
addImp(const std::shared_ptr<ConcreteLeftNodeTypePara>& leftNode, const std::shared_ptr<ConcreteRightNodeTypePara>& rightNode) {
    return rightNode;
}

template<typename ConcreteLeftNodeTypePara, typename ConcreteRightNodeTypePara,
         unsigned int indexPara = traits<ConcreteLeftNodeTypePara>::index>
typename std::enable_if<(!internal::isZeroNode<ConcreteLeftNodeTypePara>::value) && internal::isZeroNode<ConcreteRightNodeTypePara>::value,
                        std::shared_ptr<ConcreteLeftNodeTypePara>>::type
addImp(const std::shared_ptr<ConcreteLeftNodeTypePara>& leftNode, const std::shared_ptr<ConcreteRightNodeTypePara>& rightNode) {
    return leftNode;
}

template<typename ConcreteLeftNodeTypePara, typename ConcreteRightNodeTypePara>
auto operator+(const NodeWrapper<ConcreteLeftNodeTypePara>& leftNode, const NodeWrapper<ConcreteRightNodeTypePara>& rightNode) {
    auto concreteResultNodePtr = addImp(leftNode.pNode, rightNode.pNode);
    using ConcreteResultNodeType = typename decltype(concreteResultNodePtr)::element_type;
    return NodeWrapper<ConcreteResultNodeType>(concreteResultNodePtr);
}

}

#endif