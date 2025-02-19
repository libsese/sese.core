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

#include <sese/net/Socket.h>
#include <sese/log/Marco.h>
#include <sese/net/http/HeaderBuilder.h>
#include <sese/net/http/RequestHeader.h>
#include <sese/net/http/UrlHelper.h>
#include <sese/net/http/HttpUtil.h>
#include <sese/net/http/Range.h>
#include <sese/net/http/RequestParser.h>
#include <sese/io/InputBuffer.h>
#include <sese/io/OutputBuffer.h>

#include <gtest/gtest.h>

using namespace sese;

TEST(TestHttp, UrlHelper_0) {
    net::http::Url info("file:///C:/vcpkg/vcpkg.exe?ssl=enable&token=123456&");
    EXPECT_EQ(info.getProtocol(), "file");
    EXPECT_EQ(info.getHost(), "");
    EXPECT_EQ(info.getUrl(), "/C:/vcpkg/vcpkg.exe");
    EXPECT_EQ(info.getQuery(), "?ssl=enable&token=123456&");
}

TEST(TestHttp, UrlHelper_1) {
    net::http::Url info("https://www.example.com/index.html?");
    EXPECT_EQ(info.getProtocol(), "https");
    EXPECT_EQ(info.getHost(), "www.example.com");
    EXPECT_EQ(info.getUrl(), "/index.html");
    EXPECT_EQ(info.getQuery(), "?");
}

TEST(TestHttp, UrlHelper_2) {
    net::http::Url info("https://www.example.com/index.html");
    EXPECT_EQ(info.getProtocol(), "https");
    EXPECT_EQ(info.getHost(), "www.example.com");
    EXPECT_EQ(info.getUrl(), "/index.html");
    EXPECT_EQ(info.getQuery(), "");
}

TEST(TestHttp, UrlHelper_3) {
    net::http::Url info("https://localhost:8080");
    EXPECT_EQ(info.getProtocol(), "https");
    EXPECT_EQ(info.getHost(), "localhost:8080");
    EXPECT_EQ(info.getUrl(), "/");
    EXPECT_EQ(info.getQuery(), "");
}

TEST(TestHttp, UrlHelper_4) {
    net::http::Url info("https://localhost:8080?a=b");
    EXPECT_EQ(info.getProtocol(), "https");
    EXPECT_EQ(info.getHost(), "localhost:8080");
    EXPECT_EQ(info.getUrl(), "/");
    EXPECT_EQ(info.getQuery(), "?a=b");
}

// Test root escape
TEST(TestHttp, RequestHeader_root) {
    auto req = net::http::RequestHeader();
    req.setQueryArg("ssl", "enable");
    req.setQueryArg("user", "root");
    req.setQueryArg("pwd", "0x7c00");
    EXPECT_EQ(req.getUrl(), "/?pwd=0x7c00&ssl=enable&user=root");
}

// Test multi-level directory escaping
TEST(TestHttp, RequestHeader_dirs) {
    auto req = net::http::RequestHeader();
    req.setUri("/你好233/index");
    EXPECT_EQ(req.getUrl(), "/%E4%BD%A0%E5%A5%BD233/index");
}

// Query parameters are escaped
TEST(TestHttp, RequestHeader_args) {
    auto req = net::http::RequestHeader();
    req.setUrl("/?key1=&%e4%bd%a0%e5%a5%bd233=value2");
    EXPECT_EQ(req.getQueryArg("key1", ""), "");
    EXPECT_EQ(req.getQueryArg("你好233", ""), "value2");
}

TEST(TestHttp, RequestHeader_args_error) {
    auto req = net::http::RequestHeader();
    EXPECT_NO_THROW(req.setUrl("/?&"));
    EXPECT_TRUE(req.queryArgsEmpty());
}

TEST(TestHttp, Header_misc) {
    using namespace sese::net::http;

    Header header;
    HeaderBuilder(header)
            .set("Key-A", "Value-A")
            .set("Key-B", "Value-B")
            .set("Key-C", "Value-C");

    for (auto &[name, value]: header) {
        SESE_INFO("{}: {}", name, value);
    }

    EXPECT_TRUE(header.exist("Key-A"));
    EXPECT_FALSE(header.exist("Key-D"));
    EXPECT_EQ("Value-A", header.get("Key-A"));
    EXPECT_FALSE(header.empty());
    header.clear();
    EXPECT_TRUE(header.empty());
}

