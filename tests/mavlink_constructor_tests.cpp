#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "MAVLinkEndpoint.h"
#include "MockPrint.h"
#include "MockStream.h"
#include "mavlink/ardupilotmega/mavlink.h"
#include "test_utils.h"

using namespace testing;
using namespace std;

class MAVLinkEndpointConstructorTests : 
public ::testing::TestWithParam<tuple<uint32_t, uint32_t, uint32_t> >
{
public:

    MAVLinkEndpointConstructorTests() { 
        writer_call_count = 0; 
        p = new MockPrint();  
        seed = get<0>(GetParam()) * get<1>(GetParam()) * get<2>(GetParam());
    }

    virtual ~MAVLinkEndpointConstructorTests() { delete(p); }

    static size_t test_writer(uint8_t *data, size_t len)
    {
        writer_call_count++;
        return p->write(data, len);
    }
    
    static int          writer_call_count;
    static MockPrint    *p;
    MockStream          s;
    MockPrint           d;
    uint32_t            seed;
};

int MAVLinkEndpointConstructorTests::writer_call_count = 0;
MockPrint *MAVLinkEndpointConstructorTests::p = nullptr;

TEST_P(MAVLinkEndpointConstructorTests, TwoArgTest) 
{
    EXPECT_CALL(*p, write(_)).Times(0);
    EXPECT_CALL(s, write(_)).Times(0);
    auto p = GetParam();
    vector<int16_t> data = generateArbitraryIntData<int16_t>(&seed, 2, -20, 300);
    MAVLinkEndpoint mav(data[0], data[1]);
    if (isBounded<int16_t>(data[0], 0, 255))
    {
        EXPECT_EQ(mav.getComponentID(), data[0]); 
    } else 
    {
        EXPECT_NE(mav.getComponentID(), data[0]);
    }
    if (isBounded<int16_t>(data[1], 0, 255))
    {
        EXPECT_EQ(mav.getSystemID(), data[1]); 
    } else 
    {
        EXPECT_NE(mav.getSystemID(), data[1]);
    }
    EXPECT_FALSE(mav.poll());
    EXPECT_FALSE(mav.registerRXCallback(nullptr));
    EXPECT_FALSE(mav.registerRXDefaultCallback(nullptr));
    EXPECT_FALSE(mav.registerTXCallback(nullptr));
    EXPECT_FALSE(mav.registerRXStream(nullptr));
    EXPECT_FALSE(mav.registerTXPrint(nullptr));
    EXPECT_FALSE(mav.registerDebugPrint(nullptr));
    EXPECT_FALSE(mav.process(nullptr, 100));
    EXPECT_NO_THROW(mav.process(nullptr, 100));
    EXPECT_EQ(nullptr, mav.getDebugPrint());
    EXPECT_EQ(nullptr, mav.getTXPrint());
    EXPECT_EQ(nullptr, mav.getRXStream());
    EXPECT_EQ(0, mav.transmit(nullptr));
    mavlink_message_t testmsg;
    EXPECT_EQ(0, mav.transmit(&testmsg));
    EXPECT_EQ(writer_call_count, 0);
}

TEST_P(MAVLinkEndpointConstructorTests, RegisterIOTests)
{
    const int msg_size = 21;
    EXPECT_CALL(*p, write(_))
        .Times(2)
        .WillRepeatedly(Return(msg_size));
    EXPECT_CALL(s, write(_))
        .Times(0);
    vector<uint8_t> data = generateArbitraryIntData<uint8_t>(&seed, 6);
    MAVLinkEndpoint mav(data[0], data[1]);
    EXPECT_EQ(nullptr, mav.getDebugPrint());
    EXPECT_EQ(nullptr, mav.getTXPrint());
    EXPECT_EQ(nullptr, mav.getRXStream());
    EXPECT_TRUE(mav.registerWriter(test_writer));
    mavlink_message_t msg;
    mavlink_msg_heartbeat_pack(data[0], data[1], &msg, data[2], data[3], data[4], seed, data[5]); 
    EXPECT_EQ(msg_size, mav.transmit(&msg));
    EXPECT_EQ(nullptr, mav.getDebugPrint());
    EXPECT_EQ(nullptr, mav.getTXPrint());
    EXPECT_EQ(nullptr, mav.getRXStream());
    EXPECT_EQ(writer_call_count, 1);
    EXPECT_TRUE(mav.registerRXStream(&s));
    EXPECT_TRUE(mav.registerDebugPrint(&d));
    EXPECT_TRUE(mav.registerTXPrint(p));
    EXPECT_EQ(p, mav.getTXPrint());
    EXPECT_EQ(&s, mav.getRXStream());
    EXPECT_EQ(&d, mav.getDebugPrint());
    EXPECT_EQ(msg_size, mav.transmit(&msg));
    EXPECT_EQ(writer_call_count, 1);
}

