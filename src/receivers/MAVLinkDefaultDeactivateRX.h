#pragma once

#include "receivers/MAVLinkRXCallback.h"

class MAVLinkEndpoint;

// Default message RX handler that turns off unidentified messages
class MAVLinkDefaultDeactivateRX : public MAVLinkRXCallback
{
public:
    MAVLinkDefaultDeactivateRX(MAVLinkEndpoint *e) : MAVLinkRXCallback(0, "default", e) {};
    bool process(mavlink_message_t *msg, mavlink_status_t *stat);
    void timeout(uint64_t t) {};
    bool isTimeout(uint64_t t) {return false;};
};
