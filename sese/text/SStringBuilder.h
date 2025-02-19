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

/// \file SStringBuilder.h
/// \date 2022-9-27
/// \version 0.1
/// \author kaoru
/// \brief Includes SStringBuilder

#pragma once

#include <sese/Config.h>
#include <sese/text/SString.h>

namespace sstr {

/// SStringBuilder
class SESE_DEPRECATED_WITH("please use sese::text::StringBuilder") SStringBuilder final {
public:
    SStringBuilder(const SStringBuilder &builder);     // NOLINT
    SStringBuilder(SStringBuilder &&builder) noexcept; // NOLINT
    explicit SStringBuilder(size_t buffer_size);
    ~SStringBuilder();

public:
    [[nodiscard]] const uint32_t *data() const;
    [[nodiscard]] size_t size() const;
    [[nodiscard]] size_t cap() const;

    [[nodiscard]] bool null() const;
    [[nodiscard]] bool emtpy() const;

    bool reserve(size_t size);
    void trim();
    void reverse();
    int32_t find(const char *str) const;
    [[nodiscard]] int32_t find(const SStringView &str) const;
    void append(const char *str);
    void append(const SStringView &str);

    // Not supported
    // std::vector<SString> split(const char *str) const;
    // std::vector<SString> split(const SString &str) const;

    void clear();

    [[nodiscard]] SChar at(size_t index) const;
    void set(size_t index, SChar ch);
    void remove(size_t index);
    void remove(size_t begin, size_t len);
    void substring(size_t begin);
    void substring(size_t begin, size_t len);
    void insert(size_t index, SChar ch);
    void insert(size_t index, const char *str);
    void insert(size_t index, const SStringView &str);
    void replace(size_t begin, size_t len, const char *str);
    void replace(size_t begin, size_t len, const SStringView &str);

    [[nodiscard]] SString toString() const;

private:
    /// Data pointer
    uint32_t *_data = nullptr;
    /// Number of characters
    size_t _size = 0;
    /// Capacity (unit is uint32_t, i.e., 4 bytes)
    size_t _cap = 0;
};

} // namespace sstr