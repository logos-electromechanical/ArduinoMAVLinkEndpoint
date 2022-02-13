#pragma once

#include <stdint.h>
#include "MAVLinkEndpoint.h"
#include "receivers/MAVLinkRXCallback.h"

class MAVLinkParamManager;  // Forward declaration so we can have a pointer

class MAVLinkParamListener : public MAVLinkRXCallback
{
public:
    MAVLinkParamListener (
        MAVLinkEndpoint     *e,
        MAVLinkParamManager *m
    ): MAVLinkRXCallback(0, "params", e) {};
    virtual ~MAVLinkParamListener() {};
    virtual bool process(mavlink_message_t *msg, mavlink_status_t *stat);
    virtual void timeout(uint64_t t) {};
    virtual bool isTimeout(uint64_t t) {return false;};
    virtual String getName() {return mRXName;};
    virtual uint64_t getLastRX() {return mLastRX;};
private:
    bool processListRequest(mavlink_message_t *msg, mavlink_status_t *stat);
    bool processReadRequest(mavlink_message_t *msg, mavlink_status_t *stat);
    bool processSetRequest(mavlink_message_t *msg, mavlink_status_t *stat);

    MAVLinkParamManager *mManager;
};