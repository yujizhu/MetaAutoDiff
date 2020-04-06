# MetaAutoDiff
MetaAutoDiff is a C++ template library for automatic differentiation in reverse mode based on operator-overloading. MetaAutoDiff only supports first order derivative now. This project doesn't provide forward mode now.

## Dependencies
- [Eigne](http://eigen.tuxfamily.org/index.php?title=Main_Page)

MetaAutoDiff also need Eigen's unsupported modules for Kronecker Product.

## Set Up
There is no binary library to link to, so if you want to use MetaAutoDiff, you can include the header file in your project.

## Date type Support
MetaAutoDiff only support double and ad::MatrixXd(internal implement type is Eigen::MatrixXd). More type support will be added in the future.

## GPU Support
MetaAutoDiff doesn't support GPU now.

## Implemented Operators
MetaAutoDiff only support addition and multiplication operator now, more operator will be supported in the future.

