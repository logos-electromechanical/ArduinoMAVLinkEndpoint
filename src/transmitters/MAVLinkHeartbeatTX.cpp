#include "transmitters/MAVLinkHeartbeatTX.h"

void MAVHeartbeatTX::tick(uint64_t t) {
    if (is_due(t)) {
        mavlink_message_t msg;
        if (mTXEndpoint->getDebugPrint()) 
        {
            mTXEndpoint->getDebugPrint()->println("Sending heartbeat");
        }
        mavlink_msg_heartbeat_pack(mTXEndpoint->getSystemID(), mTXEndpoint->getComponentID(), &msg,
            mVehicle, mAutopilot, mSystemMode, mCustomMode, mSystemState);
        if (mTXEndpoint->transmit(&msg)) last_tx = t;
    }
}
