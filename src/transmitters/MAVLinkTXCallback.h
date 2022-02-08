#pragma once

#include "MAVLinkEndpoint.h"

class MAVLinkEndpoint;  // forward declaration for class members

class MAVLinkTXCallback
{
public:
    MAVLinkTXCallback() = delete;
    MAVLinkTXCallback(uint32_t interval, String name, MAVLinkEndpoint *e):
        mTXName(name), mTXEndpoint(e), mLastTX(0), mTXInterval(interval) {};
    virtual void tick(uint64_t t) = 0;
    virtual bool isDue(uint64_t t);
    String getName() {return mTXName;};
    uint64_t getLastTX() {return mLastTX;};
protected:
    const String            mTXName;
    MAVLinkEndpoint         *mTXEndpoint;
    uint64_t                mLastTX;
    const uint32_t          mTXInterval;
};