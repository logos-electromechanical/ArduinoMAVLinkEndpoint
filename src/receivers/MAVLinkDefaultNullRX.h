#pragma once

#include "receivers/MAVLinkRXCallback.h"

class MAVLinkEndpoint;

// Default message RX handler that turns off unidentified messages
class MAVLinkDefaultNullRX : public MAVLinkRXCallback
{
public:
    MAVLinkDefaultNullRX(MAVLinkEndpoint *e) : MAVLinkRXCallback(0, "default", e) {};
    bool process(mavlink_message_t *msg, mavlink_status_t *stat) { return true; };
    void timeout(uint32_t t) {};
    bool isTimeout(uint32_t t) { return false; };
};
