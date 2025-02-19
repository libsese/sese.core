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

#include <openssl/evp.h>
#include <openssl/x509.h>
#include <openssl/err.h>
#include <openssl/ssl.h>

#include <sese/security/SSLContext.h>
#include <sese/log/Marco.h>

#include <cassert>

using namespace sese::security;

SSLContext::SSLContext(const void *method) noexcept {
    context = SSL_CTX_new(static_cast<const SSL_METHOD *>(method));
}

SSLContext::~SSLContext() noexcept {
    if (context) {
        SSL_CTX_free(static_cast<SSL_CTX *>(context));
    }
}

void *SSLContext::release() noexcept {
    assert(context);
    auto rt = context;
    context = nullptr;
    return rt;
}

void *SSLContext::getContext() const noexcept {
    assert(context);
    return context;
}

bool SSLContext::importCertFile(const char *file) const noexcept {
    assert(context);
    return 1 == SSL_CTX_use_certificate_file(static_cast<SSL_CTX *>(context), file, SSL_FILETYPE_PEM);
}

bool SSLContext::importPrivateKeyFile(const char *file) const noexcept {
    assert(context);
    return 1 == SSL_CTX_use_PrivateKey_file(static_cast<SSL_CTX *>(context), file, SSL_FILETYPE_PEM);
}

bool SSLContext::authPrivateKey() const noexcept {
    assert(context);
    return 1 == SSL_CTX_check_private_key(static_cast<SSL_CTX *>(context));
}

// bool SSLContext::verifyAndLoad(const char *file) noexcept {
//     SSL_CTX_set_verify((SSL_CTX *) context, SSL_VERIFY_PEER, nullptr);
//     return 1 == SSL_CTX_load_verify_locations((SSL_CTX *) context, file, nullptr);
// }

sese::net::Socket::Ptr SSLContext::newSocketPtr(Socket::Family family, int32_t flags) {
    assert(context);
    return std::make_shared<SecuritySocket>(shared_from_this(), family, flags);
}

std::unique_ptr<SSLContext> SSLContext::copy() const noexcept {
    auto origin = static_cast<SSL_CTX *>(context);

    auto cert = SSL_CTX_get0_certificate(origin);
    auto key = SSL_CTX_get0_privatekey(origin);
    auto method = SSL_CTX_get_ssl_method(origin);

    if (cert == nullptr &&
        key == nullptr) {
        return nullptr;
    }

    auto new_ctx = SSL_CTX_new(method);
    auto new_cert = X509_dup(cert);
    auto new_key = EVP_PKEY_dup(key);

    SSL_CTX_use_certificate(new_ctx, new_cert);
    X509_free(new_cert);

    SSL_CTX_use_PrivateKey(new_ctx, new_key);
    EVP_PKEY_free(new_key);

    auto ssl_context = MAKE_UNIQUE_PRIVATE(SSLContext);
    ssl_context->context = new_ctx;
    return std::move(ssl_context);
}

sese::ErrorCode SSLContext::getErrorCode() noexcept {
    auto code = ERR_get_error();
    return ErrorCode(static_cast<int32_t>(code), ERR_error_string(code, nullptr));
}
