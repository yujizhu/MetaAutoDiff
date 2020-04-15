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

#ifndef METAAUTODIFF_MATTRANSPOSETRAIT_HPP
#define METAAUTODIFF_MATTRANSPOSETRAIT_HPP

#include"MatrixWrapper.hpp"

namespace MetaAD {

namespace ad_math {

    template<typename InputValueTypePara>
    struct mat_transpose_trait;
    
    template<>
    struct mat_transpose_trait<ad_MatrixXd> {
        using type = ad_MatrixXd;
    };
    
}

}

#endif