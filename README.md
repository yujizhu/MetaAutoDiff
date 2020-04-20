# MetaAutoDiff

English | [简体中文](./README_cn.md)

MetaAutoDiff is a C++ template library for automatic differentiation in reverse mode based on operator-overloading and expression templates. MetaAutoDiff supports any order derivative for scalar variable. For matrix operator, MetaAutoDiff only provide first and second order derivative now. This project doesn't provide forward mode now.

## Dependencies
- [Eigen](http://eigen.tuxfamily.org/index.php?title=Main_Page)

MetaAutoDiff also need Eigen's unsupported modules for Kronecker Product.

## Set Up
There is no binary library to link to, so if you want to use MetaAutoDiff, you can include the header file in your project.

## Date type Support
MetaAutoDiff only support double and ad::MatrixXd(internal implement type is Eigen::MatrixXd). More type support will be added in the future.

## GPU Support
MetaAutoDiff doesn't support GPU now.

## Implemented Operators
MetaAutoDiff only support addition, subtraction multiplication, exponential, trigonometric operator now, more operator will be supported in the future.

