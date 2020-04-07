# MetaAutoDif

[English](./README.md) | 简体中文

MetaAutoDiff是一个基于操作符重载的反向传播自动微分C++模板库。该模板库当前只支持一阶导数的计算，不支持前向传播。

## 依赖
- [Eigen](http://eigen.tuxfamily.org/index.php?title=Main_Page)

使用MetaAutoDiff需要包含Eigen's unsupported modules。

## 安装
MetaAutoDiff是一个纯模板元程序库，因此没有需要链接的二进制库文件，仅需要将头文件包含进所要进行的项目中即可。

## 数据类型支持
MetaAutoDiff当前仅支持double类型与ad_MatrixXd(内部实现为Eigen::MatrixXd)类型，其他类型可能会在未来进行支持。

## GPU
MetaAutoDiff当前不支持GPU加速计算。

## 已实现的操作符
MetaAutoDiff当前仅支持加、减、乘算子，其他算子将在未来逐步添加进该程序库中。