/// Illegal HTTP header ending
TEST(TestHttpUtil, GetLine_0) {
    auto str = "GET / HTTP/1.1";
    auto input = io::InputBufferWrapper(str, strlen(str));

    net::http::RequestHeader req;
    ASSERT_FALSE(sese::net::http::HttpUtil::recvRequest(&input, &req));
}

/// HTTP headers that exceed the single-line length limit
TEST(TestHttpUtil, GetLine_1) {
    char buffer[HTTP_MAX_SINGLE_LINE + 1]{};
    auto input = io::InputBufferWrapper(buffer, sizeof(buffer));

    net::http::RequestHeader req;
    ASSERT_FALSE(sese::net::http::HttpUtil::recvRequest(&input, &req));
}

/// Illegal FirstLine
TEST(TestHttpUtil, RecvRequest_0) {
    auto str = "GET / HTTP/1.1 Hello\r\n";
    auto input = io::InputBufferWrapper(str, strlen(str));

    net::http::RequestHeader req;
    ASSERT_FALSE(sese::net::http::HttpUtil::recvRequest(&input, &req));
}

/// \brief Invalid HTTP version
TEST(TestHttpUtil, RecvRequest_1) {
    auto str = "GET / HTTP/0.9\r\n\r\n";
    auto input = io::InputBufferWrapper(str, strlen(str));

    net::http::RequestHeader req;
    ASSERT_TRUE(sese::net::http::HttpUtil::recvRequest(&input, &req));
    ASSERT_EQ(req.getVersion(), net::http::HttpVersion::VERSION_UNKNOWN);
}

/// \brief Failed to receive field
TEST(TestHttpUtil, RecvRequest_2) {
    auto str = "GET / HTTP/1.1\r\n"
               "Version: 0.0.1";
    auto input = io::InputBufferWrapper(str, strlen(str));

    net::http::RequestHeader req;
    ASSERT_FALSE(sese::net::http::HttpUtil::recvRequest(&input, &req));
}

TEST(TestHttpUtil, RecvRequest_3) {
    auto str = "GET / HTTP/1.1\r\n"
               "Version: 0.0.1\r\n"
               "\r\n";
    auto input = io::InputBufferWrapper(str, strlen(str));

    net::http::RequestHeader req;
    ASSERT_TRUE(sese::net::http::HttpUtil::recvRequest(&input, &req));
    ASSERT_EQ(req.getVersion(), net::http::HttpVersion::VERSION_1_1);
}

TEST(TestHttpUtil, SendRequest_0) {
    char buffer[1024]{};
    auto output = io::OutputBufferWrapper(buffer, sizeof(buffer));

    net::http::RequestHeader req{
            {"Host", "www.example.com"},
            {"Version", "0.0.1"}
    };
    ASSERT_TRUE(sese::net::http::HttpUtil::sendRequest(&output, &req));
}

/// \brief Failed to send field
TEST(TestHttpUtil, SendRequest_1) {
    char buffer[18]{};
    auto output = io::OutputBufferWrapper(buffer, sizeof(buffer));

    net::http::RequestHeader req{
            {"Host", "www.example.com"},
            {"Version", "0.0.1"}
    };
    ASSERT_FALSE(sese::net::http::HttpUtil::sendRequest(&output, &req));
}

/// \brief Invalid HTTP header ending
TEST(TestHttpUtil, RecvResponse_0) {
    auto str = "HTTP/1.1 200";
    auto input = io::InputBufferWrapper(str, strlen(str));

    net::http::ResponseHeader resp;
    ASSERT_FALSE(sese::net::http::HttpUtil::recvResponse(&input, &resp));
}

/// \brief Invalid HTTP version
TEST(TestHttpUtil, RecvResponse_1) {
    auto str = "HTTP/0.1 200\r\n\r\n";
    auto input = io::InputBufferWrapper(str, strlen(str));

    net::http::ResponseHeader resp;
    ASSERT_TRUE(sese::net::http::HttpUtil::recvResponse(&input, &resp));
    ASSERT_EQ(resp.getVersion(), net::http::HttpVersion::VERSION_UNKNOWN);
}

