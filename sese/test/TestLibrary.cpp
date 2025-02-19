// Copyright 2024 libsese
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "sese/system/Library.h"
#include "sese/Log.h"

#include "gtest/gtest.h"

using Func = double(double);

TEST(TestLibrary, Sin) {
#ifdef _WIN32
    auto lib_name = "NTDLL.DLL";
#elif __linux__
    auto *lib_name = "libstdc++.so.6";
#elif __APPLE__
    auto *lib_name = "libstdc++.6.dylib";
#endif
    SESE_INFO("loading lib \"{}\"", lib_name);
    const auto OBJECT = sese::system::Library::create(lib_name);
    ASSERT_NE(OBJECT, nullptr);
    const auto SIN = OBJECT->findFunctionByNameAs<Func>("sin");
    ASSERT_NE(SIN, nullptr);

    const auto A = SIN(1.0f);
    const auto B = SIN(0.0f);
    SESE_INFO("sin(1.0f) = {}", A);
    SESE_INFO("sin(0.0f) = {}", B);
}

TEST(TestLibrary, Result) {
    if (auto result = sese::sys::Library::createEx("undef.dll")) {
        SESE_INFO("{}", result.err().message());
        return;
    }
    FAIL();
}