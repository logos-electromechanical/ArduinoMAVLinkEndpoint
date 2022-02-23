#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "MAVLinkEndpoint.h"
#include "MockPrint.h"
#include "MockStream.h"
#include "MockCallbacks.h"
#include "mavlink/ardupilotmega/mavlink.h"
#include "receivers/MAVLinkRXCallback.h"
#include "receivers/MAVLinkDefaultDeactivateRX.h"
#include "transmitters/MAVLinkTXCallback.h"

#include <random>
#include <vector>
#include <memory>

using namespace testing;
using namespace std;

// Matcher for incoming message ID
MATCHER_P(IsMessageID, msg_id, "Check if the arg->msgid == value") {
    return arg->msgid == msg_id;
}

// Values in tuple are sending system id, sending component id, component to send message,
// requested message id, and desired transmission interval (in milliseconds)
class MAVLinkEndpointMsgRequestTest : 
public ::testing::TestWithParam<tuple<uint8_t, uint8_t, uint8_t, uint8_t, uint32_t>> {
public:
    MAVLinkEndpointMsgRequestTest() {}
    virtual ~MAVLinkEndpointMsgRequestTest() {}

    void checkRequestMessage(
        vector<uint8_t> buf, 
        uint8_t     sys_id,
        uint8_t     comp_id,
        uint8_t     msg_src, 
        uint8_t     msg_id, 
        uint32_t    interval
    ) {
        mavlink_message_t msg;
        mavlink_status_t status;
        mavlink_command_long_t cmd;
        bool rcvd;
        for (uint8_t c : buf)
        {
            rcvd |= mavlink_parse_char(MAVLINK_COMM_0, c, &msg, &status);
        }
        ASSERT_TRUE(rcvd);
        ASSERT_EQ(MAVLINK_MSG_ID_COMMAND_LONG, msg.msgid);
        EXPECT_EQ(comp_id, msg.compid);
        EXPECT_EQ(sys_id, msg.sysid);
        mavlink_msg_command_long_decode(&msg, &cmd);
        EXPECT_EQ(cmd.command, MAV_CMD_SET_MESSAGE_INTERVAL);
        EXPECT_EQ(cmd.target_system, sys_id);
        EXPECT_EQ(cmd.target_component, msg_src);
        EXPECT_EQ(cmd.confirmation, 0);
        EXPECT_FLOAT_EQ(cmd.param1, msg_id);
        EXPECT_FLOAT_EQ(cmd.param2, interval);
        EXPECT_FLOAT_EQ(cmd.param3, 0);
        EXPECT_FLOAT_EQ(cmd.param4, 0);
        EXPECT_FLOAT_EQ(cmd.param5, 0);
        EXPECT_FLOAT_EQ(cmd.param6, 0);
        EXPECT_FLOAT_EQ(cmd.param7, 1);
    }
};

TEST_P(MAVLinkEndpointMsgRequestTest, MsgRequestTest)
{
    MockStream s;
    EXPECT_CALL(s, write(_)).Times(1);
    MAVLinkEndpoint mav(get<1>(GetParam()), get<0>(GetParam()), &s);
    mav.requestMessage(get<2>(GetParam()), get<3>(GetParam()), get<4>(GetParam()));
    checkRequestMessage(s.write_data, get<0>(GetParam()), get<1>(GetParam()), 
        get<2>(GetParam()), get<3>(GetParam()), get<4>(GetParam()));
}

// Tuple members are number of receivers to generate,
// the random seed to use for generating them
// the maximum interval/timeout range to use
class MAVLinkEndpointReceiveTickTest :
public ::testing::TestWithParam<tuple<uint8_t, uint32_t, uint32_t>> 
{
public:
    MAVLinkEndpointReceiveTickTest() {}
    virtual ~MAVLinkEndpointReceiveTickTest() { }

    void SetUp() 
    {
        gen = new default_random_engine(get<1>(GetParam()));
        mav = new MAVLinkEndpoint(0x10, 0x20);
        for (int i = 0; i < get<0>(GetParam()); i++)
        {
            uinform_int_distribution<uint32_t> timeout_distribution(10,get<2>(GetParam()));
            uint32_t timeout = timeout_distribution(*gen);
            uinform_int_distribution<uint32_t> interval_distribution(0, timeout);
            uint32_t interval = interval_distribution(*gen);
            rcvrs.push_back(new MockReceiver(timeout, interval, "", mav.get()));
            mav->registerRXCallback(rcvrs[i].get());
            tick_times[timeout] = rcvrs[i].get();
        }
    }

    unique_ptr<default_random_engine>   gen;
    unique_ptr<MAVLinkEndpoint>         mav;
    vector<unique_ptr<MockReceiver>>    rcvrs;
    map<uint32_t, MockReceiver*>        tick_times;
}

TEST_P(MAVLinkEndpointReceiveTickTest, TickTimingTest)
{
    
    InSequence seq;
    for (MockReceiver* rx : tick_times) 
    {
        EXPECT_CALL(*rx, timeout_action, ());
    }
    for (MockReceiver* rx : tick_times) 
    {
        set_millis(rx->getTimeout() - 1);
        mav->tick();
        EXPECT_EQ(0, rx->timeout_count);
        set_millis(rx->getTimeout() + 1);
        mav->tick();
        EXPECT_EQ(1, rx->timeout_count);
    }
}


INSTANTIATE_TEST_SUITE_P(
    MessageRequestTest,
    MAVLinkEndpointMsgRequestTest,
    ::testing::Combine(
        ::testing::Values(0, 1, 10, 100, 200, 255),
        ::testing::Values(0, 1, 10, 100, 200, 255),
        ::testing::Values(0, 1, 10, 100, 200, 255),
        ::testing::Values(0, 1, 10, 100, 200, 255),
        ::testing::Values(0, 100, 1000, 10000, UINT32_MAX)
    )
);