/// \brief Error receiving field
TEST(TestHttpUtil, RecvResponse_2) {
    auto str = "HTTP/1.1 200\r\n"
               "Version: 0.0.1";
    auto input = io::InputBufferWrapper(str, strlen(str));

    net::http::ResponseHeader resp;
    ASSERT_FALSE(sese::net::http::HttpUtil::recvResponse(&input, &resp));
}

TEST(TestHttpUtil, RecvResponse_3) {
    auto str = "HTTP/1.1 200\r\n"
               "Version: 0.0.1\r\n"
               "\r\n";
    auto input = io::InputBufferWrapper(str, strlen(str));

    net::http::ResponseHeader resp;
    ASSERT_TRUE(sese::net::http::HttpUtil::recvResponse(&input, &resp));
    ASSERT_EQ(resp.getVersion(), net::http::HttpVersion::VERSION_1_1);
}

/// \brief Invalid HTTP version
TEST(TestHttpUtil, SendResponse_0) {
    char buffer[1024]{};
    auto output = io::OutputBufferWrapper(buffer, sizeof(buffer));

    net::http::ResponseHeader resp{};
    resp.setVersion(net::http::HttpVersion::VERSION_UNKNOWN);
    ASSERT_FALSE(sese::net::http::HttpUtil::sendResponse(&output, &resp));
}

/// \brief Failed to send field
TEST(TestHttpUtil, SendResponse_1) {
    char buffer[18]{};
    auto output = io::OutputBufferWrapper(buffer, sizeof(buffer));

    net::http::ResponseHeader resp{
            {"Host", "www.example.com"},
            {"Version", "0.0.1"}
    };
    ASSERT_FALSE(sese::net::http::HttpUtil::sendResponse(&output, &resp));
}

/// \brief Failed to send version
TEST(TestHttpUtil, SendResponse_2) {
    char buffer[5]{};
    auto output = io::OutputBufferWrapper(buffer, sizeof(buffer));

    net::http::ResponseHeader resp{};
    ASSERT_FALSE(sese::net::http::HttpUtil::sendResponse(&output, &resp));
}

TEST(TestHttpUtil, SendResponse_3) {
    char buffer[1024]{};
    auto output = io::OutputBufferWrapper(buffer, sizeof(buffer));

    net::http::ResponseHeader resp{
            {"Host", "www.example.com"},
            {"Version", "0.0.1"}
    };
    ASSERT_TRUE(sese::net::http::HttpUtil::sendResponse(&output, &resp));
}

TEST(TestHttpCookie, RecvRequestCookie) {
    auto str = "GET / HTTP/1.1\r\n"
               "Cookie: id=123; name=hello=xxx; user=foo\r\n"
               "\r\n";
    auto input = io::InputBufferWrapper(str, strlen(str));
    net::http::RequestHeader req;
    ASSERT_TRUE(sese::net::http::HttpUtil::recvRequest(&input, &req));

    ASSERT_EQ(req.getCookies()->size(), 2);
    for (auto &[name, value]: *req.getCookies()) {
        SESE_INFO("cookie name: {}, value: {}", name, value->getValue());
    }
}

TEST(TestHttpCookie, RecvResponseCookie) {
    auto str = "HTTP/1.1 200 OK\r\n"
               "Set-Cookie: "
               "id=bar; "
               "HttpOnly; "
               "Expires=Wed, 21 Oct 2015 07:28:00 GMT; "
               "Secure; "
               "Domain=www.example.com; "
               "Path=/\r\n"
               "Set-Cookie: "
               "name=foo; "
               "undef=undef; "
               "max-age=114514\r\n"
               "\r\n";
    auto input = io::InputBufferWrapper(str, strlen(str));

    net::http::ResponseHeader resp;
    ASSERT_TRUE(sese::net::http::HttpUtil::recvResponse(&input, &resp));

    ASSERT_EQ(resp.getCookies()->size(), 2);
    for (auto &[name, value]: *resp.getCookies()) {
        SESE_INFO("cookie name: {}, value: {}", name, value->getValue());
    }
}

