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

#ifndef METAAUTODIFF_GRADIENTEDGE_HPP
#define METAAUTODIFF_GRADIENTEDGE_HPP

#include"GraphEdgeBase.hpp"
#include"math/PartialDerivativeTrait.hpp"

namespace MetaAD {

// 前两个模板参数必须是节点类型，且必须是它的退化类型(decay)，即不能带有const 或者& 类型说明符
template<typename HeadNodeTypePara, typename TailNodeTypePara,
        typename ValueTypePara = typename ad_math::partial_derivative_trait<typename TailNodeTypePara::ValueType,
		                                                                    typename HeadNodeTypePara::ValueType>::type>
class GradientEdge: public GraphEdgeBase<GradientEdge<HeadNodeTypePara, TailNodeTypePara, ValueTypePara>> {
  public:
    using Base = GraphEdgeBase<GradientEdge>;
	using HeadNodeType = HeadNodeTypePara; // 表示箭头所指的节点，即偏导数中的分子部分，分数线的上面
	using TailNodeType = TailNodeTypePara; // 表示箭头尾部的节点，偏导数中的分母部分
	using ValueType = ValueTypePara;
	GradientEdge() : Base(),  headNode(nullptr), tailNode(nullptr), gradient() {}
	GradientEdge(HeadNodeType* headNodePara, TailNodeType* tailNodePara, ValueType gradientPara)
	    : headNode(headNodePara), tailNode(tailNodePara), gradient(gradientPara) {}
    ValueType getValue() { return gradient; }
	ValueType getValue() const { return gradient; }
  private:
    HeadNodeType* headNode;
	TailNodeType* tailNode;
    ValueType gradient;

};

template<typename HeadNodeTypePara, typename TailNodeTypePara, typename ValueTypePara>
struct traits<GradientEdge<HeadNodeTypePara, TailNodeTypePara, ValueTypePara>> {
	using HeadNodeType = HeadNodeTypePara;
	using TailNodeType = TailNodeTypePara;
	using ValueType = ValueTypePara;
};



}






#endif