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

#include "../Core/GraphNodeBase.hpp"
#include"../Core/math/Add.hpp"
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
class AddNode;


// 不为每个几点单独设置边类型了,选择单独定义一个梯度边类模板，详见GradientEdge.hpp
/*
template<typename HeadNodeTypePara, typename TailNodeTypePara, typename DerivativeValueTypePara>
class AddEdge
  : public GraphEdgeBase<AddEdge<HeadNdeTypePara, DerivedValueTypePara>> {
  private:
    HeadNodeType* headNodePtr;
    AddNode* tailNodePtr;
    DerivedValueType derivedValue;
  public:
    using ValueType = DerivativeValueTypePara;
    using HeadNodeType = Add;
    using TailNodeType = HeadNodeTypePara;
    AddEdge() = delete;
    AddEdge(HeadNodeType* headNodePtrPara, Add* tailNodePtrPara, DerivedValueType derivedValuePara)
        :headNodePtr(headNodePtrPara), tailNodePtr(tailNodePtrPara), derivedValue(derivedValuePara);

    DerivedDateType getValue(){ return derivedValue; }
};
*/

template<typename LeftInputNodeTypePara, typename RightInputNodeTypePara, unsigned int indexPara, 
         typename ValueTypePara, typename PolicyPara>
class AddNode<LeftInputNodeTypePara, RightInputNodeTypePara, indexPara, ValueTypePara, PolicyPara, true>
  : public GraphNodeBase<AddNode<LeftInputNodeTypePara, RightInputNodeTypePara, indexPara, ValueTypePara, PolicyPara,true>> {
  public:
    using Base = GraphNodeBase<AddNode>;
    using InputNodeTypes = std::tuple<LeftInputNodeTypePara*, RightInputNodeTypePara*>;
    using ValueType = ValueTypePara;
    using Policy = PolicyPara;
    using ConcreteNodeType = AddNode;
    static constexpr unsigned int index() {
        return indexPara;
    }

    AddNode(LeftInputNodeTypePara* inputNodeL, RightInputNodeTypePara* inputNodeR)
      : Base(), inputs(std::tuple<LeftInputNodeTypePara*, RightInputNodeTypePara*>(inputNodeL, inputNodeR)) {
        output = inputNodeL->getValue() + inputNodeR->getValue();
    }

    AddNode(const AddNode& rValue) = default;
    //AddNode(const AddNode&& lvalue) = default;    //不确定要不要加移动构造函数，这要求所有的成员都具有移动构造函数
    
    // 拷贝赋值运算符
    AddNode& operator=(const AddNode& rValue) = default;

    ValueType getValue() { return output; }   
    ValueType getValue() const { return output; }   
    template<unsigned int variableIndex>
    auto partialDerivative() const {
        static_assert((variableIndex==0) || (variableIndex==1), "The partial derivative variable index must be 0 or 1.");
        auto derivativeValue =  Policy::template derivative<variableIndex>(std::get<0>(inputs)->getValue(),
                                                                                     std::get<1>(inputs)->getValue(),
                                                                                     output);
        return derivativeValue;
        /*
        if constexpr (variableIndex == 0)
            return GradientEdge<AddNode, LeftInputNodeTypePara>(this, std::get<0>(inputs), derivativeValue);
        else if constexpr (variableIndex == 1)
            return GradientEdge<AddNode, RightInputNodeTypePara>(this, std::get<1>(inputs), derivativeValue);
        */	
    }
    

    // 这个偏导数可以用成员函数来实现，虽然函数不能被偏特化，但是函数是能被特化的，下面的偏导数可能实现都是基于函数模板的特化
    // 实现的。
    /*
    template<unsigned int>
    class _PartialDerivative;

    template<>
    class _PartialDerivative<1> {
      public:
        static auto evulate() {
            using LeftInputNodeValueType = LeftInputNodeTypePara::ValueType;
            using RightInputNodeValueType = RightInputNodeTypePara::ValueType;
            using partialDerivativeType = ad_math::paratial_derivative_trait<ValueType, LeftInputNodeValueType>;      // 这里应该有一个trait，判断偏导数结果的类型
            return AddEdge<LeftInputNodeTypePara, PartialDerivedType>(input.get<1>(), this, math::OneValue<partialDervativeType>(1));
        }
    };

    template<>
    class _PartialDerive<2> {
      public:
        static auto evulate() {
            using partialDerivativeType = 
            return AddEdge<RightInputNodeTypePara, PartialDerivativeType>(input.get<2>(), this, partialDerivativeType(1));
        }
    }
    */
    template<unsigned int n>
    friend struct ChainRuleExpansion;
  private:
    InputNodeTypes inputs;
    ValueType output;
};

template<typename LeftInputNodeTypePara, typename RightInputNodeTypePara, unsigned int indexPara,
         typename ValueTypePara, typename PolicyPara, bool legalPara>
struct traits<AddNode<LeftInputNodeTypePara, RightInputNodeTypePara, indexPara, ValueTypePara,
                      PolicyPara, legalPara>> {
    using InputNodeTypes = std::tuple<LeftInputNodeTypePara*, RightInputNodeTypePara*>;
    using ValueType = ValueTypePara;
    using Policy = PolicyPara;
    static constexpr unsigned int index = indexPara;
};

/*  // 下面这种形式暂时不行了
// 如果参数为非const，则无法将临时对象作为实参，例如将某函数的返回对象作为实参等，但是如果将形参该位const，则
// 需要将所有的都改为const，包括AddNode构造函数以及tuple中的指针类型，而且使用const版本意味着，计算图中尾结点
// 无法通过其包含的tuple成员更改输入节点的值。
template<typename ConcreteLeftNodeTypePara, typename ConcreteRightNodeTypePara>
auto operator+(GraphNodeBase<ConcreteLeftNodeTypePara>& leftNode, 
               GraphNodeBase<ConcreteRightNodeTypePara>& rightNode) {
    
    // 这里必须使用getPtrFromBase成员来返回指向Derived类型的指针，因为指针类型无法实现从基类到派生类的自动转换，而
    // 该构造函数的形参类型都是指向派生类的指针，因此需要在GraphNodeBase中加上getPtrFromBase成员来返回指向派生类类型的指针
    return AddNode<ConcreteLeftNodeTypePara, ConcreteRightNodeTypePara>(leftNode.getDerivedPtr(),
                                                                        rightNode.getDerivedPtr());

}

*/
template<unsigned int indexPara, typename ConcreteLeftNodeTypePara, typename ConcreteRightNodeTypePara>
auto add(GraphNodeBase<ConcreteLeftNodeTypePara>& leftNode, 
         GraphNodeBase<ConcreteRightNodeTypePara>& rightNode) {
    
    // 这里必须使用getPtrFromBase成员来返回指向Derived类型的指针，因为指针类型无法实现从基类到派生类的自动转换，而
    // 该构造函数的形参类型都是指向派生类的指针，因此需要在GraphNodeBase中加上getPtrFromBase成员来返回指向派生类类型的指针
    return AddNode<ConcreteLeftNodeTypePara, ConcreteRightNodeTypePara, indexPara>(leftNode.getDerivedPtr(),
                                                                        rightNode.getDerivedPtr());

}

}

#endif