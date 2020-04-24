/*
reference:https://zhuanlan.zhihu.com/p/36834302
*/

#include<Eigen>
#include<src/Algebra>
#include<fstream>
#include<iostream>
#include<vector>

double sigmoid(double x) {
    return 1 / (1 + std::exp(-x));
}

double grad_sigmoid(double x) {
    return sigmoid(x) * (1 - sigmoid(x));
}

std::vector<std::vector<std::string>> load_csv1(const std::string & path) 
{
    std::ifstream in;
    in.open(path);
    /*
    if (in) {
        std::cout << " open " << std::endl;
    }
    */
    std::string line;
    std::vector<std::vector<std::string>> values;
    unsigned int rows = 0;
    while (std::getline(in, line))
    {
        std::stringstream ss(line);
        std::string cell;
       // std::cout << "in" << std::endl;
       std::vector<std::string> temp;
       values.push_back(temp);
        while (std::getline(ss, cell, ',')) 
        {
            values[rows].push_back(cell);
        }
        ++rows;
    }

    return values;
}

int findIndex(std::vector<std::vector<std::string>>& rawDate, const std::string& key) {
    using CIterator = std::vector<std::string>::const_iterator;
    auto iterB = rawDate.at(0).cbegin();
    auto iterE = rawDate.at(0).cend();
    for (auto iter = iterB; iter != iterE; ++iter) {
        if (key == *iter)
            return iter - iterB;
    }
    return -1;
}

std::vector<int> findIndex(std::vector<std::vector<std::string>>& rawDate, const std::vector<std::string>& keyVec) {
    using CIterator = std::vector<std::string>::const_iterator;
    auto iterB = keyVec.cbegin();
    auto iterE = keyVec.cend();
    std::vector<int> index;
    for (auto iterKey = iterB; iterKey != iterE; ++iterKey) {
        index.push_back(findIndex(rawDate, *iterKey));
    }
    return index;
}



int main () {

    /*
    Eigen::MatrixXd t = Eigen::MatrixXd::Ones(3,2);
    t << 1,2,3,4,5,6;
    auto tt = MetaAD::makeMatrixXd<0>(t);
    auto log = MetaAD::log(tt);
    auto der = MetaAD::calcPartialDerivative<1>(log, tt);
    std::cout << der.compute() << std::endl;
    */
    


    Eigen::MatrixXd trainSet = Eigen::MatrixXd::Zero(20,1);
    Eigen::MatrixXd trainLabel = Eigen::MatrixXd::Zero(20,1);
    trainSet << 0.50,0.75,1.00,1.25,1.50,1.75,1.75,2.00,2.25,2.50,2.75,3.00,3.25,3.50,4.00,4.25,4.50,4.75,5.00,5.50;
    trainLabel << 0,0,0,0,0,0,1,0,1,0,1,0,1,0,1,1,1,1,1,1;
    
    /*// SGD
    Eigen::MatrixXd w = Eigen::MatrixXd::Zero(1,1);
    Eigen::MatrixXd b = Eigen::MatrixXd::Zero(1,1);

    auto w_p = MetaAD::makeMatrixXd<0>(w);
    auto b_p = MetaAD::makeMatrixXd<1>(b);
    auto x_v = MetaAD::makeMatrixXd<2>(trainSet.row(2).transpose());
    auto y_p = MetaAD::makeMatrixXd<3>(trainLabel.row(2));
    auto one_m = MetaAD::makeMatrixXd<4>(Eigen::MatrixXd::Ones(1, 1));
    auto one_y = MetaAD::makeMatrixXd<5>(Eigen::MatrixXd::Ones(1, 1));
    auto logistics= MetaAD::unaryExpr(w_p*x_v + b_p, &sigmoid, &grad_sigmoid);
    std::cout << logistics.compute() << std::endl;
    auto likelihood = MetaAD::log(logistics)*y_p + MetaAD::log(one_m - logistics)*(one_y - y_p);
    auto neg_one = MetaAD::makeMatrixXd<6>(-Eigen::MatrixXd::Ones(1,1));
    auto cost = neg_one*likelihood;

    auto der_w = MetaAD::calcPartialDerivative<1>(cost, w_p);
    auto der_b = MetaAD::calcPartialDerivative<1>(cost, b_p);

    for (int j = 0; j < 3000; j++) {
        for (int i = 0; i < 20; i++) {
            w = w_p.compute();
            w = w - 0.01*der_w.compute();
            b = b_p.compute();
            b = b - 0.01*der_b.compute();
            w_p.resetValue(w);
            b_p.resetValue(b);
            x_v.resetValue(trainSet.row(i).transpose());
            y_p.resetValue(trainLabel.row(i));
            std::cout << w_p.compute() << " " << b_p.compute() << std::endl;
        }
    }
    w = w_p.compute();
    b = b_p.compute();
    Eigen::MatrixXd W = Eigen::MatrixXd::Ones(1,2);
    W << w,b;
    Eigen::MatrixXd X = Eigen::MatrixXd::Ones(20,2);
    Eigen::MatrixXd X_add = Eigen::MatrixXd::Ones(20,1);
    X << trainSet, X_add;
    Eigen::MatrixXd A = W*X.transpose();
    for (int i = 0; i < 20; i ++) {
        std::cout << sigmoid(A(0,i) << std::endl;
    }
    */

    // BGD

    Eigen::MatrixXd w = Eigen::MatrixXd::Zero(1,2);
    auto w_p = MetaAD::makeMatrixXd<0>(w);
    Eigen::MatrixXd trainSetTemp = Eigen::MatrixXd::Ones(20,2);
    Eigen::MatrixXd train_add = Eigen::MatrixXd::Ones(20,1);
    trainSetTemp << trainSet, train_add;
    auto x_v = MetaAD::makeMatrixXd<2>(trainSetTemp.transpose());
    auto y_p = MetaAD::makeMatrixXd<3>(trainLabel);
    auto logistics= MetaAD::unaryExpr(w_p*x_v, &sigmoid, &grad_sigmoid);
    auto one_m = MetaAD::makeMatrixXd<4>(Eigen::MatrixXd::Ones(1, 20));
    auto one_y = MetaAD::makeMatrixXd<5>(Eigen::MatrixXd::Ones(20, 1));
    auto likelihood = MetaAD::log(logistics)*y_p + MetaAD::log(one_m - logistics)*(one_y - y_p);
    auto neg_one = MetaAD::makeMatrixXd<6>(-Eigen::MatrixXd::Ones(1,1));
    auto cost = neg_one*likelihood;

    auto der_w = MetaAD::calcPartialDerivative<1>(cost, w_p);
    for (int j = 0; j < 5000; j++) {
        w = w_p.compute();
        w = w - 0.01*der_w.compute();
        w_p.resetValue(w);
        std::cout << w_p.compute() << std::endl;
    }
    
    
    std::cout << logistics.compute() << std::endl;

    //std::cout << w_p.compute() << std::endl;
    //std::cout << der_b.compute() << std::endl;


    int forEnd;
    std::cin >> forEnd;

}