TEST_P(MAVLinkEndpointConstructorTests, ThreeArgsWriter) {
    const int msg_size = 21;
    EXPECT_CALL(*p, write(_))
        .Times(1)
        .WillOnce(Return(msg_size));
    vector<uint8_t> data = generateArbitraryIntData<uint8_t>(&seed, 6);
    MAVLinkEndpoint mav(data[0], data[1], &MAVLinkEndpointConstructorTests::test_writer);
    EXPECT_EQ(mav.getComponentID(), data[0]); 
    EXPECT_EQ(mav.getSystemID(), data[1]); 
    EXPECT_FALSE(mav.poll());
    EXPECT_FALSE(mav.registerRXCallback(nullptr));
    EXPECT_FALSE(mav.registerRXDefaultCallback(nullptr));
    EXPECT_FALSE(mav.registerTXCallback(nullptr));
    EXPECT_FALSE(mav.registerRXStream(nullptr));
    EXPECT_FALSE(mav.registerTXPrint(nullptr));
    EXPECT_FALSE(mav.registerDebugPrint(nullptr));
    EXPECT_FALSE(mav.process(nullptr, 100));
    EXPECT_NO_THROW(mav.process(nullptr, 100));
    EXPECT_EQ(nullptr, mav.getDebugPrint());
    EXPECT_EQ(nullptr, mav.getTXPrint());
    EXPECT_EQ(nullptr, mav.getRXStream());
    EXPECT_EQ(0, mav.transmit(nullptr));
    EXPECT_EQ(writer_call_count, 0);

    mavlink_message_t msg;
    uint8_t msgbuf[MAVLINK_MAX_PACKET_LEN];
    vector<uint8_t> msgvec;
    mavlink_msg_heartbeat_pack(data[0], data[1], &msg, data[2], data[3], data[4], seed, data[5]); 
    uint16_t msgbuf_len = mavlink_msg_to_send_buffer(msgbuf, &msg);
    for (int i = 0; i < msgbuf_len; i++) { msgvec.push_back(msgbuf[i]); }
    EXPECT_EQ(msg_size, mav.transmit(&msg));
    EXPECT_EQ(writer_call_count, 1);
    EXPECT_THAT(p->write_data, ContainerEq(msgvec));
}

TEST_P(MAVLinkEndpointConstructorTests, ThreeArgsPrint) {
    const int msg_size = 21;
    EXPECT_CALL(*p, write(_))
        .Times(1)
        .WillOnce(Return(msg_size));
    vector<uint8_t> data = generateArbitraryIntData<uint8_t>(&seed, 6);
    MAVLinkEndpoint mav(data[0], data[1], p);
    EXPECT_EQ(mav.getComponentID(), data[0]); 
    EXPECT_EQ(mav.getSystemID(), data[1]); 
    EXPECT_FALSE(mav.poll());
    EXPECT_FALSE(mav.registerRXCallback(nullptr));
    EXPECT_FALSE(mav.registerRXDefaultCallback(nullptr));
    EXPECT_FALSE(mav.registerTXCallback(nullptr));
    EXPECT_FALSE(mav.registerRXStream(nullptr));
    EXPECT_FALSE(mav.registerTXPrint(nullptr));
    EXPECT_FALSE(mav.registerDebugPrint(nullptr));
    EXPECT_FALSE(mav.process(nullptr, 100));
    EXPECT_NO_THROW(mav.process(nullptr, 100));
    EXPECT_EQ(nullptr, mav.getDebugPrint());
    EXPECT_NE(nullptr, mav.getTXPrint());
    EXPECT_EQ(nullptr, mav.getRXStream());
    EXPECT_EQ(0, mav.transmit(nullptr));
    EXPECT_EQ(writer_call_count, 0);

    mavlink_message_t msg;
    uint8_t msgbuf[MAVLINK_MAX_PACKET_LEN];
    vector<uint8_t> msgvec;
    mavlink_msg_heartbeat_pack(data[0], data[1], &msg, data[2], data[3], data[4], seed, data[5]); 
    uint16_t msgbuf_len = mavlink_msg_to_send_buffer(msgbuf, &msg);
    for (int i = 0; i < msgbuf_len; i++) { msgvec.push_back(msgbuf[i]); }
    EXPECT_EQ(msg_size, mav.transmit(&msg));
    EXPECT_EQ(writer_call_count, 0);
    EXPECT_THAT(p->write_data, ContainerEq(msgvec));
}

