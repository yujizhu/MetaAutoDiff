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
// 
class IdendityType {
  public:
    IdendityType() = default;
    static IdendityType creat() { return IdendityType{}; } 
};

class ZeroType {
  public:
    ZeroType() = default;
    static ZeroType creat() { return ZeroType{}; }
};

template<typename T>
T operator+(const ZeroType& leftValue, const T& rightValue) {
    return rightValue;
}

template<typename T>
T operator+(const T& leftValue, const ZeroType& rightValue) {
    return leftValue;
}

ZeroType operator+(const ZeroType& leftValue, const ZeroType& rightValue) {
    return ZeroType::creat();
}

template<typename T>
ZeroType operator*(const ZeroType& leftValue, const T& rightValue) {
    return ZeroType::creat();
}

template<typename T>
ZeroType operator*(const T& leftValue, const ZeroType& rightValue) {
    return ZeroType::creat();
}

ZeroType operator*(const ZeroType& leftValue, const ZeroType& rightValue) {
    return ZeroType::creat();
}

template<typename T>
T operator*(const IdendityType& leftValue, const T& rightValue) {
    return rightValue;
}

template<typename T>
T operator*(const T& leftValue, const IdendityType& rightValue) {
    return leftValue;
}

IdendityType operator*(const IdendityType& leftValue, const IdendityType& rightValue) {
    return IdendityType::creat();
}


std::ostream & operator<<(std::ostream & a, ZeroType b) {
    std::cout << "Zero" << std::endl;
    return a;
}

std::ostream & operator<<(std::ostream & a, IdendityType b) {
    std::cout << "Idendity" << std::endl;
    return a;
}








// IdendityType does't support operator+.
template<typename ValueType>
class IdendityType_TMP;

template<>
class IdendityType_TMP<double> {
  public:
    IdendityType_TMP() : value(1) {}
    IdendityType_TMP(double valuePara) : value(1) {} 
    double getValue() {
        return value;
    }
    operator double(){
        return 1;
    }
  private:
    double value;
};

template<>
class IdendityType_TMP<ad_MatrixXd> {
  public:
    IdendityType_TMP() : value(ad_MatrixXd::Identity(1,1)) {}
    IdendityType_TMP(const ad_MatrixXd valuePara) : value(ad_MatrixXd::Identity(valuePara.rows(), valuePara.cols())) {} 
    ad_MatrixXd getValue() {
        return value;
    }
    operator ad_MatrixXd(){
        return value;
    }
  private:
    ad_MatrixXd value;
};

template<typename ValueType>
class ZeroType_TMP;

template<>
class ZeroType_TMP<double> {
  public:
    ZeroType_TMP() : value(0) {}
    ZeroType_TMP(double valuePara) : value(0) {}
    double getValue() {
        return value;
    }
    operator double() {
        return 0;
    }
  private:
    double value;
};

template<>
class ZeroType_TMP<ad_MatrixXd> {
  public:
    ZeroType_TMP() : value(ad_MatrixXd::Zero(1,1)) {}
    ZeroType_TMP(const ad_MatrixXd& valuePara) : value(ad_MatrixXd::Zero(valuePara.rows(), valuePara.cols())) {}
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
T operator+(const ZeroType_TMP<ValueType>& leftValue, const T& rightValue) {
    return rightValue;
}

template<typename T, typename ValueType>
T operator+(const T& leftValue, const ZeroType_TMP<ValueType>& rightValue) {
    return leftValue;
}

template<typename ValueType>
ZeroType_TMP<ValueType> operator+(const ZeroType_TMP<ValueType>& leftValue, const ZeroType_TMP<ValueType>& rightValue) {
    return ZeroType_TMP<ValueType>();
}

template<typename T, typename ValueType>
ZeroType_TMP<ValueType> operator*(const ZeroType_TMP<ValueType>& leftValue, const T& rightValue) {
    return ZeroType_TMP<ValueType>();
}

template<typename T, typename ValueType>
ZeroType_TMP<ValueType> operator*(const T& leftValue, const ZeroType_TMP<ValueType>& rightValue) {
    return ZeroType_TMP<ValueType>();
}

template<typename ValueType>
ZeroType_TMP<ValueType> operator*(const ZeroType_TMP<ValueType>& leftValue, const ZeroType_TMP<ValueType>& rightValue) {
    return ZeroType_TMP<ValueType>(leftValue.getValue() * rightValue.getValue());
}

template<typename T>
T operator*(const IdendityType_TMP<T>& leftValue, const T& rightValue) {
    return rightValue;
}

template<typename T>
T operator*(const T& leftValue, const IdendityType_TMP<T>& rightValue) {
    return leftValue;
}

template<typename ValueType>
IdendityType operator*(const IdendityType_TMP<ValueType>& leftValue, const IdendityType_TMP<ValueType>& rightValue) {
    return IdendityType_TMP<ValueType>(leftValue.getValue() * rightValue.getValue());
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