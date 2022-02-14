#pragma once

#include "Arduino.h"
#include "mavlink/ardupilotmega/mavlink.h"
#include "receivers/MAVLinkRXCallback.h"
#include "transmitters/MAVLinkTXCallback.h"

#ifndef MAVLINK_RX_MAX_HANDLER_COUNT
    #define MAVLINK_RX_MAX_HANDLER_COUNT    (32)
#endif // MAVLINK_RX_MAX_HANDLER_COUNT
#ifndef MAVLINK_TX_MAX_HANDLER_COUNT
    #define MAVLINK_TX_MAX_HANDLER_COUNT    (32)
#endif // MAVLINK_RX_MAX_HANDLER_COUNT

class MAVLinkRXCallback;    // forward declared so we can declare pointers to it
class MAVLinkTXCallback;    // forward declared so we can declare pointers to it

typedef size_t (*MAVlinkTXWriter_t)(uint8_t *data, size_t len);             // Function to write out a packet

class MAVLinkEndpoint
{
public:
    MAVLinkEndpoint(uint8_t compid, uint8_t sysid):
        mComponentID(compid), mSystemID(sysid), mRXDefaultCallback(nullptr),
        mWriter(nullptr), mInputStream(nullptr), mOutputPrint(nullptr), 
        mRXCallbackCount(0), mTXCallbackCount(0) {};
    MAVLinkEndpoint(uint8_t compid, uint8_t sysid, MAVlinkTXWriter_t w):
        mComponentID(compid), mSystemID(sysid), mRXDefaultCallback(nullptr),
        mWriter(w), mInputStream(nullptr), mOutputPrint(nullptr), 
        mRXCallbackCount(0), mTXCallbackCount(0) {};
    MAVLinkEndpoint(uint8_t compid, uint8_t sysid, Stream *s):
        mComponentID(compid), mSystemID(sysid), mRXDefaultCallback(nullptr),
        mWriter(nullptr), mInputStream(s), mOutputPrint(nullptr), 
        mRXCallbackCount(0), mTXCallbackCount(0) {};
    MAVLinkEndpoint(uint8_t compid, uint8_t sysid, Print *p):
        mComponentID(compid), mSystemID(sysid), mRXDefaultCallback(nullptr),
        mWriter(nullptr), mInputStream(nullptr), mOutputPrint(p), 
        mRXCallbackCount(0), mTXCallbackCount(0) {};
    MAVLinkEndpoint(uint8_t compid, uint8_t sysid, Stream *s, Print *p):
        mComponentID(compid), mSystemID(sysid), mRXDefaultCallback(nullptr),
        mWriter(nullptr), mInputStream(s), mOutputPrint(p), 
        mRXCallbackCount(0), mTXCallbackCount(0) {};
    virtual ~MAVLinkEndpoint() {};
    virtual bool registerRXCallback(MAVLinkRXCallback* cb);
    virtual bool registerRXDefaultCallback(MAVLinkRXCallback* cb);
    virtual bool registerTXCallback(MAVLinkTXCallback* cb);
    virtual bool registerWriter(MAVlinkTXWriter_t w);
    virtual bool registerRXStream(Stream *s);
    virtual bool registerTXPrint(Print *p);
    virtual bool registerDebugPrint(Print *p);
    virtual bool process(uint8_t *data, size_t len);
    virtual bool process(uint8_t c);
    virtual bool poll();
    virtual size_t transmit(mavlink_message_t *msg_ptr);
    virtual bool requestMessage(uint8_t msg_src, uint8_t msg_id, uint32_t interval);
    virtual uint8_t getSystemID() {return mSystemID;};
    virtual uint8_t getComponentID() {return mComponentID;};
    virtual void tick();
    virtual Print* getDebugPrint() {return mDebugPrint;};
private:
    const uint8_t       mComponentID;
    const uint8_t       mSystemID;
    MAVLinkRXCallback   *mRXCallbacks[MAVLINK_RX_MAX_HANDLER_COUNT];
    MAVLinkRXCallback   *mRXDefaultCallback;
    MAVLinkTXCallback   *mTXCallbacks[MAVLINK_TX_MAX_HANDLER_COUNT];
    MAVlinkTXWriter_t   mWriter;
    Stream              *mInputStream;
    Print               *mOutputPrint;
    Print               *mDebugPrint;
    uint8_t             mRXCallbackCount;
    uint8_t             mTXCallbackCount;
    mavlink_message_t   mMessage;
    mavlink_status_t    mStatus;
};
