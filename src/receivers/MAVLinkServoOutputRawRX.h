#pragma once

#include "MAVLinkEndpoint.h"
#include "receivers/MAVLinkRXCallback.h"
#include "receivers/MAVLinkRXNotifiable.h"

#define MAV_SERVO_MAX_NOTIFIER_COUNT    (32)
#define MAV_SERVO_CHANNEL_COUNT         (16)

class MAVLinkServoOutputRawRX : public MAVLinkRXCallback
{
public:
    MAVLinkServoOutputRawRX() = delete;
    MAVLinkServoOutputRawRX(uint32_t timeout, uint32_t interval, String name, MAVLinkEndpoint *e);
    virtual bool process(mavlink_message_t *msg, mavlink_status_t *stat);
    virtual void timeout(uint64_t t);
    virtual void setChannelFailsafe(uint8_t channel, int16_t value);
    virtual void registerTimeCallback(MAVLinkRXNotifiable<uint32_t> *cb);
    virtual void registerPortCallback(MAVLinkRXNotifiable<uint8_t> *cb);
    virtual bool registerServoCallback(MAVLinkRXNotifiable<uint16_t> *cb, 
                                        const String name, 
                                        uint8_t servoChannel);
    virtual uint8_t getLastPort() { return mLastPort; };
    virtual uint32_t getLastMAVTime() { return mLastMAVLinkTime; };
    virtual uint8_t getLastSystemID() { return mLastSysID; }
    virtual uint8_t getLastComponentID() { return mLastCompID; }
    virtual uint16_t getServo(uint8_t channel);
    
private:
    void callNotifiers(uint8_t compid);
    void setFailSafes();
    uint32_t                        mLastMAVLinkTime; 
    uint8_t                         mLastPort;
    uint16_t                        mServosRaw[MAV_SERVO_CHANNEL_COUNT];
    int16_t                         mServosRawFailsafe[MAV_SERVO_CHANNEL_COUNT];
    uint8_t                         mLastSysID;
    uint8_t                         mLastCompID;
    uint8_t                         mServoNotifierCount;
    MAVLinkRXNotifiable<uint32_t>   *mTimeNotifier;
    MAVLinkRXNotifiable<uint8_t>    *mPortNotifier;
    String                          mNotifierNames[MAV_SERVO_MAX_NOTIFIER_COUNT];
    uint8_t                         mNotifierIdx[MAV_SERVO_MAX_NOTIFIER_COUNT];
    MAVLinkRXNotifiable<uint16_t>   *mServoNotifiers[MAV_SERVO_MAX_NOTIFIER_COUNT];
};