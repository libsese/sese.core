/**
 * @file EncodingConverter.h
 * @author kaoru
 * @date 2022年4月15日
 * @brief 字符串编码转换
 */
#pragma once
#include "sese/Config.h"
#include "sese/NotInstantiable.h"

#ifdef _WIN32
#pragma warning(disable : 4996)
#pragma warning(disable : 4624)
#endif

namespace sese {

    /**
     * @brief 字符串编码转换
     */
    class API EncodingConverter : NotInstantiable {
    public:
        static std::string toString(const std::wstring &wstring) noexcept;
        static std::wstring toWstring(const std::string &string) noexcept;
    };
}