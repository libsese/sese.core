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

#pragma once

#include <sese/net/http/HttpServletContext.h>
#include <sese/net/http/Range.h>
#include <sese/io/File.h>
#include <sese/util/StopWatch.h>

#include <sese/internal/service/http/ConnType.h>

namespace sese::internal::service::http {

struct Handleable {
    using Ptr = std::shared_ptr<Handleable>;

    ConnType conn_type = ConnType::NONE;
    sese::net::http::Request request;
    sese::net::http::Response response;
    std::string content_type = "application/x-";
    io::File::Ptr file;
    size_t filesize = 0;
    std::vector<sese::net::http::Range> ranges;
    std::vector<sese::net::http::Range>::iterator range_iterator = ranges.begin();
    sese::net::IPAddress::Ptr remote_address{};
    bool keepalive = false;
    sese::StopWatch stopwatch;
};

}