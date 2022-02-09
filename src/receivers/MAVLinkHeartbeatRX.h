#pragma once

#include "receivers/MAVLinkRXCallback.h"

class MAVLinkEndpoint;

// Process incoming heartbeat packets
// TODO: Add interface to track incoming heartbeat packets
class MAVLinkHeartbeatRX : public MAVLinkRXCallback
{
public:
    MAVLinkHeartbeatRX (MAVLinkEndpoint *e) : MAVLinkRXCallback(0, "heartbeat", e) {};
    bool process(mavlink_message_t *msg, mavlink_status_t *stat);
    void timeout(uint64_t t) {};
    bool isTimeout(uint64_t t) {return false;};
};