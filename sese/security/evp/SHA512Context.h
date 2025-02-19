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

/**
 * @file SHA512Context.h
 * @author kaoru
 * @version 0.1
 * @brief EVP SHA512 algorithm context
 * @date September 13, 2023
 */

#pragma once

#include <sese/security/evp/Context.h>

namespace sese::security::evp {

/// EVP SHA512 algorithm context
class SHA512Context : public Context {
public:
    SHA512Context() noexcept;
    ~SHA512Context() noexcept override;
    void update(const void *buffer, size_t len) noexcept override;
    void final() noexcept override;
    void *getResult() noexcept override;
    size_t getLength() noexcept override;

private:
    size_t length = 64;
    uint8_t result[64]{};
#ifdef SESE_PLATFORM_WINDOWS
    uint64_t hProv = 0;
    uint64_t hHash = 0;
#else
    void *context = nullptr;
#endif
};
} // namespace sese::security::evp