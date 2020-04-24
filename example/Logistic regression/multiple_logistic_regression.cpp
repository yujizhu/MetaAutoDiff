/*
reference:https://blog.csdn.net/love__live1/article/details/79137458
          https://github.com/fenghaotong/MachineLearning/tree/master/LogisticRegression
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
    std::string empty = "?";
    while (std::getline(in, line))
    {
        std::stringstream ss(line);
        std::string cell;
        // std::cout << "in" << std::endl;
        std::vector<std::string> temp;
        bool discard = false;
        while (std::getline(ss, cell, ',')) 
        {   
            //std::cout << cell << std::endl;
            if (cell == empty) {
                discard = true;
            }
            else {

                temp.push_back(cell);
            }
        }
        if (discard)
            continue;
       // std::cout << temp[1]<<std::endl;
        values.push_back(temp);
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

    auto rawTrainSet = load_csv1("data.csv");
    std::cout << rawTrainSet.size() << " " << rawTrainSet[0].size() << std::endl;
    std::vector<double> dateVec;
    std::vector<double> dateLabelVec;
    auto iterB = rawTrainSet.cbegin();
    auto iterE = rawTrainSet.cend();
    for (auto iter = iterB; iter != iterE; ++iter) {
        double val = std::stod((*iter).at(1));
        dateVec.push_back(val);
        val = std::stod((*iter).at(2));
        dateVec.push_back(val);
        val = std::stod((*iter).at(3));
        dateVec.push_back(val);
        val = std::stod((*iter).at(4));
        dateVec.push_back(val);
        val = std::stod((*iter).at(5));
        dateVec.push_back(val);
        val = std::stod((*iter).at(6));
        dateVec.push_back(val);
        val = std::stod((*iter).at(7));
        dateVec.push_back(val);
        val = std::stod((*iter).at(8));
        dateVec.push_back(val);
        val = std::stod((*iter).at(9));
        dateVec.push_back(val);
        val = std::stod((*iter).at(10));
        if (val == 2) {
            dateLabelVec.push_back(0);
        }
        else if (val == 4) {
            dateLabelVec.push_back(1);
        }
    }
    
    //std::cout << dateVec.size() << " " << dateLabelVec.size() << std::endl;

    Eigen::MatrixXd copiedMatrix = Eigen::Map<Eigen::MatrixXd>(&dateVec[0], 9, 683); // MatrixXd默认按列存储
    Eigen::MatrixXd dateLabel = Eigen::Map<Eigen::MatrixXd>(&dateLabelVec[0], 683, 1);
    Eigen::MatrixXd date = copiedMatrix.transpose();
    //Eigen::MatrixXd date = dateT.block(0,0,10000,10);
    //std::cout << dateLabel.array().mean() << std::endl;

    //Eigen::RowVectorXd max = date.colwise().maxCoeff();
    //Eigen::RowVectorXd min = date.colwise().minCoeff();
    //std::cout << max << std::endl;
    //std::cout << min << std::endl;
    //std::cout << dateLabel << std::endl;
    

    /*  //SGD
    Eigen::MatrixXd trainDate = date.block(0,0,400,9);
    Eigen::MatrixXd trainLabel = dateLabel.block(0,0,400,1);
    Eigen::MatrixXd testDate = date.block(400,0,283,9);
    Eigen::MatrixXd testLabel = dateLabel.block(400,0,283,1);


    Eigen::MatrixXd w = Eigen::MatrixXd::Zero(1,9);
    Eigen::MatrixXd b = Eigen::MatrixXd::Zero(1,1);
    

    auto w_p = MetaAD::makeMatrixXd<0>(w);
    auto b_p = MetaAD::makeMatrixXd<1>(b);
    auto x_v = MetaAD::makeMatrixXd<2>(trainDate.row(0).transpose());
    auto y_p = MetaAD::makeMatrixXd<3>(trainLabel.row(0));
    auto one_m = MetaAD::makeMatrixXd<4>(Eigen::MatrixXd::Ones(1, 1));
    auto one_y = MetaAD::makeMatrixXd<5>(Eigen::MatrixXd::Ones(1, 1));
    auto logistics= MetaAD::unaryExpr(w_p*x_v + b_p, &sigmoid, &grad_sigmoid);
    auto likelihood = MetaAD::log(logistics)*y_p + MetaAD::log(one_m - logistics)*(one_y - y_p);
    auto neg_one = MetaAD::makeMatrixXd<6>(-Eigen::MatrixXd::Ones(1,1));
    auto cost = neg_one*likelihood;

    auto der_w = MetaAD::calcPartialDerivative<1>(cost, w_p);
    auto der_b = MetaAD::calcPartialDerivative<1>(cost, b_p);
    for (int j = 0; j < 1000; j++) {
        for (int i = 0; i < 400; i++) {
            w = w_p.compute();
            w = w - 0.01*der_w.compute();
            b = b_p.compute();
            b = b - 0.01*der_b.compute();
            w_p.resetValue(w);
            b_p.resetValue(b);
            x_v.resetValue(trainDate.row(i).transpose());
            y_p.resetValue(trainLabel.row(i));
            std::cout << w_p.compute() << " " << b_p.compute() << std::endl;
        }
    }
    
    Eigen::MatrixXd bb = b_p.compute();
    double bbb = bb(0,0);
    
    //w << 0.517282,0.17409,0.345136,0.497475,-0.0137871,0.510653,0.238218,0.110922,0.27281; 
    //double bbb = -9.47746;
    Eigen::MatrixXd temp = w_p.compute() * testDate.transpose();
    std::cout << temp.cols() << " " << temp.rows() << std::endl;
    Eigen::MatrixXd det = (temp + Eigen::MatrixXd::Ones(1, 283)*bbb) - testLabel.transpose();
    std::cout << det.transpose() << std::endl;
    int count = 0;
    for (int i = 0; i < 283; i++) {
        if ( ((det(0, i) < 0) && (testLabel(i, 0) == 0))
         ||  ((det(0, i) > 0) && (testLabel(i, 0) == 1))
           )
           count++;
    }
    std::cout << double(count)/283 << std::endl;
    */
    

    
    
    //BGD
    Eigen::MatrixXd dateT = Eigen::MatrixXd::Ones(683, 1);
    Eigen::MatrixXd dateR = Eigen::MatrixXd::Ones(683, 10);
    dateR << date, dateT;
    Eigen::MatrixXd trainDate = dateR.block(0,0,400,10);
    Eigen::MatrixXd trainLabel = dateLabel.block(0,0,400,1);
    Eigen::MatrixXd testDate = dateR.block(400,0,283,10);
    Eigen::MatrixXd testLabel = dateLabel.block(400,0,283,1);
    
    Eigen::MatrixXd w = Eigen::MatrixXd::Zero(1,10);
    //w << 0.526938,-0.0727235,0.339262,0.45329,0.112257,0.371966,0.237853,0.147191,0.438972,-9.0147;
    auto w_p = MetaAD::makeMatrixXd<0>(w);
    auto x_v = MetaAD::makeMatrixXd<2>(trainDate.block(0,0,40,10).transpose());
    auto y_p = MetaAD::makeMatrixXd<3>(trainLabel.block(0,0,40,1));
    auto logistics= MetaAD::unaryExpr(w_p*x_v, &sigmoid, &grad_sigmoid);
    auto one_m = MetaAD::makeMatrixXd<4>(Eigen::MatrixXd::Ones(1, 40));
    auto one_y = MetaAD::makeMatrixXd<5>(Eigen::MatrixXd::Ones(40, 1));
    auto likelihood = MetaAD::log(logistics)*y_p + MetaAD::log(one_m - logistics)*(one_y - y_p);
    auto neg_one = MetaAD::makeMatrixXd<6>(-Eigen::MatrixXd::Ones(1,1));
    auto cost = neg_one*likelihood;
    auto der_w = MetaAD::calcPartialDerivative<1>(cost, w_p);
    //std::cout << der_w.compute() << std::endl;
    
    for (int j = 0; j < 6000; j++) {
        for (int i = 0; i < 10; i++) {
            x_v.resetValue(trainDate.block(i*40 , 0, 40, 10).transpose());
            y_p.resetValue(trainLabel.block(i*40 , 0, 40, 1));
            //std::cout << "at here" << std::endl;
            w = w_p.compute();
            w = w - 0.001*der_w.compute();
            w_p.resetValue(w);
            //std::cout << w_p.compute() << std::endl;
        }
        
    }
    
    Eigen::MatrixXd temp = w_p.compute() * testDate.transpose();
    //std::cout << temp.cols() << " " << temp.rows() << std::endl;
    Eigen::MatrixXd det = (temp) - testLabel.transpose();
    std::cout << det.transpose() << std::endl;
    int count = 0;
    for (int i = 0; i < 283; i++) {
        if ( ((det(0, i) < 0) && (testLabel(i, 0) == 0))
         ||  ((det(0, i) > 0) && (testLabel(i, 0) == 1))
           )
           count++;
    }
    std::cout << double(count)/283 << std::endl;
    
    int forEnd;
    std::cin >> forEnd;


}