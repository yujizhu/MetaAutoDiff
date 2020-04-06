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

#ifndef METAAUTODIFF_GRAPHNODEBASE_HPP
#define METAAUTODIFF_GRAPHNODEBASE_HPP


#include"util/Meta.hpp"
#include"util/ForwardDeclarations.hpp"

namespace MetaAD {

template<typename DerivedPara>
class GraphNodeBase {
  public:
    // Derived是该节点真正的类型，该类型一定派生自GraphNodeBase。
    using Derived = DerivedPara;
    using ConcreteNodeType = Derived;
    // ValueType是该节点所存储的值得类型，即该节点的输出值类型。
    using ValueType = typename traits<Derived>::ValueType;
    // InputNodeTypes一定是一个tuple类型，tuple中存放该节点的输入节点的指针类型，顺序与该算子对输入顺序的定义相关。
    using InputNodeTypes = typename traits<Derived>::InputNodeTypes;
    //using DerivativePolicy = typename traits<Derived>::DerivativePolicy;
    ValueType getValue() {
        return derived()-> getValue();
    };
    ValueType getValue() const {
        return derived()-> getValue();
    };
    Derived* getDerivedPtr() { return derived(); }
    const Derived* getDerivedPtr() const { return derived(); }
    static constexpr unsigned int index() {
        return traits<Derived>::index;
	}
    
    template<unsigned int variableIndex>
    auto partialDerivative() const {
        return (derived())->template partialDerivative<variableIndex>();
    }
  private:
    Derived* derived() { return static_cast<Derived*>(this); }
    const Derived* derived() const { return static_cast<const Derived*>(this); }
};


struct NullTypeNode {};

//这个trait用于判断该类型是否是一个节点。
template<typename NodeType>
struct isNodeType {
    static constexpr bool value = internal::is_base_of<GraphNodeBase<NodeType>, NodeType>::value;
};

// 该基节点类型也应该算作节点类型
template<typename ConcreteNodeTypePara>
struct isNodeType<GraphNodeBase<ConcreteNodeTypePara>> {
    static constexpr bool value = true;
};

template<>
struct isNodeType<NullTypeNode> {
    static constexpr bool value = true;
};

}

#endif