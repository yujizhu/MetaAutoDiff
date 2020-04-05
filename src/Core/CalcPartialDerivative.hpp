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

#ifndef METAAUTODIFF_CALCPARTIALDERIVATIVE_HPP
#define METAAUTODIFF_CALCPARTIALDERIVATIVE_HPP

#include<tuple>
#include"util/Meta.hpp"
#include"GraphNodeBase.hpp"
#include"GradientEdge.hpp"
#include"math/SpecialType.hpp"
#include"math/PartialDerivativeTrait.hpp"


namespace MetaAD {

// 判断两个节点之间是否具有依赖关系，这部分要求计算图中的任意两个节点的类型都要不同，本框架通过构造节点对象时的index参数来显式使每个节点的类型不同
// 前置声明
template<typename HeadNodeTypePara, typename TailNodeTypePara, unsigned int n>
struct has_dependence_imp;

template<typename HeadNodeTypePara, typename TailNodeTypePara, 
         bool legal = isNodeType<HeadNodeTypePara>::value && isNodeType<TailNodeTypePara>::value>
struct has_dependence {
    static constexpr bool value = false;
};

template<typename HeadNodeTypePara, typename TaliNodeTypePara>
struct has_dependence<HeadNodeTypePara, TaliNodeTypePara, true> {
    static constexpr bool value = has_dependence_imp<HeadNodeTypePara, TaliNodeTypePara,
	                              std::tuple_size<typename HeadNodeTypePara::InputNodeTypes>::value - 1>::value;
};

// 这个偏特化不知道要不要加,这个感觉不用加，因为在计算偏导数过程中，只会计算两个不同类型节点间的偏导数，因此不会产生这个调用
template<typename NodeTypePara>
struct has_dependence<NodeTypePara, NodeTypePara, true> { // 这里的true如果改成legal则会产生ambiguous,因为偏序规则可能无法判断
    static constexpr bool value = true;
};

template<typename TailNodeTypePara>
struct has_dependence<NullTypeNode, TailNodeTypePara, true> {
	static constexpr bool value = false;
};

template<typename HeadNodeTypePara>
struct has_dependence<HeadNodeTypePara, NullTypeNode, true> {
	static constexpr bool value = false;
};

template<>
struct has_dependence<NullTypeNode, NullTypeNode,true> {
	static constexpr bool value = false;
};

template<typename HeadNodeTypePara, typename TailNodeTypePara, unsigned int n>
struct has_dependence_imp {
    //static constexpr bool value = std::is_same<typename std::tuple_element<n, typename HeadNodeTypePara::InputNodeTypes>, TailNodeTypePara>::value
	//                           || std::has_dependence<typename std::tuple_element<n, typename HeadNodeTypePara::InputNodeTypes>, TailNodeType>::value
	//						   || has_dependence_imp<HeadNodeType, TailNodeTypePara, n-1>::value;
	// 使用了定义于internal中的logic_or，实现了静态逻辑短路，其具体调用方法与||运算符略有不同，使用时请注意.
	/* //下面的实现可行，但是代码有重复
    static constexpr bool value = internal::logic_or<internal::logic_or<std::is_same<typename internal::remove_pointer<typename std::tuple_element<n, typename HeadNodeTypePara::InputNodeTypes>>::type, TailNodeTypePara>::value, // 由于has_dependence定义了两个节点类型相同的偏特化，因此这一行和下一行的逻辑好像重复了
	                                                                    has_dependence<typename internal::remove_pointer<typename std::tuple_element<n, typename HeadNodeTypePara::InputNodeTypes>::type>::type, TailNodeTypePara>
																	   >::value, 
													 has_dependence_imp<HeadNodeTypePara, TailNodeTypePara, n-1>
													>::value;
	*/
    static constexpr bool value = internal::logic_or<has_dependence<typename internal::remove_pointer<typename std::tuple_element<n, typename HeadNodeTypePara::InputNodeTypes>::type>::type, TailNodeTypePara>::value,						   
													 has_dependence_imp<HeadNodeTypePara, TailNodeTypePara, n-1>
													>::value;
};

template<typename HeadNodeTypePara, typename TailNodeTypePara>
struct has_dependence_imp<HeadNodeTypePara, TailNodeTypePara, 0> {
    static constexpr bool value = internal::logic_or<std::is_same<typename std::tuple_element<0, typename HeadNodeTypePara::InputNodeTypes>, TailNodeTypePara>::value,
	                                                 has_dependence<typename internal::remove_pointer<typename std::tuple_element<0, typename HeadNodeTypePara::InputNodeTypes>::type>::type, TailNodeTypePara>
													>::value;

};

// ChainRuleExpansion
template<unsigned int n>
struct ChainRuleExpansion;

//对相同节点计算时
template<typename NodeTypePara>
typename std::enable_if<isNodeType<NodeTypePara>::value, IdendityType>::type
calcPartialDerivative(const GraphNodeBase<NodeTypePara>* fenzi, const GraphNodeBase<NodeTypePara>* fenmu) {
	//std::cout << "in 0" << std::endl;
    return IdendityType::creat();
}

template<typename FenziPara, typename FenmuPara>
typename std::enable_if<has_dependence<FenziPara, FenmuPara>::value,
                        typename ad_math::partial_derivative_trait<typename FenziPara::ValueType, typename FenmuPara::ValueType>::type
					   >::type
calcPartialDerivative(const GraphNodeBase<FenziPara>* fenzi, const GraphNodeBase<FenmuPara>* fenmu) {
	//std::cout << "in 1" << std::endl;
    auto result =  ChainRuleExpansion<std::tuple_size<typename FenziPara::InputNodeTypes>::value - 1
	                         >::expansion(fenzi->getDerivedPtr(), fenmu->getDerivedPtr());
	//std::cout << "in 1-1" << std::endl;
	//std::cout << result <<std::endl;
	return result;
}

template<typename FenziPara, typename FenmuPara>
typename std::enable_if<!has_dependence<FenziPara, FenmuPara>::value, ZeroType>::type
calcPartialDerivative(const GraphNodeBase<FenziPara>* fenzi, const GraphNodeBase<FenmuPara>* fenmu) {
	//std::cout << "in 3" << std::endl;
    return ZeroType::creat();
}

template<unsigned int n>
struct ChainRuleExpansion {
    template<typename FenziPara, typename FenmuPara>
	static typename ad_math::partial_derivative_trait<typename FenziPara::ValueType, typename FenmuPara::ValueType>::type
    expansion(const GraphNodeBase<FenziPara>* fenzi, const GraphNodeBase<FenmuPara>* fenmu) {
		//std::cout << "in 4" << std::endl;
		auto temp1 = (fenzi->template partialDerivative<n>());
		auto temp2 = calcPartialDerivative(std::get<n>(fenzi->getDerivedPtr()->inputs), fenmu);
		//std::cout << "in 4-1" << std::endl;
		//std::cout << "temp1: " << std::endl;
        //std::cout << temp1 << std::endl;
		//std::cout << "temp2: " << std::endl;
        //std::cout << temp2 << std::endl;
		auto temp3 = temp1*temp2;
		//std::cout << "temp3: " << std::endl;
        //std::cout << temp3 << std::endl;
		auto temp4 = ChainRuleExpansion<n-1>::expansion(fenzi, fenmu);
        //std::cout << "temp4: " << std::endl;
        //std::cout << temp4 << std::endl;
        auto result = temp3 + temp4; // 这里不使用temp1和temp2，直接使用原式子相乘会出错，为什么？   
		//std::cout << "in 4-2" << std::endl;
		//std::cout << result << std::endl;
		return result;
	}
};

template<>
struct ChainRuleExpansion<0> {
	template<typename FenziPara, typename FenmuPara>
	static typename std::conditional<has_dependence<typename internal::remove_pointer<typename std::tuple_element<0, typename FenziPara::InputNodeTypes>::type
	                                                                                 >::type, FenmuPara>::value, 
									 typename ad_math::partial_derivative_trait<typename FenziPara::ValueType, typename FenmuPara::ValueType>::type,
									 ZeroType>::type
	expansion(const GraphNodeBase<FenziPara>* fenzi, const GraphNodeBase<FenmuPara>* fenmu) {
		//std::cout << "in 5" << std::endl;
		auto temp1 = (fenzi->template partialDerivative<0>());
		auto temp2 = calcPartialDerivative(std::get<0>(fenzi->getDerivedPtr()->inputs), fenmu);
		auto result =  temp1*temp2;
		//std::cout << "in 5-1" << std::endl;
		//std::cout << result << std::endl;
		return result;
	}	
};

}

#endif