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

#ifndef METAAUTODIFF_NODEWRAPPER_HPP
#define METAAUTODIFF_NODEWRAPPER_HPP

#include"GraphNodeBase.hpp"
#include<memory>


namespace MetaAD{

template<typename NodeTypePara, bool legal = isNodeType<NodeTypePara>::value 
                                       && (!(isBaseNodeType<NodeTypePara>::value))>
class NodeWrapper;

template<typename NodeTypePara>
class NodeWrapper<NodeTypePara, true> {
  public:
    using ConcreteNodeType = typename NodeTypePara::ConcreteNodeType;
    NodeWrapper(std::shared_ptr<ConcreteNodeType> pNodePara) : pNode(pNodePara) {}
    //NodeWrapper(const NodeWrapper& rhs) = default;

    template<unsigned int variableIndex>
    auto derivative() ;/*{
        auto derivativeNodePtr = pNode->template derivative<variableIndex>();
        return derivative;
    }*/

    auto getValue() {
        return pNode->getValue();
    }
    
    auto compute() {
        return pNode->compute();
    }
    // only ValueNode has resetValue member.
    void resetValue(const typename NodeTypePara::ValueType& operand) {
        pNode->resetValue(operand);
    }

    std::shared_ptr<ConcreteNodeType> pNode;
};

template<typename NodeTypePara>
template<unsigned int variableIndex>
auto NodeWrapper<NodeTypePara, true>::derivative() 
{
    auto derivativeNodePtr = pNode->template derivative<variableIndex>();
    using DerivativeNodeType = typename decltype(derivativeNodePtr)::element_type;
    return NodeWrapper<DerivativeNodeType>(derivativeNodePtr);
}

}

#endif