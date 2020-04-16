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

#ifndef METAAUTODIFF_SPECIALNODE_HPP
#define METAAUTODIFF_SPECIALNODE_HPP

#include"GraphNodeBase.hpp"
#include"math/SpecialType.hpp"
#include<tuple>
#include<memory>

namespace MetaAD {

namespace internal {

template<unsigned int indexPara, typename ValueTypePara>
class IdentityNode : public GraphNodeBase<IdentityNode<indexPara, ValueTypePara>> {
  public:
    using Base = GraphNodeBase<IdentityNode>;
    using InputNodeTypes = std::tuple<std::shared_ptr<NullTypeNode>>;
    using ValueType = ValueTypePara;
    using ConcreteNodeType = IdentityNode;
    IdentityNode(const ValueType& valuePara) : Base(), output(valuePara) {} 
    IdentityNode(const IdentityType<ValueType>& valuePara) : Base(), output(valuePara) {}
    operator ValueType() { return output; }
    ValueType getValue() { return output.getValue(); }
    ValueType getValue() const { return output.getValue(); }
    ValueType compute() const {  return output.getValue(); }
    static constexpr unsigned int index() {
        return indexPara;
    }
  private:
    IdentityType<ValueType> output;
};

template<unsigned int indexPara, typename ValueTypePara>
class ZeroNode : public GraphNodeBase<ZeroNode<indexPara, ValueTypePara>> {
  public:
    using Base = GraphNodeBase<ZeroNode>;
    using InputNodeTypes = std::tuple<std::shared_ptr<NullTypeNode>>;
    using ValueType = ValueTypePara;
    using ConcreteNodeType = ZeroNode;
    ZeroNode(const ValueType& valuePara) : Base(), output(valuePara) {} 
    ZeroNode(const IdentityType<ValueType>& valuePara) : Base(), output(valuePara) {}
    operator ValueType() { return output; }
    ValueType getValue() { return output.getValue(); }
    ValueType getValue() const { return output.getValue(); }
    ValueType compute() const {  return output.getValue(); }
    static constexpr unsigned int index() {
        return indexPara;
    }
  private:
    ZeroType<ValueType> output;
};

// If the NodeType is not a node, but a arbitary type, what should isIdentityNode::value be, ture or false or get an error.
template<typename NodeType>
struct isIdentityNode {
    static constexpr bool value = false;
};

template<unsigned int indexPara, typename ValueTypePara>
struct isIdentityNode<IdentityNode<indexPara, ValueTypePara>> {
    static constexpr bool value = true;
};

template<typename NodeType>
struct isZeroNode {
    static constexpr bool value = false;
};

template<unsigned int indexPara, typename ValueTypePara>
struct isZeroNode<ZeroNode<indexPara, ValueTypePara>> {
    static constexpr bool  value = true;
};

}

template<typename ValueTypePara, unsigned int indexPara>
struct traits<internal::IdentityNode<indexPara, ValueTypePara>> {
    using InputNodeTypes = std::tuple<std::shared_ptr<NullTypeNode>>;
    using ValueType = ValueTypePara;
    static constexpr unsigned int index = indexPara;
};

template<typename ValueTypePara, unsigned int indexPara>
struct traits<internal::ZeroNode<indexPara, ValueTypePara>> {
    using InputNodeTypes = std::tuple<std::shared_ptr<NullTypeNode>>;
    using ValueType = ValueTypePara;
    static constexpr unsigned int index = indexPara;
};

}

#endif