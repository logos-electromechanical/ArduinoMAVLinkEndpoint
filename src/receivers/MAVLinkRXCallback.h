#pragma once

#include "MAVLinkEndpoint.h"

class MAVLinkEndpoint;

class MAVLinkRXCallback
{
public:
    MAVLinkRXCallback() = delete;
    MAVLinkRXCallback(uint32_t timeout, String name, MAVLinkEndpoint *e):
        mRXName(name), mRXEndpoint(e), mLastRX(0), mTimeout(timeout), mInterval(1000000) {};
    MAVLinkRXCallback(uint32_t timeout, uint32_t interval, String name, MAVLinkEndpoint *e):
        mRXName(name), mRXEndpoint(e), mLastRX(0), mTimeout(timeout), mInterval(interval) {};
    virtual ~MAVLinkRXCallback() {};
    virtual bool process(mavlink_message_t *msg, mavlink_status_t *stat) = 0;
    virtual void timeout(uint32_t t) = 0;
    virtual bool isTimeout(uint32_t t);
    virtual String getName() { return mRXName; }
    virtual uint32_t getLastRX() { return mLastRX; }
    virtual uint32_t getInterval() { return mInterval; }
    virtual uint32_t getTimeout() { return mTimeout; }
protected:
    const String            mRXName;
    MAVLinkEndpoint         *mRXEndpoint;
    uint32_t                mLastRX;
    const uint32_t          mTimeout;
    const uint32_t          mInterval;
};