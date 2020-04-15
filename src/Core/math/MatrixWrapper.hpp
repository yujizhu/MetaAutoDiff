

#ifndef METAAUTODIFF_MATRIXWRAPPER_H
#define METAAUTODIFF_MATRIXWRAPPER_H

#include<Eigen>
#include<KroneckerProduct>

namespace MetaAD {
    using ad_MatrixXd = Eigen::MatrixXd;

    ad_MatrixXd ad_kroneckerProduct(const ad_MatrixXd& leftOperand, const ad_MatrixXd& rightOperand) {
        ad_MatrixXd result = Eigen::kroneckerProduct(leftOperand, rightOperand);
        return result;
    }
    
    ad_MatrixXd ad_matTranspose(const ad_MatrixXd& operand) {
        ad_MatrixXd result = operand.transpose();
        return result;
    }
}




#endif