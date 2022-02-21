#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "MAVLinkEndpoint.h"
#include "MockPrint.h"
#include "MockStream.h"
#include "mavlink/ardupilotmega/mavlink.h"

using namespace testing;
using namespace std;

template<typename T>
class MAVLinkEndpointConstructorTests : 
public ::testing::TestWithParam<T>
{
public:

    MAVLinkEndpointConstructorTests() { 
        writer_call_count = 0; 
        p = new MockPrint();  
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
};

template<>
int MAVLinkEndpointConstructorTests<tuple<int16_t, int16_t>>::writer_call_count = 0;
template<>
MockPrint *MAVLinkEndpointConstructorTests<tuple<int16_t, int16_t>>::p = nullptr;

template<>
int MAVLinkEndpointConstructorTests<tuple<int16_t, int16_t, uint8_t, uint8_t, uint8_t, uint32_t, uint8_t>>::writer_call_count = 0;
template<>
MockPrint *MAVLinkEndpointConstructorTests<tuple<int16_t, int16_t, uint8_t, uint8_t, uint8_t, uint32_t, uint8_t>>::p = nullptr;

class MAVLinkEndpointDualConstructorTest : 
    public MAVLinkEndpointConstructorTests<tuple<int16_t, int16_t>> {};

class MAVLinkEndpointWriterConstructorTest : 
    public MAVLinkEndpointConstructorTests<tuple<int16_t, int16_t, uint8_t, uint8_t, uint8_t, uint32_t, uint8_t>> {};

TEST_P(MAVLinkEndpointDualConstructorTest, TwoArgs) 
{
    EXPECT_CALL(*p, write(_)).Times(0);
    EXPECT_CALL(s, write(_)).Times(0);
    MAVLinkEndpoint mav(get<0>(GetParam()), get<1>(GetParam()));
    if ((get<0>(GetParam()) >= 0) && (get<0>(GetParam()) <= 255)) 
    {
        EXPECT_EQ(mav.getComponentID(), get<0>(GetParam())); 
    } else 
    {
        EXPECT_NE(mav.getSystemID(), get<0>(GetParam()));
    }
    if ((get<1>(GetParam()) >= 0) && (get<1>(GetParam()) <= 255)) 
    {
        EXPECT_EQ(mav.getSystemID(), get<1>(GetParam())); 
    } else 
    {
        EXPECT_NE(mav.getComponentID(), get<1>(GetParam()));
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

TEST_P(MAVLinkEndpointDualConstructorTest, RegisterIOTests)
{
    const int msg_size = 21;
    EXPECT_CALL(*p, write(_))
        .Times(2)
        .WillRepeatedly(Return(msg_size));
    EXPECT_CALL(s, write(_))
        .Times(0);
    MAVLinkEndpoint mav(get<0>(GetParam()), get<1>(GetParam()));
    EXPECT_EQ(nullptr, mav.getDebugPrint());
    EXPECT_EQ(nullptr, mav.getTXPrint());
    EXPECT_EQ(nullptr, mav.getRXStream());
    EXPECT_TRUE(mav.registerWriter(test_writer));
    mavlink_message_t msg;
    mavlink_msg_heartbeat_pack(get<0>(GetParam()), get<1>(GetParam()), &msg, 0, 0, 0, 0, 0); 
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

TEST_P(MAVLinkEndpointWriterConstructorTest, ThreeArgsWriter) {
    const int msg_size = 21;
    EXPECT_CALL(*p, write(_))
        .Times(1)
        .WillOnce(Return(msg_size));
    MAVLinkEndpoint mav(
        get<0>(GetParam()), 
        get<1>(GetParam()),
        &MAVLinkEndpointWriterConstructorTest::test_writer
    );
    if ((get<0>(GetParam()) >= 0) && (get<0>(GetParam()) <= 255)) 
    {
        EXPECT_EQ(mav.getComponentID(), get<0>(GetParam())); 
    } else 
    {
        EXPECT_NE(mav.getSystemID(), get<0>(GetParam()));
    }
    if ((get<1>(GetParam()) >= 0) && (get<1>(GetParam()) <= 255)) 
    {
        EXPECT_EQ(mav.getSystemID(), get<1>(GetParam())); 
    } else 
    {
        EXPECT_NE(mav.getComponentID(), get<1>(GetParam()));
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
    EXPECT_EQ(writer_call_count, 0);

    mavlink_message_t msg;
    uint8_t msgbuf[MAVLINK_MAX_PACKET_LEN];
    vector<uint8_t> msgvec;
    mavlink_msg_heartbeat_pack(
        get<0>(GetParam()),
        get<1>(GetParam()),
        &msg,
        get<2>(GetParam()),
        get<3>(GetParam()),
        get<4>(GetParam()),
        get<5>(GetParam()),
        get<6>(GetParam())
    ); 
    uint16_t msgbuf_len = mavlink_msg_to_send_buffer(msgbuf, &msg);
    for (int i = 0; i < msgbuf_len; i++) { msgvec.push_back(msgbuf[i]); }
    EXPECT_EQ(msg_size, mav.transmit(&msg));
    EXPECT_EQ(writer_call_count, 1);
    EXPECT_THAT(p->write_data, ContainerEq(msgvec));
}

TEST_P(MAVLinkEndpointWriterConstructorTest, ThreeArgsPrint) {
    const int msg_size = 21;
    EXPECT_CALL(*p, write(_))
        .Times(1)
        .WillOnce(Return(msg_size));
    MAVLinkEndpoint mav(
        get<0>(GetParam()), 
        get<1>(GetParam()),
        p
    );
    if ((get<0>(GetParam()) >= 0) && (get<0>(GetParam()) <= 255)) 
    {
        EXPECT_EQ(mav.getComponentID(), get<0>(GetParam())); 
    } else 
    {
        EXPECT_NE(mav.getSystemID(), get<0>(GetParam()));
    }
    if ((get<1>(GetParam()) >= 0) && (get<1>(GetParam()) <= 255)) 
    {
        EXPECT_EQ(mav.getSystemID(), get<1>(GetParam())); 
    } else 
    {
        EXPECT_NE(mav.getComponentID(), get<1>(GetParam()));
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
    EXPECT_NE(nullptr, mav.getTXPrint());
    EXPECT_EQ(nullptr, mav.getRXStream());
    EXPECT_EQ(0, mav.transmit(nullptr));
    EXPECT_EQ(writer_call_count, 0);

    mavlink_message_t msg;
    uint8_t msgbuf[MAVLINK_MAX_PACKET_LEN];
    vector<uint8_t> msgvec;
    mavlink_msg_heartbeat_pack(
        get<0>(GetParam()),
        get<1>(GetParam()),
        &msg,
        get<2>(GetParam()),
        get<3>(GetParam()),
        get<4>(GetParam()),
        get<5>(GetParam()),
        get<6>(GetParam())
    ); 
    uint16_t msgbuf_len = mavlink_msg_to_send_buffer(msgbuf, &msg);
    for (int i = 0; i < msgbuf_len; i++) { msgvec.push_back(msgbuf[i]); }
    EXPECT_EQ(msg_size, mav.transmit(&msg));
    EXPECT_EQ(writer_call_count, 0);
    EXPECT_THAT(p->write_data, ContainerEq(msgvec));
}

TEST_P(MAVLinkEndpointWriterConstructorTest, ThreeArgsStream) {
    const int msg_size = 21;
    EXPECT_CALL(s, write(_))
        .Times(1)
        .WillOnce(Return(msg_size));
    EXPECT_CALL(*p, write(_))
        .Times(0);
    MAVLinkEndpoint mav(
        get<0>(GetParam()), 
        get<1>(GetParam()),
        &s
    );
    if ((get<0>(GetParam()) >= 0) && (get<0>(GetParam()) <= 255)) 
    {
        EXPECT_EQ(mav.getComponentID(), get<0>(GetParam())); 
    } else 
    {
        EXPECT_NE(mav.getSystemID(), get<0>(GetParam()));
    }
    if ((get<1>(GetParam()) >= 0) && (get<1>(GetParam()) <= 255)) 
    {
        EXPECT_EQ(mav.getSystemID(), get<1>(GetParam())); 
    } else 
    {
        EXPECT_NE(mav.getComponentID(), get<1>(GetParam()));
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
    EXPECT_NE(nullptr, mav.getTXPrint());
    EXPECT_NE(nullptr, mav.getRXStream());
    EXPECT_EQ(mav.getTXPrint(), mav.getRXStream());
    EXPECT_EQ(0, mav.transmit(nullptr));
    EXPECT_EQ(writer_call_count, 0);

    // test that we can write out through the stream
    mavlink_message_t msg;
    uint8_t msgbuf[MAVLINK_MAX_PACKET_LEN];
    vector<uint8_t> msgvec;
    mavlink_msg_heartbeat_pack(
        get<0>(GetParam()),
        get<1>(GetParam()),
        &msg,
        get<2>(GetParam()),
        get<3>(GetParam()),
        get<4>(GetParam()),
        get<5>(GetParam()),
        get<6>(GetParam())
    ); 
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

TEST_P(MAVLinkEndpointWriterConstructorTest, FourArgs) {
    const int msg_size = 21;
    EXPECT_CALL(*MAVLinkEndpointWriterConstructorTest::p, write(_))
        .Times(1)
        .WillOnce(Return(msg_size));
    MAVLinkEndpoint mav(
        get<0>(GetParam()), 
        get<1>(GetParam()),
        &s, p
    );
    if ((get<0>(GetParam()) >= 0) && (get<0>(GetParam()) <= 255)) 
    {
        EXPECT_EQ(mav.getComponentID(), get<0>(GetParam())); 
    } else 
    {
        EXPECT_NE(mav.getSystemID(), get<0>(GetParam()));
    }
    if ((get<1>(GetParam()) >= 0) && (get<1>(GetParam()) <= 255)) 
    {
        EXPECT_EQ(mav.getSystemID(), get<1>(GetParam())); 
    } else 
    {
        EXPECT_NE(mav.getComponentID(), get<1>(GetParam()));
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
    EXPECT_NE(nullptr, mav.getTXPrint());
    EXPECT_NE(nullptr, mav.getRXStream());
    EXPECT_NE(mav.getTXPrint(), mav.getRXStream());
    EXPECT_EQ(0, mav.transmit(nullptr));
    EXPECT_EQ(writer_call_count, 0);

    // test that we can write out through the stream
    mavlink_message_t msg;
    uint8_t msgbuf[MAVLINK_MAX_PACKET_LEN];
    vector<uint8_t> msgvec;
    mavlink_msg_heartbeat_pack(
        get<0>(GetParam()),
        get<1>(GetParam()),
        &msg,
        get<2>(GetParam()),
        get<3>(GetParam()),
        get<4>(GetParam()),
        get<5>(GetParam()),
        get<6>(GetParam())
    ); 
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

TEST_P(MAVLinkEndpointWriterConstructorTest, FiveArgs) {
    const int msg_size = 21;
    EXPECT_CALL(*p, write(_))
        .Times(1)
        .WillOnce(Return(msg_size));
    EXPECT_CALL(d, println(Matcher<const char *>(_)))
        .Times(AtLeast(1));
    EXPECT_CALL(d, print(Matcher<const char *>(_)))
        .Times(AtLeast(1));
    EXPECT_CALL(d, println(Matcher<uint32_t>(_),Matcher<int>(_)))
        .Times(AtLeast(1));
    MAVLinkEndpoint mav(
        get<0>(GetParam()), 
        get<1>(GetParam()),
        &s, p, &d
    );
    if ((get<0>(GetParam()) >= 0) && (get<0>(GetParam()) <= 255)) 
    {
        EXPECT_EQ(mav.getComponentID(), get<0>(GetParam())); 
    } else 
    {
        EXPECT_NE(mav.getSystemID(), get<0>(GetParam()));
    }
    if ((get<1>(GetParam()) >= 0) && (get<1>(GetParam()) <= 255)) 
    {
        EXPECT_EQ(mav.getSystemID(), get<1>(GetParam())); 
    } else 
    {
        EXPECT_NE(mav.getComponentID(), get<1>(GetParam()));
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
    mavlink_msg_heartbeat_pack(
        get<0>(GetParam()),
        get<1>(GetParam()),
        &msg,
        get<2>(GetParam()),
        get<3>(GetParam()),
        get<4>(GetParam()),
        get<5>(GetParam()),
        get<6>(GetParam())
    ); 
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
    MAVLinkConstructorsTwoArgTest,
    MAVLinkEndpointDualConstructorTest,
    ::testing::Combine(
        ::testing::Values(0, -1, UINT8_MAX, INT16_MAX, INT16_MIN, 100, 32),
        ::testing::Values(0, -1, UINT8_MAX, INT16_MAX, INT16_MIN, 100, 32)
    ) 
);

INSTANTIATE_TEST_SUITE_P(
    MAVLinkConstructorsWriterTest,
    MAVLinkEndpointWriterConstructorTest,
    ::testing::Combine(
        #ifdef LONG_TEST
        ::testing::Values(0, -1, UINT8_MAX, INT16_MAX, INT16_MIN, 100, 32),
        ::testing::Values(0, -1, UINT8_MAX, INT16_MAX, INT16_MIN, 100, 32),
        ::testing::Values(0, 255, 1, 32, 2, 10, 100, 200),
        ::testing::Values(0, 100, UINT8_MAX),
        ::testing::Values(0, 100, UINT8_MAX),
        ::testing::Values(0, 1, 100, UINT32_MAX),
        ::testing::Values(0, 100, UINT8_MAX)
        #else
        ::testing::Values(0, -1, 100, 32),
        ::testing::Values(0, -1, 100, 32),
        ::testing::Values(0, 255),
        ::testing::Values(0, UINT8_MAX),
        ::testing::Values(0, UINT8_MAX),
        ::testing::Values(0, 1, 100),
        ::testing::Values(0, UINT8_MAX)
        #endif //LONG_TEST
    ) 
);