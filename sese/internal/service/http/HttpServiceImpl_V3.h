#pragma once

#include <sese/service/http/HttpService_V3.h>
#include <sese/service/iocp/IOBuf.h>
#include <sese/net/http/Range.h>
#include <sese/io/ByteBuilder.h>
#include <sese/io/File.h>

#include <asio.hpp>
#include <asio/ssl/stream.hpp>

#include <optional>

namespace sese::internal::service::http::v3 {

struct HttpConnection;
struct HttpConnectionImpl;
struct HttpSSLConnectionImpl;
class HttpServiceImpl;

struct HttpConnection {
    using Ptr = std::shared_ptr<HttpConnection>;

    enum class ConnType {
        NORMAL,
        FILE_DOWNLOAD
    } conn_type = ConnType::NORMAL;

    HttpConnection(const std::shared_ptr<HttpServiceImpl> &service, asio::io_context &context);
    virtual ~HttpConnection() = default;

    net::http::Request request;
    net::http::Response response;
    asio::ip::tcp::socket socket;

    bool keepalive = false;
    asio::system_timer timer;
    void reset();

    std::string content_type = "application/x-";
    size_t filesize = 0;
    char send_buffer[MTU_VALUE]{};
    size_t expect_length;
    size_t real_length;
    io::File::Ptr file;
    std::vector<net::http::Range> ranges;
    std::vector<net::http::Range>::iterator range_iterator = ranges.begin();

    std::shared_ptr<HttpServiceImpl> service;
};

struct HttpConnectionImpl final : HttpConnection, std::enable_shared_from_this<HttpConnectionImpl> {
    HttpConnectionImpl(const std::shared_ptr<HttpServiceImpl> &service, asio::io_context &context);

    io::ByteBuilder parse_buffer;
    asio::streambuf asio_dynamic_buffer;

    /// 写入块函数，此函数会确保写完所有的缓存，出现意外则连接断开
    /// @param buffer 缓存指针
    /// @param length 缓存大小
    /// @param callback 完成回调函数
    void writeBlock(const char *buffer, size_t length, const std::function<void(const asio::error_code &code)> &callback);

    void readHeader();
    void readBody();
    void handleRequest();
    void writeHeader();
    void writeBody();
    void writeSingleRange();
    void writeRanges();
    void checkKeepalive();
};

struct HttpSSLConnectionImpl final : HttpConnection, std::enable_shared_from_this<HttpSSLConnectionImpl> {
    HttpSSLConnectionImpl(const std::shared_ptr<HttpServiceImpl> &service, asio::io_context &context);

    std::unique_ptr<asio::ssl::stream<asio::ip::tcp::socket &>> stream;
    bool is0x0d = false;
    iocp::IOBuf io_buffer;
    io::ByteBuilder dynamic_buffer;

    void readHeader();
    void readBody();
    void handleRequest();
    void writeHeader();
    void writeBody();
    void checkKeepalive();
};

class HttpServiceImpl final : public sese::service::http::v3::HttpService, public std::enable_shared_from_this<HttpServiceImpl> {
public:
    HttpServiceImpl();

    bool startup() override;
    bool shutdown() override;
    int getLastError() override;
    uint32_t getKeepalive() const {
        return keepalive;
    }

    void handleRequest(const HttpConnection::Ptr &conn);

private:
    asio::io_context io_context;
    std::optional<asio::ssl::context> ssl_context;
    asio::ip::tcp::acceptor acceptor;
    asio::error_code error;

    void handleAccept();
    void handleSSLAccept();
};

} // namespace sese::internal::service::http::v3