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
#include "test_utils.h"

#include <random>
#include <vector>
#include <memory>

using namespace testing;
using namespace std;

uint32_t currentMillis = 0; // declared here in order to enable millis() and set_millis() to work correctly

// Matcher for incoming message ID
MATCHER_P(IsMessageID, msg_id, "Check if the arg->msgid == value") {
    return arg->msgid == msg_id;
}

// Values in tuple are sending system id, sending component id, component to send message,
// requested message id, and desired transmission interval (in milliseconds)
class MAVLinkEndpointMsgRequestTest : 
public ::testing::TestWithParam<tuple<uint32_t, uint32_t, uint32_t>> {
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
    uint32_t seed = get<0>(GetParam()) * get<1>(GetParam()) * get<2>(GetParam());
    vector<uint8_t> data = generateArbitraryIntData<uint8_t>(&seed, 4);
    uint32_t interval = generateArbitraryIntData<uint32_t>(&seed, 1, 100000)[0];
    EXPECT_CALL(s, write(_)).Times(1);
    MAVLinkEndpoint mav(data[1], data[0], &s);
    mav.requestMessage(data[2], data[3], interval);
    checkRequestMessage(s.write_data, data[0], data[1], 
        data[2], data[3], interval);
}

// Tuple members are number of receivers to generate,
// the random seed to use for generating them
// the maximum interval/timeout range to use
class MAVLinkEndpointReceiveTickTest :
public ::testing::TestWithParam<tuple<uint32_t, uint32_t, uint32_t>> 
{
public:
    MAVLinkEndpointReceiveTickTest() {}
    virtual ~MAVLinkEndpointReceiveTickTest() { }

    void SetUp() 
    {
        seed = get<0>(GetParam()) * get<1>(GetParam()) * get<2>(GetParam());
        gen = unique_ptr<default_random_engine>(new default_random_engine(seed));
        mav = unique_ptr<MAVLinkEndpoint>(new MAVLinkEndpoint(0x10, 0x20));
        std::uniform_int_distribution<uint8_t> countDist(5,30);
        uint8_t count = countDist(*gen);
        for (int i = 0; i < count; i++)
        {
            std::uniform_int_distribution<uint32_t> timeout_distribution(10, 10000);
            uint32_t timeout = timeout_distribution(*gen);
            std::uniform_int_distribution<uint32_t> interval_distribution(0, timeout);
            uint32_t interval = interval_distribution(*gen);
            rcvrs.push_back(unique_ptr<MockReceiver>(new MockReceiver(timeout, interval, "", mav.get())));
            mav->registerRXCallback(rcvrs[i].get());
            tick_times[timeout] = rcvrs[i].get();
        }
    }

    unique_ptr<default_random_engine>   gen;
    unique_ptr<MAVLinkEndpoint>         mav;
    vector<unique_ptr<MockReceiver>>    rcvrs;
    map<uint32_t, MockReceiver*>        tick_times;
    uint32_t                            seed;
};

TEST_P(MAVLinkEndpointReceiveTickTest, TickTimingTest)
{
    InSequence seq;
    for (auto rx : tick_times) 
    {
        EXPECT_CALL(*(rx.second), timeout_action).Times(AtLeast(1));
    }
    for (auto rx : tick_times) 
    {
        set_millis(rx.second->getTimeout() - 1);
        mav->tick();
        EXPECT_EQ(0, rx.second->timeout_count);
        set_millis(rx.second->getTimeout() + 1);
        mav->tick();
        EXPECT_EQ(1, rx.second->timeout_count);
    }
}

INSTANTIATE_TEST_SUITE_P(
    MessageRequestTest,
    MAVLinkEndpointMsgRequestTest,
    ::testing::Combine(
        ::testing::Values(13, 17, 19, 23, 29),
        ::testing::Values(31, 37, 41, 43, 47), 	
        ::testing::Values(53, 59, 61, 67, 71)
    )
);

INSTANTIATE_TEST_SUITE_P(
    TickMethodTest,
    MAVLinkEndpointReceiveTickTest,
    ::testing::Combine(
        ::testing::Values(13, 17, 19, 23, 29),
        ::testing::Values(31, 37, 41, 43, 47), 	
        ::testing::Values(53, 59, 61, 67, 71)
    )
);