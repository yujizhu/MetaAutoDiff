#include<iostream>
#include<src/Core/util/Meta.hpp>
//#include<src/Operator/Add.hpp>
#include<src/Core/ValueNode.hpp>
#include<tuple>
#include<utility>
#include<type_traits>
#include<Eigen>
#include<src/Core/math/Add.hpp>
#include<src/Core/GradientEdge.hpp>
#include<src/Operator/AddNode.hpp>
#include<type_traits>
#include<src/Core/CalcPartialDerivative.hpp>


template<typename Derived>
class B{

};


template<typename Derived>
class D:public B<Derived> {

};

class C:public D<C>{

};

class E:public D<E> {	

};

template<typename Derived>
class N{

};

template<typename Derived>
class M: public N<Derived> {

};

template<typename T>
class F:public M<F<T>> {
  public:
};

class G:public M<G> {

};

template<typename T>
void test(M<T>& p) {
	std::cout<<"in test" << std::endl;

}

template<typename T, typename U>
int operator+(const N<T>& l, const N<U>& r) {
	std::cout<<"in operator 1" << std::endl;
	return 1;
}
namespace TempnameTest{
template<typename NodeType>
struct trait ;

template<typename NodeType>
struct trait<const NodeType> : trait<NodeType> {};


template<typename DerivedPara>
class GraphNodeBase {
  public:
  
    using Derived = DerivedPara;
	
    using ValueType = typename trait<DerivedPara>::ValueType;
	
  
};

//测试函数模板特化，用于policy中。

template<typename T,typename T1, typename T2>
struct TestFunFullSpe {
	template<unsigned int A> 
	static typename std::enable_if<MetaAD::internal::is_same_value<unsigned int, A, 1>::value, T>::type _derivative(T1 input1, T2 input2,T output) {
		return input1;
	}
    
	template<unsigned int A>
	static typename std::enable_if<MetaAD::internal::is_same_value<unsigned int, A, 2>::value, T>::type _derivative(T1 input1,T2 input2, T output) {
		return input2; 
	}
	

	//template<>
	//static auto _derivative<2>(int input, int input2, int output) {
	//	return input2;
	//x}
};

template<typename T1, typename T2>
struct testTrait {
	using type = double;
};

template<>
struct testTrait<int, int> {
	using type = char;
};


template<typename T1, typename T2, typename T3 = typename testTrait<T1,T2>::type>
struct policyTest;

template<>
struct policyTest<double, double> {
    static void test() {
		using T= testTrait<double, double>::type;
		T temp = 1;
		std::cout << "in policyTest double double:" << temp << std::endl;
	}
};

template<>
struct policyTest<int ,int> {
    static void test() {
		using T = testTrait<int, int>::type;
		T temp = 'a';
		std::cout << "in policyTest int int:" << temp << std::endl;
	}
};	

class NullType{};

template<typename ValueTypePara>
class NullTypeNode : public GraphNodeBase<NullTypeNode<ValueTypePara>> {
  public:
    using ValueType = ValueTypePara;
	using InputNodeTypes = NullType;
};


template<typename ValueTypePara>
struct trait<NullTypeNode<ValueTypePara>> {
	using ValueType = ValueTypePara;

};

using A = NullTypeNode<int>;


}



template<typename T>
void printEdgeByBase(const MetaAD::GraphEdgeBase<T>* edge) {
    std::cout <<"in printEdgeByBase function: " << edge->getValue() << std::endl;
}


template<typename T>
auto getDerivativeByBase(const MetaAD::GraphNodeBase<T>* node) {
    return node->template partialDerivative<0>();
}

// 尝试模板偏特化与模板实参结合




// logic_or测试
struct TrueType {
	static constexpr bool value = true;
};

struct FalseType {
	static constexpr bool value = false;
};

template<unsigned int indexPara>
using INT = MetaAD::ValueNode<MetaAD::NullTypeNode, int, indexPara>;

template<unsigned int indexPara>
using DOUBLE = MetaAD::ValueNode<MetaAD::NullTypeNode, double, indexPara>;

