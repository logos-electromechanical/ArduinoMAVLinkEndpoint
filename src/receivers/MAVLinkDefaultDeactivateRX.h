#pragma once

#include "MAVLinkEndpoint.h"
#include "receivers/MAVLinkRXCallback.h"

// Default message RX handler that turns off unidentified messages
class MAVDefaultDeactivateRX : public MAVLinkRXCallback
{
public:
    MAVDefaultDeactivateRX (MAVLinkEndpoint *e) : MAVLinkRXCallback(0, "default", e) {};
    bool process(mavlink_message_t *msg, mavlink_status_t *stat)
    {
        return mRXEndpoint->requestMessage(msg->compid, msg->msgid, -1);
    }
    void timeout(uint64_t t) {};
    bool isTimeout(uint64_t t) {return false;};
};
