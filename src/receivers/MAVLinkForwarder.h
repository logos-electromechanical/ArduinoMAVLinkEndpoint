#pragma once

#include "MAVLinkEndpoint.h"
#include "receivers/MAVLinkRXCallback.h"

class MAVLinkForwarder : public MAVLinkRXCallback
{
public:
    MAVLinkForwarder() = delete;
    MAVLinkForwarder(MAVLinkEndpoint *in, MAVLinkEndpoint *out): 
        MAVLinkRXCallback(0, "forwarder", in), mTXEndpoint(out) {};
    virtual ~MAVLinkForwarder() {};
    virtual bool process(mavlink_message_t *msg, mavlink_status_t *stat)
    {
        bool result = mTXEndpoint->transmit(msg);
        return result;
    }
    virtual void timeout(uint64_t t) {};
    virtual bool isTimeout(uint64_t t) {return false;};
    virtual String getName() {return mRXName;};
    virtual uint64_t getLastRX() {return mLastRX;};
private:
    MAVLinkEndpoint *mTXEndpoint;
};