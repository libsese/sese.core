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

/// \file SpinLock.h
/// \brief Spin Lock
/// \author kaoru

#pragma once

#include <sese/Config.h>

#include <atomic>

#ifdef SESE_PLATFORM_WINDOWS
#pragma warning(disable : 4251)
#endif

namespace sese {
/// Spin Lock
class  SpinLock {
public:
    void lock();

    void unlock();

private:
    std::atomic_bool flag{false};
};
} // namespace sese