#include "receivers/MAVLinkServoOutputRawRX.h"

MAVLinkServoOutputRawRX::MAVLinkServoOutputRawRX(
    uint32_t timeout, uint32_t interval, 
    String name, MAVLinkEndpoint *e
) : MAVLinkRXCallback(timeout, interval, name, e), 
    mServoNotifierCount(0),
    mTimeNotifier(nullptr), 
    mPortNotifier(nullptr)
{
}

bool MAVLinkServoOutputRawRX::process(
    mavlink_message_t *msg, 
    mavlink_status_t *stat
) {
    if (msg->msgid != MAVLINK_MSG_ID_SERVO_OUTPUT_RAW) return false;
    mavlink_servo_output_raw_t input;
    mavlink_msg_servo_output_raw_decode(msg, &input);
    mLastMAVLinkTime = input.time_usec;
    mLastSysID = msg->sysid;
    mLastCompID = msg->compid;
    mLastPort = input.port;
    mServosRaw[0] = input.servo1_raw;
    mServosRaw[1] = input.servo2_raw;
    mServosRaw[2] = input.servo3_raw;
    mServosRaw[3] = input.servo4_raw;
    mServosRaw[4] = input.servo5_raw;
    mServosRaw[5] = input.servo6_raw;
    mServosRaw[6] = input.servo7_raw;
    mServosRaw[7] = input.servo8_raw;
    mServosRaw[8] = input.servo9_raw;
    mServosRaw[9] = input.servo10_raw;
    mServosRaw[10] = input.servo11_raw;
    mServosRaw[11] = input.servo12_raw;
    mServosRaw[12] = input.servo13_raw;
    mServosRaw[13] = input.servo14_raw;
    mServosRaw[14] = input.servo15_raw;
    mServosRaw[15] = input.servo16_raw;
    callNotifiers(msg->compid);
    return true;
}

void MAVLinkServoOutputRawRX::setChannelFailsafe(uint8_t channel, int16_t value)
{
    channel--;
    if (channel < MAV_SERVO_CHANNEL_COUNT) mServosRawFailsafe[channel] = value;
}

void MAVLinkServoOutputRawRX::setFailSafes()
{
    for (uint8_t idx = 0; idx < MAV_SERVO_CHANNEL_COUNT; idx++)
    {
        if (mServosRawFailsafe[idx] > 0) mServosRaw[idx] = mServosRawFailsafe[idx]; 
    }
    callNotifiers(mRXEndpoint->getComponentID());
}

void MAVLinkServoOutputRawRX::callNotifiers(uint8_t compid)
{
    if (mTimeNotifier) mTimeNotifier->notify(mRXName.c_str(), compid, mLastMAVLinkTime);
    if (mPortNotifier) mPortNotifier->notify(mRXName.c_str(), compid, mLastPort);
    for (uint8_t idx = 0; idx < mServoNotifierCount; idx++)
    {
        if (mServoNotifiers[idx]) 
        {
            mServoNotifiers[idx]->notify((mRXName + mNotifierNames[idx]).c_str(), 
                                        compid, mServosRaw[mNotifierIdx[idx]]);
        }
    }
}

void MAVLinkServoOutputRawRX::timeout(uint64_t t)
{
    if (isTimeout(t))
    {
        setFailSafes();
        mRXEndpoint->requestMessage(MAV_COMP_ID_AUTOPILOT1, 
                                    MAVLINK_MSG_ID_SERVO_OUTPUT_RAW, 
                                    mInterval);
    }
}

void MAVLinkServoOutputRawRX::registerTimeCallback
(
    MAVLinkNotifiable<uint32_t> *cb
) {
    if (cb) mTimeNotifier = cb;
}

void MAVLinkServoOutputRawRX::registerPortCallback
(
    MAVLinkNotifiable<uint8_t> *cb
) {
    if (cb) mPortNotifier = cb;
}

bool MAVLinkServoOutputRawRX::registerServoCallback
(
    MAVLinkNotifiable<uint16_t> *cb, 
    const String name, 
    uint8_t servoChannel
) {
    servoChannel--;
    if ((mServoNotifierCount < MAV_SERVO_MAX_NOTIFIER_COUNT) 
        && cb && (servoChannel <= MAV_SERVO_CHANNEL_COUNT))
    {
        mServoNotifiers[mServoNotifierCount] = cb;
        mNotifierIdx[mServoNotifierCount] = servoChannel;
        mNotifierNames[mServoNotifierCount] = name;
        mServoNotifierCount++;
    }
    else return false;
    return true;
}

uint16_t MAVLinkServoOutputRawRX::getServo(uint8_t channel) 
{
    channel--;
    if (channel < MAV_SERVO_CHANNEL_COUNT) return mServosRaw[channel];
    return 0;
}
