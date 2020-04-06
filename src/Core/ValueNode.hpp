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
#include<Tuple>

namespace MetaAD {


template<typename InputNodeTypePara, typename ValueTypePara, unsigned int indexPara>
class ValueNode
  : public GraphNodeBase<ValueNode<InputNodeTypePara, ValueTypePara, indexPara>> {
  public:
    using Base = GraphNodeBase<ValueNode>;
    using InputNodeTypes = std::tuple<InputNodeTypePara*>;
    using ValueType = ValueTypePara;
    using ConcreteNodeType = ValueNode;
    ValueNode(const ValueType& valuePara) : Base(), output(valuePara) {} 
    ValueNode() = default;
    operator ValueType() { return output; }
    ValueType getValue() { return output; }
    ValueType getValue() const { return output; }
    static constexpr unsigned int index() {
        return indexPara;
    }
  private:
    ValueType output;
};

template<typename InputNodeTypePara, typename ValueTypePara, unsigned int indexPara>
struct traits<ValueNode<InputNodeTypePara, ValueTypePara, indexPara>> {
    using InputNodeTypes = std::tuple<InputNodeTypePara>;
    using ValueType = ValueTypePara;
    static constexpr unsigned int index = indexPara;
};

}

#endif