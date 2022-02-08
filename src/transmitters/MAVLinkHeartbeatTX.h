#pragma once

#include "MAVLinkEndpoint.h"
#include "transmitters/MAVLinkTXCallback.h"

// Generate outgoing heartbeat packets
class MAVHeartbeatTX : public MAVLinkTXCallback {
public:
    MAVHeartbeatTX(MAVLinkEndpoint *mav, uint8_t vehicle, uint8_t autopilot,
        uint8_t sysmode, uint32_t custmode, uint8_t sysstate, uint32_t interval = 1000) :
        MAVLinkTXCallback(interval, "heartbeat", mav), mVehicle(vehicle),
        mAutopilot(autopilot), mSystemMode(sysmode), mCustomMode(custmode), 
        mSystemState(sysstate) {}
    void tick(uint64_t t);
    void setSystemMode(uint8_t mode) {mSystemMode = mode;};
    void setCustomMode(uint32_t mode) {mCustomMode = mode;};
    void setSystemState(uint8_t state) {mSystemState = state;};
private:
    const uint8_t mVehicle;
    const uint8_t mAutopilot;
    uint8_t mSystemMode;
    uint32_t mCustomMode;
    uint8_t mSystemState;
};