TEST_P(MAVLinkEndpointConstructorTests, ThreeArgsStream) {
    const int msg_size = 21;
    EXPECT_CALL(s, write(_))
        .Times(1)
        .WillOnce(Return(msg_size));
    EXPECT_CALL(*p, write(_))
        .Times(0);
    vector<uint8_t> data = generateArbitraryIntData<uint8_t>(&seed, 6);
    MAVLinkEndpoint mav(data[0], data[1], &s);
    EXPECT_EQ(mav.getComponentID(), data[0]); 
    EXPECT_EQ(mav.getSystemID(), data[1]);
    EXPECT_FALSE(mav.poll());
    EXPECT_FALSE(mav.registerRXCallback(nullptr));
    EXPECT_FALSE(mav.registerRXDefaultCallback(nullptr));
    EXPECT_FALSE(mav.registerTXCallback(nullptr));
    EXPECT_FALSE(mav.registerRXStream(nullptr));
    EXPECT_FALSE(mav.registerTXPrint(nullptr));
    EXPECT_FALSE(mav.registerDebugPrint(nullptr));
    EXPECT_FALSE(mav.process(nullptr, 100));
    EXPECT_NO_THROW(mav.process(nullptr, 100));
    EXPECT_EQ(nullptr, mav.getDebugPrint());
    EXPECT_NE(nullptr, mav.getTXPrint());
    EXPECT_NE(nullptr, mav.getRXStream());
    EXPECT_EQ(mav.getTXPrint(), mav.getRXStream());
    EXPECT_EQ(0, mav.transmit(nullptr));
    EXPECT_EQ(writer_call_count, 0);

    // test that we can write out through the stream
    mavlink_message_t msg;
    uint8_t msgbuf[MAVLINK_MAX_PACKET_LEN];
    vector<uint8_t> msgvec;
    mavlink_msg_heartbeat_pack(data[0], data[1], &msg, data[2], data[3], data[4], seed, data[5]); 
    uint16_t msgbuf_len = mavlink_msg_to_send_buffer(msgbuf, &msg);
    for (int i = 0; i < msgbuf_len; i++) { msgvec.push_back(msgbuf[i]); }
    EXPECT_EQ(msg_size, mav.transmit(&msg));
    EXPECT_EQ(writer_call_count, 0);
    EXPECT_THAT(s.write_data, ContainerEq(msgvec));

    // check that we can poll through the stream
    EXPECT_FALSE(mav.poll());
    for (int i = 0; i < msgbuf_len; i++) { s.stream_data.push(msgbuf[i]); }
    EXPECT_TRUE(mav.poll());

}

TEST_P(MAVLinkEndpointConstructorTests, FourArgs) {
    const int msg_size = 21;
    EXPECT_CALL(*p, write(_))
        .Times(1)
        .WillOnce(Return(msg_size));
    vector<uint8_t> data = generateArbitraryIntData<uint8_t>(&seed, 6);
    MAVLinkEndpoint mav(data[0], data[1], &s, p);
    EXPECT_EQ(mav.getComponentID(), data[0]); 
    EXPECT_EQ(mav.getSystemID(), data[1]);
    EXPECT_FALSE(mav.poll());
    EXPECT_FALSE(mav.registerRXCallback(nullptr));
    EXPECT_FALSE(mav.registerRXDefaultCallback(nullptr));
    EXPECT_FALSE(mav.registerTXCallback(nullptr));
    EXPECT_FALSE(mav.registerRXStream(nullptr));
    EXPECT_FALSE(mav.registerTXPrint(nullptr));
    EXPECT_FALSE(mav.registerDebugPrint(nullptr));
    EXPECT_FALSE(mav.process(nullptr, 100));
    EXPECT_NO_THROW(mav.process(nullptr, 100));
    EXPECT_EQ(nullptr, mav.getDebugPrint());
    EXPECT_NE(nullptr, mav.getTXPrint());
    EXPECT_NE(nullptr, mav.getRXStream());
    EXPECT_NE(mav.getTXPrint(), mav.getRXStream());
    EXPECT_EQ(0, mav.transmit(nullptr));
    EXPECT_EQ(writer_call_count, 0);

    // test that we can write out through the stream
    mavlink_message_t msg;
    uint8_t msgbuf[MAVLINK_MAX_PACKET_LEN];
    vector<uint8_t> msgvec;
    mavlink_msg_heartbeat_pack(data[0], data[1], &msg, data[2], data[3], data[4], seed, data[5]); 
    uint16_t msgbuf_len = mavlink_msg_to_send_buffer(msgbuf, &msg);
    for (int i = 0; i < msgbuf_len; i++) { msgvec.push_back(msgbuf[i]); }
    EXPECT_EQ(msg_size, mav.transmit(&msg));
    EXPECT_EQ(writer_call_count, 0);
    EXPECT_THAT(p->write_data, ContainerEq(msgvec));

    // check that we can poll through the stream
    EXPECT_FALSE(mav.poll());
    for (int i = 0; i < msgbuf_len; i++) { s.stream_data.push(msgbuf[i]); }
    EXPECT_TRUE(mav.poll());
};