template<unsigned int indexPara>
using MATRIXXD = MetaAD::ValueNode<MetaAD::NullTypeNode, MetaAD::ad_MatrixXd, indexPara>;



int main() {
    std::cout << MetaAD::internal::is_base_of<B<C>,E>::value;
	F<int> f;
	G g;
	test(f);
	int q = f + g;
    /*
	typename TempnameTest::GraphNodeBase<TempnameTest::A>::ValueType zz = 1;
	std::cout << std::endl;
	std::cout << "test trati " << zz << std::endl;
	*/

    //using INT = MetaAD::ValueNode<MetaAD::NullTypeNode, int>;
	//using DOUBLE = MetaAD::ValueNode<MetaAD::NullTypeNode, double>;
	//using MATRIXXD = MetaAD::ValueNode<MetaAD::NullTypeNode, MetaAD::ad_MatrixXd>;
	DOUBLE<1> testValue1(1);
	DOUBLE<2> testValue2(2);
	std::cout << "test ValueNode: " << testValue1 << std::endl;

	std::cout << "test addtrait: " << std::endl;
	

    std::cout << std::is_same<decltype(std::declval<F<int>>() + std::declval<G>()), int&&>::value << std::endl;

    std::cout << "test is_same_value trait:" << std::endl;
	std::cout << MetaAD::internal::is_same_value<unsigned int, 1,1>::value << std::endl;
/*
    std::cout << "ValueNodeTest:"<<std::endl;
	MetaAD::ValueNode<int> ValueNodeO1;
	std::cout << ValueNodeO1 << std::endl;
*/

    std::cout << "test operator policy:" << std::endl;
    std::cout << TempnameTest::TestFunFullSpe<int, int ,int>::_derivative<2>(1,2,3) << std::endl;

    std::cout << "test policy specialization:" << std::endl;
	TempnameTest::policyTest<int, int>::test();

    

    std::cout << "Test Eigen:" << std::endl;
    Eigen::MatrixXd testM;
	testM = Eigen::MatrixXd::Identity(3,3);
	std::cout << testM << std::endl;


	std::cout << "Test Add.hpp:" << std::endl;
	Eigen::MatrixXd add1 = Eigen::MatrixXd::Ones(3,2);
	Eigen::MatrixXd add2 = Eigen::MatrixXd::Ones(3,2);
    Eigen::MatrixXd addRes = add1 + add2;
	std::cout << MetaAD::ad_math::add_derivative_policy<Eigen::MatrixXd, Eigen::MatrixXd>::derivative<1>(add1,add2,addRes) << std::endl;
	std::cout << MetaAD::ad_math::add_derivative_policy<double, double>::derivative<1>(1, 2, 3) << std::endl;
    std::cout << std::endl;
	std::cout << addRes << std::endl;

	std::cout << "Test GradientEdge.hpp:" << std::endl;
	MetaAD::GradientEdge<DOUBLE<1>, DOUBLE<2>, double> gradientEdgeTest(&testValue1, &testValue2, 123456);
    std::cout << gradientEdgeTest.getValue() << std::endl;
	printEdgeByBase(&gradientEdgeTest);
    

	std::cout << "Test AddNode.hpp" << std::endl;
	auto addNodeTest1 = MetaAD::add<3>(testValue1,  testValue2);
	MetaAD::GraphNodeBase<decltype(addNodeTest1)>* addpt= &addNodeTest1;
	std::cout << addpt->partialDerivative<0>() <<std::endl;
    std::cout << addNodeTest1.getValue() <<std::endl;
	std::cout << addNodeTest1.partialDerivative<0>() << " " << addNodeTest1.partialDerivative<1>() << std::endl;
    MATRIXXD<4> matrixNode1(add1);
	MATRIXXD<5> matrixNode2(add2);
	auto addNodeTest2 = MetaAD::add<6>(matrixNode1, matrixNode2);
	std::cout << addNodeTest2.getValue() << std::endl;
	std::cout << addNodeTest2.partialDerivative<0>() << std::endl;
	std::cout << std::endl;
	std::cout << addNodeTest2.partialDerivative<1>() << std::endl;
    

	std::cout << "Test decltype:" << std::endl;
	int testDecltype(2);
	C testC;
	E testE;
	std::tuple<C*, E*> testTuple(&testC,&testE);
	std::cout << std::is_same<decltype(testDecltype), int>::value << std::endl;
	std::cout << std::is_same<decltype(std::get<0>(testTuple)), C*&>::value << std::endl; //这个会输出true
	std::cout << std::is_same<MetaAD::internal::remove_cvref<decltype(std::get<0>(testTuple))>::type, C*>::value << std::endl;
	std::cout << std::is_same<std::tuple_element<1, std::tuple<C*, E*>>::type, E*>::value << std::endl;

	std::cout << "Test remove_pointer:" << std::endl;
	std::cout << std::is_same<std::tuple_element<1, std::tuple<C*, E*>>::type, E>::value << std::endl;
    std::cout << std::is_same<MetaAD::internal::remove_pointer<std::tuple_element<1, std::tuple<C*, E*>>::type>::type, E>::value << std::endl;

	std::cout << "Test logic_or:" << std::endl;
	std::cout << MetaAD::internal::logic_or<true, TrueType>::value << std::endl;
	std::cout << MetaAD::internal::logic_or<true, FalseType>::value << std::endl;
	std::cout << MetaAD::internal::logic_or<false, TrueType>::value << std::endl;
	std::cout << MetaAD::internal::logic_or<false, FalseType>::value << std::endl;
	std::cout << "Test has_dependence: " << std::endl;
	std::cout << std::is_same<decltype(addNodeTest2), MetaAD::AddNode<MATRIXXD<4>, MATRIXXD<5>, 6>>::value << std::endl;
	std::cout << MetaAD::has_dependence<decltype(addNodeTest2), decltype(matrixNode1)>::value <<std::endl;
	std::cout << MetaAD::has_dependence<decltype(addNodeTest1), decltype(matrixNode1)>::value <<std::endl;
    MATRIXXD<7> matrixNode3(Eigen::MatrixXd::Ones(3,2));
	auto addNodeTest3 = MetaAD::add<8>(matrixNode3, addNodeTest2);
	MATRIXXD<9> matrixNode4(Eigen::MatrixXd::Ones(3,2));
	auto addNodeTest4 = MetaAD::add<10>(matrixNode4, addNodeTest3);	
    std::cout << MetaAD::has_dependence<decltype(addNodeTest4), decltype(matrixNode4)>::value <<std::endl;
	std::cout << "Test has_dependence<NullTypeNode>: " << std::endl;
	std::cout << MetaAD::has_dependence<MetaAD::NullTypeNode, decltype(matrixNode3)>::value <<std::endl;
	std::cout << MetaAD::has_dependence<decltype(addNodeTest3), MetaAD::NullTypeNode>::value <<std::endl;
	std::cout << MetaAD::has_dependence<MetaAD::NullTypeNode, MetaAD::NullTypeNode>::value <<std::endl;

	std::cout << "Test partialDerivative By BaseNodeType " << std::endl;
    std::cout << getDerivativeByBase(&addNodeTest4) << std::endl;
    //Eigen::MatrixXf::Identity()*Eigen::MatrixXf::Identity();
	std::cout << "Test CalcPartialDerivative.hpp: " << std::endl;
	auto addNodeTest5 = MetaAD::add<11>(addNodeTest4, matrixNode1);
	auto addNodeTest6 = MetaAD::add<12>(addNodeTest5, matrixNode1);
	std::cout << MetaAD::calcPartialDerivative(&addNodeTest6, &matrixNode1);
	//std::cout << MetaAD::ZeroType::creat() << std::endl;
	//std::cout << std::endl;
	//MetaAD::ad_MatrixXd tttt = (MetaAD::IdendityType::creat()*MetaAD::ad_MatrixXd::Identity(6,6)+MetaAD::ZeroType::creat())*MetaAD::ad_MatrixXd::Identity(6,6)+MetaAD::ZeroType::creat();
    //std::cout << tttt << std::endl;
    /*
	double d(1);
	int qq(1);
    double* pd = &d;
	int* pqq = &qq;

	pqq == pd;
    */	
	
    int a;
    std::cin >> a;
}