TEST(TestHttpCookie, SendRequestCookie) {
    char buffer[1024]{};
    auto output = io::OutputBufferWrapper(buffer, sizeof(buffer));

    auto cookie_map = std::make_shared<net::http::CookieMap>();
    {
        auto cookie = std::make_shared<net::http::Cookie>("id", "foo");
        cookie->setValue("bar");
        cookie_map->add(cookie);
    }

    {
        auto cookie = std::make_shared<net::http::Cookie>("token", "123456");
        cookie_map->add(cookie);
    }

    net::http::RequestHeader resp{};
    resp.setCookies(cookie_map);

    ASSERT_TRUE(sese::net::http::HttpUtil::sendRequest(&output, &resp));
}

TEST(TestHttpCookie, SendResponseCookie) {
    char buffer[1024]{};
    auto output = io::OutputBufferWrapper(buffer, sizeof(buffer));

    auto cookie_map = std::make_shared<net::http::CookieMap>();
    {
        auto cookie = std::make_shared<net::http::Cookie>("id", "foo");
        cookie->setValue("bar");
        cookie->setDomain("www.example.com");
        cookie->setHttpOnly(true);
        cookie->setMaxAge(10);
        cookie->setPath("/docs");
        cookie->setSecure(true);
        cookie_map->add(cookie);
    }

    {
        auto cookie = std::make_shared<net::http::Cookie>("token", "123456");
        cookie->setExpires(1690155929);
        cookie_map->add(cookie);
    }

    net::http::ResponseHeader resp;
    resp.setCookies(cookie_map);

    ASSERT_TRUE(sese::net::http::HttpUtil::sendResponse(&output, &resp));
}

TEST(TestHttpRange, Parse_0) {
    auto ranges = net::http::Range::parse("bytes=200-1000, 2000-6576, 19000-", 20000);
    ASSERT_EQ(ranges.size(), 3);

    EXPECT_EQ(ranges[0].begin, 200);
    EXPECT_EQ(ranges[0].len, 801);


    EXPECT_EQ(ranges[1].begin, 2000);
    EXPECT_EQ(ranges[1].len, 4577);


    EXPECT_EQ(ranges[2].begin, 19000);
    EXPECT_EQ(ranges[2].len, 1000);
}

TEST(TestHttpRange, Parse_1) {
    auto ranges = net::http::Range::parse("bytes=", 20000);
    EXPECT_TRUE(ranges.empty());
}

TEST(TestHttpRange, Parse_2) {
    auto ranges = net::http::Range::parse("block=1200-", 20000);
    EXPECT_TRUE(ranges.empty());
}

TEST(TestHttpRange, Parse_3) {
    auto ranges = net::http::Range::parse("bytes=1200-2000-3000", 20000);
    EXPECT_TRUE(ranges.empty());
}

TEST(TestHttpRange, Parse_4) {
    auto ranges = net::http::Range::parse("bytes=1200-3000", 100);
    EXPECT_TRUE(ranges.empty());
}

TEST(TestHttpRange, Parse_5) {
    auto ranges = net::http::Range::parse("bytes=1-3000", 100);
    EXPECT_TRUE(ranges.empty());
}

TEST(TestHttpRange, Parse_6) {
    auto ranges = net::http::Range::parse("bytes=99-1", 100);
    EXPECT_TRUE(ranges.empty());
}

TEST(TestHttpRange, toString) {
    auto ranges = net::http::Range::parse("bytes=200-1000", 2000)[0];
    auto len = ranges.toStringLength(2000);
    EXPECT_EQ(len, ranges.toString(2000).length());
}

TEST(TestRequestParser, HostWithPort) {
    {
        auto result = net::http::RequestParser::parse("https://127.0.0.1:7890/");
        ASSERT_NE(result.address, nullptr);
        EXPECT_EQ(result.address->getPort(), 7890);
    }
    {
        auto result = net::http::RequestParser::parse("https://127.0.0.1:7890a/");
        ASSERT_EQ(result.address, nullptr);
    }
    {
        auto result = net::http::RequestParser::parse("https://127.0.0.1:7890:5678/");
        ASSERT_EQ(result.address, nullptr);
    }
}