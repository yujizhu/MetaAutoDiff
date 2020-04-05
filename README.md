# MetaAutoDiff
MetaAutoDiff is a C++ template library for automatic differentiation in reverse mode.This project doesn't provide forward mode or higher order derivatives now.

## Dependencies
- [Eigne](http://eigen.tuxfamily.org/index.php?title=Main_Page)

## Set Up
There is no binary library to link to, so if you want to use MetaAutoDiff, you can include the header file in your project.

## Date type Support
MetaAutoDiff only support MetaAD::DOUBLE<n>(internal implement type is double) and MetaAD::MATRIXXD<n>(internal implement type is Eigen::MatrixXd). More type support will be added in the future.

## GPU Support
MetaAutoDiff doesn't support GPU now.

## Implemented Operators
MetaAutoDiff only support add operator now, more operator will be supported in the future.

