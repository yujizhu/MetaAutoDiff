#include<iostream>
#include<Eigen>
#include<src/Core/SpecialNode.hpp>
#include<src/Operator/AddNode.hpp>
#include<src/Operator/MulNode.hpp>
#include<src/Core/CalcPartialDerivative.hpp>

template<unsigned int indexPara>
using INT = MetaAD::ValueNode<MetaAD::NullTypeNode, int, indexPara>;

//template<unsigned int indexPara>
//using DOUBLE = MetaAD::ValueNode<MetaAD::NullTypeNode, double, indexPara>;

template<unsigned int indexPara>
using MATRIXXD = MetaAD::ValueNode<MetaAD::NullTypeNode, MetaAD::ad_MatrixXd, indexPara>;

namespace MetaAD {
    template<unsigned int indexPara>
    using DOUBLE = NodeWrapper<ValueNode<MetaAD::NullTypeNode, double, indexPara>>;

    template<unsigned int indexPara>
    DOUBLE<indexPara> makeDouble(double value) {
        return NodeWrapper<ValueNode<MetaAD::NullTypeNode, double, indexPara>>(std::shared_ptr<MetaAD::ValueNode<MetaAD::NullTypeNode, double, indexPara>>(new MetaAD::ValueNode<MetaAD::NullTypeNode, double, indexPara>(value)));
    }
}

int main() {

    //MetaAD::ValueNode<MetaAD::NullTypeNode, double, 0>* p = new MetaAD::ValueNode<MetaAD::NullTypeNode, double, 0>(1);
    auto dVariable1 = MetaAD::makeDouble<0>(2);
    auto dVariable2 = MetaAD::makeDouble<1>(2);
    auto dVariable3 = MetaAD::makeDouble<2>(3);
    auto add1 = dVariable1 + dVariable2 + dVariable3;
    //std::cout << add1.getValue() << std::endl;
    auto derivative = add1.derivative<1>();
    //std::cout << derivative->getValue() << std::endl;
    auto mul1 = dVariable3 * dVariable2 * dVariable1;
    //std::cout << mul1.getValue() << std::endl;
    auto derivative1 = mul1.derivative<0>();
    //std::cout << derivative1->getValue() << std::endl;
    auto idendityNode1 = MetaAD::NodeWrapper<MetaAD::IdendityNode<5, double>>(std::shared_ptr<MetaAD::IdendityNode<5, double>>(new MetaAD::IdendityNode<5, double>(1)));
    auto zeroNode1 = MetaAD::NodeWrapper<MetaAD::ZeroNode<5, double>>(std::shared_ptr<MetaAD::ZeroNode<5, double>>(new MetaAD::ZeroNode<5, double>(1)));
    auto zeroNode2 = MetaAD::NodeWrapper<MetaAD::ZeroNode<4, double>>(std::shared_ptr<MetaAD::ZeroNode<4, double>>(new MetaAD::ZeroNode<4, double>(2)));
    auto zeroNode3 = MetaAD::NodeWrapper<MetaAD::ZeroNode<3, double>>(std::shared_ptr<MetaAD::ZeroNode<3, double>>(new MetaAD::ZeroNode<3, double>(3)));
    auto zeroAdd1 = zeroNode1 + zeroNode2;
    auto zeroAdd2 = zeroAdd1 + zeroNode3;
    //std::cout << zeroAdd2.getValue() << std::endl;
    auto add2 = zeroNode2 + zeroNode1;
    //std::cout << add2.getValue() << std::endl;
    auto mul2 = zeroNode1 * dVariable1 *dVariable2;
    //std::cout << mul2.getValue() << std::endl;
    //std::cout <<(std::is_same<decltype(mul2)::ConcreteNodeType, decltype(zeroNode1)::ConcreteNodeType>::value) << std::endl;
    auto mul3 = idendityNode1 * zeroNode1;
    std::cout << mul3.getValue() << std::endl;
    std::cout << (mul3.pNode == zeroNode1.pNode) << std::endl;
    auto add3 = zeroNode1 + dVariable1;
    std::cout << (std::is_same<decltype(zeroAdd1)::ConcreteNodeType, decltype(zeroNode1)::ConcreteNodeType>::value) << std::endl;
    std::cout << (add3.pNode == dVariable1.pNode) << std::endl;
    
    auto r1 = (dVariable1 + dVariable1 + dVariable1 + dVariable3 * dVariable2) * dVariable2;
    auto d1 = MetaAD::calcPartialDerivative_temp<1>(r1, dVariable2);
    std::cout << r1.getValue() << std::endl;
    std::cout << d1->getValue() << std::endl;

    



    int a;
    std::cin >> a;

}