#pragma once

#include "MAVLinkEndpoint.h"
#include "receivers/MAVLinkRXCallback.h"

// Process incoming heartbeat packets
// TODO: Add interface to track incoming heartbeat packets
class MAVHeartbeatRX : public MAVLinkRXCallback
{
public:
    MAVHeartbeatRX (MAVLinkEndpoint *e) : MAVLinkRXCallback(0, "heartbeat", e) {};
    bool process(mavlink_message_t *msg, mavlink_status_t *stat);
    void timeout(uint64_t t) {};
    bool isTimeout(uint64_t t) {return false;};
};