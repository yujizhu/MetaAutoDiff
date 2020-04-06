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

#ifndef METAAUTODIFF_META_HPP
#define METAAUTODIFF_META_HPP

namespace MetaAD {

namespace internal {
template<typename T, T leftParameter, T rightParameter>
struct is_same_value {
	static constexpr bool value = false;
};

template<typename T, T parameter>
struct is_same_value<T, parameter, parameter> {
	static constexpr bool value = true;
};


template<typename T>
struct remove_const {
    using type = T;
};

template<typename T>
struct remove_const<const T> {
    using type = T;
};

template<typename T>
struct remove_volatile {
	using type = T;
};

template<typename T>
struct remove_volatile<volatile T> {
	using type =T;
};

template<typename T>
struct remove_cv {
	using type = typename remove_volatile<typename remove_const<T>::type>::type;
};

template<typename T>
struct remove_ref {
	using type = T;
};

template<typename T>
struct remove_ref<T&> {
	using type = T;
};

template<typename T>
struct remove_ref<T&&> {
	using type = T;
};

template<typename T>
struct remove_cvref {
	using type = typename remove_cv<typename remove_ref<T>::type>::type;
};

template<typename T>
struct remove_pointer {
	using type = T;
};

template<typename T>
struct remove_pointer<T*> {
	using type = T;
};

template<typename T>
struct decay {
    using type = typename remove_cvref<T>::type;
};

// 第一个实参为或运算左侧值，为一个bool类型值，第二个形参为或运算右侧类型，它包含一个value成员，该成员为bool类型
// 该trait实现了具有编译器短路逻辑的或运算，但是调用方法与一般地||运算符不同，该trait第二个参数为一个包含bool类型
// value成员的类型。
template<bool leftValuePara, typename RightTypePara> 
struct logic_or {
    static constexpr bool value = true;
};

template<typename RightTypePara>
struct logic_or<false, RightTypePara> {
	static constexpr bool value = RightTypePara::value;
};



template<typename type>
struct is_class {
    
};

template<typename Base, typename Derived>
struct is_base_of_ {
    using yes = char[1];
    using no = char[2];

    template<typename HelperBase, typename HelperDerived> // 这里如果使用Base和Derived的话会报错
    struct helper {
        operator HelperBase*() const;
        operator HelperDerived*();
    };
    
    template<typename Placeholders>
    static yes& test(Derived*, Placeholders); //不能直接返回数组类型，应该返回数组类型的引用  这个函数必须时static的
    static no& test(Base*, int);   // 最好不用int，考虑一下用具体类型作为占位类型  这个函数必须时static的

    static constexpr bool value = (sizeof(test(helper<Base, Derived>(), 0)) == sizeof(yes));  //这里要调用helper的构造函数，不能直接用它的类型（即不能不加括号）
    
};

template<typename Base>
struct is_base_of_<Base, Base> {
    static constexpr bool value = true;
};

template<typename Base, typename Derived>
struct is_base_of {
	using DecayBase = typename decay<Base>::type;
	using DecayDerived = typename decay<Derived>::type;
	static constexpr bool value = is_base_of_<DecayBase, DecayDerived>::value;
};

template<typename NodeType>
struct NodeValueTrait {
    using type = typename NodeType::ValueType;
};


template<typename EdgeType>
struct EdgeValueTrait {
    using type = typename EdgeType::valueType;
};

template<typename>
struct EdgeHeadNodeTrait {

};

template<typename>
struct EdgeTailNodeTrait {

};





}

}


#endif