TEST_P(MAVLinkEndpointConstructorTests, FiveArgs) {
    const int msg_size = 21;
    EXPECT_CALL(*p, write(_))
        .Times(1)
        .WillOnce(Return(msg_size));
    EXPECT_CALL(d, println(Matcher<const String&>(_)))
        .Times(AtLeast(1));
    EXPECT_CALL(d, print(Matcher<const char *>(_)))
        .Times(AtLeast(1));
    EXPECT_CALL(d, println(Matcher<uint32_t>(_),Matcher<int>(_)))
        .Times(AtLeast(1));
    
    vector<uint8_t> data = generateArbitraryIntData<uint8_t>(&seed, 6);
    MAVLinkEndpoint mav(data[0], data[1], &s, p, &d);
    EXPECT_EQ(mav.getComponentID(), data[0]); 
    EXPECT_EQ(mav.getSystemID(), data[1]);
    EXPECT_FALSE(mav.poll());
    EXPECT_FALSE(mav.registerRXCallback(nullptr));
    EXPECT_FALSE(mav.registerRXDefaultCallback(nullptr));
    EXPECT_FALSE(mav.registerTXCallback(nullptr));
    EXPECT_FALSE(mav.registerRXStream(nullptr));
    EXPECT_FALSE(mav.registerTXPrint(nullptr));
    EXPECT_FALSE(mav.registerDebugPrint(nullptr));
    EXPECT_FALSE(mav.process(nullptr, 100));
    EXPECT_NO_THROW(mav.process(nullptr, 100));
    EXPECT_EQ(&d, mav.getDebugPrint());
    EXPECT_EQ(p, mav.getTXPrint());
    EXPECT_EQ(&s, mav.getRXStream());
    EXPECT_NE(mav.getTXPrint(), mav.getRXStream());
    EXPECT_EQ(0, mav.transmit(nullptr));
    EXPECT_EQ(writer_call_count, 0);

    // test that we can write out through the stream
    mavlink_message_t msg;
    uint8_t msgbuf[MAVLINK_MAX_PACKET_LEN];
    vector<uint8_t> msgvec;
    mavlink_msg_heartbeat_pack(data[0], data[1], &msg, data[2], data[3], data[4], seed, data[5]); 
    uint16_t msgbuf_len = mavlink_msg_to_send_buffer(msgbuf, &msg);
    for (int i = 0; i < msgbuf_len; i++) { msgvec.push_back(msgbuf[i]); }
    EXPECT_EQ(msg_size, mav.transmit(&msg));
    EXPECT_EQ(writer_call_count, 0);
    EXPECT_THAT(p->write_data, ContainerEq(msgvec));

    // check that we can poll through the stream
    EXPECT_FALSE(mav.poll());
    for (int i = 0; i < msgbuf_len; i++) { s.stream_data.push(msgbuf[i]); }
    EXPECT_TRUE(mav.poll());
}

INSTANTIATE_TEST_SUITE_P(
    MAVLinkConstructorTest,
    MAVLinkEndpointConstructorTests,
    ::testing::Combine(
        ::testing::Values(2, 3, 5, 7, 11, 13, 17),
        ::testing::Values(19, 23, 29, 31, 37, 41, 43),
        ::testing::Values(47, 53, 59, 61, 67, 71, 73)
    ) 
);