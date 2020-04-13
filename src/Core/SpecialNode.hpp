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

#include"ValueNode.hpp"
#include"math/SpecialType.hpp"

namespace MetaAD {

template<unsigned int indexPara, typename ValueType>
using IdendityNode = ValueNode<NullTypeNode, IdendityType_TMP<ValueType>, indexPara>;

template<unsigned int indexPara, typename ValueType>
using ZeroNode = ValueNode<NullTypeNode, ZeroType_TMP<ValueType>, indexPara>;

// If the NodeType is not a node, but a arbitary type, what should isIndedityNode::value be, ture or false or get an error.
template<typename NodeType>
struct isIdendityNode {
    static constexpr bool value = false;
};

template<unsigned int indexPara, typename ValueType>
struct isIdendityNode<ValueNode<NullTypeNode, IdendityType_TMP<ValueType>, indexPara>> {
    static constexpr bool value = true;
};

template<typename NodeType>
struct isZeroNode {
    static constexpr bool value = false;
};

template<unsigned int indexPara, typename ValueType>
struct isZeroNode<ValueNode<NullTypeNode, ZeroType_TMP<ValueType>, indexPara>> {
    static constexpr bool  value = true;
};

}

#endif