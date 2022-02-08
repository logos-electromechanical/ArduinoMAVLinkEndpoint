#pragma once

#include "MAVLinkEndpoint.h"
#include "receivers/MAVLinkRXCallback.h"

class MAVLinkRXCallback
{
public:
    MAVLinkRXCallback() = delete;
    MAVLinkRXCallback(uint32_t timeout, String name, MAVLinkEndpoint *e):
        mRXName(name), mRXEndpoint(e), mLastRX(0), mTimeout(timeout) {};
    virtual bool process(mavlink_message_t *msg, mavlink_status_t *stat) = 0;
    virtual void timeout(uint64_t t) = 0;
    virtual bool isTimeout(uint64_t t);
    virtual String getName() {return mRXName;};
    virtual uint64_t getLastRX() {return mLastRX;};
protected:
    const String            mRXName;
    MAVLinkEndpoint         *mRXEndpoint;
    uint64_t                mLastRX;
    const uint32_t          mTimeout;
};