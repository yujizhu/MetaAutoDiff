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

#ifndef METAAUTODIFF_GRAPHEDGEBASE_HPP
#define METAAUTODIFF_GRAPHEDGEBASE_HPP

#include"util/Meta.hpp"
#include"util/ForwardDeclarations.hpp"

namespace MetaAD {

template<typename DerivedPara>
class GraphEdgeBase {
  public:
    using Derived = DerivedPara;
    // 头结点类型为箭头所指的节点，即偏导数的分子部分,分数线的上侧
    using HeadNodeType = typename traits<Derived>::HeadNodeType;
    // 尾结点的类型，为箭头尾部的节点类型，即偏导数的分母部分
    using TailNodeType = typename traits<Derived>::TailNodeType;
    using ValueType = typename traits<Derived>::ValueType;
    ValueType getValue() {
        return derived()->getValue();
    };
    ValueType getValue() const {
        return derived()->getValue();
    };
  private:
    Derived* derived() { return static_cast<Derived*>(this); }
    const Derived* derived() const { return static_cast<const Derived*>(this); }
};


//这个trait用于判断该类型是否是一个边类型
template<typename EdgeType>
struct isEdgeType {
    static constexpr bool value = internal::is_base_of<GraphEdgeBase<EdgeType>, EdgeType>::value;
};

// 基边类型也应该算作边类型
template<typename ConcreteEdgeTypePara>
struct isEdgeType<GraphEdgeBase<ConcreteEdgeTypePara>> {
    static constexpr bool value = true;
};

}

#endif