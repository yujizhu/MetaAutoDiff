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

#ifndef METAAUTODIFF_SPECIALTYPE_HPP
#define METAAUTODIFF_SPECIALTYPE_HPP

#include<iostream>
#include"MatrixWrapper.hpp"

namespace MetaAD {

// IdentityType does't support operator+.
template<typename ValueType>
class IdentityType;

template<>
class IdentityType<double> {
  public:
    IdentityType() : value(1) {}
    IdentityType(double valuePara) : value(1) {} 
    double getValue() {
        return value;
    }
    double getValue() const {
        return value;
    }
    operator double(){
        return 1;
    }
  private:
    double value;
};

template<>
class IdentityType<ad_MatrixXd> {
  public:
    IdentityType() : value(ad_MatrixXd::Identity(1,1)) {}
    IdentityType(const ad_MatrixXd& valuePara) : value(ad_MatrixXd::Identity(valuePara.rows(), valuePara.cols())) {} 
    ad_MatrixXd getValue() {
        return value;
    }
    ad_MatrixXd getValue() const {
        return value;
    }
    operator ad_MatrixXd(){
        return value;
    }
  private:
    ad_MatrixXd value;
};

template<typename ValueType>
class ZeroType;

template<>
class ZeroType<double> {
  public:
    ZeroType() : value(0) {}
    ZeroType(double valuePara) : value(0) {}
    double getValue() {
        return value;
    }
    operator double() const {
        return 0;
    }
  private:
    double value;
};

template<>
class ZeroType<ad_MatrixXd> {
  public:
    ZeroType() : value(ad_MatrixXd::Zero(1,1)) {}
    ZeroType(const ad_MatrixXd& valuePara) : value(ad_MatrixXd::Zero(valuePara.rows(), valuePara.cols())) {}
    ad_MatrixXd getValue() {
        return value;
    }
    operator ad_MatrixXd() {
        return value;
    }
  private:
    ad_MatrixXd value;
};

// 这里要不要限定T和ValueType为相同类型？
template<typename T, typename ValueType>
T operator+(const ZeroType<ValueType>& leftValue, const T& rightValue) {
    return rightValue;
}

template<typename T, typename ValueType>
T operator+(const T& leftValue, const ZeroType<ValueType>& rightValue) {
    return leftValue;
}

template<typename ValueType>
ZeroType<ValueType> operator+(const ZeroType<ValueType>& leftValue, const ZeroType<ValueType>& rightValue) {
    return ZeroType<ValueType>();
}

template<typename T, typename ValueType>
ZeroType<ValueType> operator*(const ZeroType<ValueType>& leftValue, const T& rightValue) {
    return ZeroType<ValueType>();
}

template<typename T, typename ValueType>
ZeroType<ValueType> operator*(const T& leftValue, const ZeroType<ValueType>& rightValue) {
    return ZeroType<ValueType>();
}

template<typename ValueType>
ZeroType<ValueType> operator*(const ZeroType<ValueType>& leftValue, const ZeroType<ValueType>& rightValue) {
    return ZeroType<ValueType>(leftValue.getValue() * rightValue.getValue());
}

template<typename T>
T operator*(const IdentityType<T>& leftValue, const T& rightValue) {
    return rightValue;
}

template<typename T>
T operator*(const T& leftValue, const IdentityType<T>& rightValue) {
    return leftValue;
}

template<typename ValueType>
auto operator*(const IdentityType<ValueType>& leftValue, const IdentityType<ValueType>& rightValue) {
    return IdentityType<ValueType>(leftValue.getValue() * rightValue.getValue());
}

/*
std::ostream & operator<<(std::ostream & a, ZeroType b) {
    std::cout << "Zero" << std::endl;
    return a;
}

std::ostream & operator<<(std::ostream & a, IdendityType b) {
    std::cout << "Idendity" << std::endl;
    return a;
}
*/
}
#endif