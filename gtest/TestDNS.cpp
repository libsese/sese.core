#include <sese/net/dns/DNSServer.h>
#include <sese/net/dns/DNSUtil.h>

#include <random>

#include <gtest/gtest.h>

TEST(TestDNS, Decode_0) {
    const uint8_t buffer[12] = {0xc1, 0xa8, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    sese::net::dns::FrameHeaderInfo info{};
    sese::net::dns::DNSUtil::decodeFrameHeaderInfo(buffer, info);

    EXPECT_EQ(info.transactionId, 0xc1a8);

    EXPECT_EQ(info.flags.QR, 0);
    EXPECT_EQ(info.flags.opcode, 0);
    EXPECT_EQ(info.flags.TC, 0);
    EXPECT_EQ(info.flags.RD, 1);
    EXPECT_EQ(info.flags.z, 0);

    EXPECT_EQ(info.questions, 1);
    EXPECT_EQ(info.answerPrs, 0);
    EXPECT_EQ(info.authorityPrs, 0);
    EXPECT_EQ(info.additionalPrs, 0);
}

TEST(TestDNS, Decode_1) {
    const uint8_t buffer[12] = {0xc1, 0xa8, 0x81, 0x83, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00};

    sese::net::dns::FrameHeaderInfo info{};
    sese::net::dns::DNSUtil::decodeFrameHeaderInfo(buffer, info);

    EXPECT_EQ(info.transactionId, 0xc1a8);

    EXPECT_EQ(info.flags.QR, 1);
    EXPECT_EQ(info.flags.opcode, 0);
    EXPECT_EQ(info.flags.TC, 0);
    EXPECT_EQ(info.flags.RD, 1);
    EXPECT_EQ(info.flags.RA, 1);
    EXPECT_EQ(info.flags.z, 0);
    EXPECT_EQ(info.flags.rcode, SESE_DNS_RCODE_NAME_ERROR);

    EXPECT_EQ(info.questions, 1);
    EXPECT_EQ(info.answerPrs, 0);
    EXPECT_EQ(info.authorityPrs, 1);
    EXPECT_EQ(info.additionalPrs, 0);
}

TEST(TestDNS, Encode_0) {
    const uint8_t expect[12] = {0x82, 0x67, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t result[12]{};

    sese::net::dns::FrameHeaderInfo info{};
    info.transactionId = 0x8267;

    info.flags.QR = 0;
    info.flags.opcode = 0;
    info.flags.TC = 0;
    info.flags.RD = 1;
    info.flags.z = 0;

    info.questions = 1;
    info.answerPrs = 0;
    info.authorityPrs = 0;
    info.additionalPrs = 0;

    sese::net::dns::DNSUtil::encodeFrameHeaderInfo(result, info);
    EXPECT_EQ(memcmp(expect, result, 12), 0);
}

TEST(TestDNS, Encode_1) {
    const uint8_t expect[12] = {0x82, 0x67, 0x81, 0x83, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00};
    uint8_t result[12]{};

    sese::net::dns::FrameHeaderInfo info{};
    info.transactionId = 0x8267;

    info.flags.QR = 1;
    info.flags.opcode = 0;
    info.flags.TC = 0;
    info.flags.RD = 1;
    info.flags.RA = 1;
    info.flags.z = 0;
    info.flags.rcode = SESE_DNS_RCODE_NAME_ERROR;

    info.questions = 1;
    info.answerPrs = 0;
    info.authorityPrs = 1;
    info.additionalPrs = 0;

    sese::net::dns::DNSUtil::encodeFrameHeaderInfo(result, info);
    EXPECT_EQ(memcmp(expect, result, 12), 0);
}

sese::net::IPv4Address::Ptr createAddress() {
    std::random_device device;
    auto engine = std::default_random_engine(device());
    std::uniform_int_distribution<uint16_t> dis(1025, 65535);
    auto port = dis(engine);
    printf("select port %d\n", (int) port);
    return sese::net::IPv4Address::create("127.0.0.1", 53);
}

TEST(TestDNS, Server_0) {
    auto ip = createAddress();

    sese::net::dns::DNSConfig config;
    config.address = ip;
    config.hostMap["www.example.com"] = "127.0.0.1";
    config.hostMap["www.kaoru.com"] = "192.168.3.230";

    auto server = sese::net::dns::DNSServer::create(&config);
    ASSERT_NE(server, nullptr);

    server->start();
    getchar();
    server->shutdown();
}