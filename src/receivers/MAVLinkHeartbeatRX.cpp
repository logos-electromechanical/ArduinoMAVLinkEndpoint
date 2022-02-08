#include "receivers/MAVLinkHeartbeatRX.h"

bool MAVHeartbeatRX::process(mavlink_message_t *msg, mavlink_status_t *stat)
{
    if (msg->msgid != MAVLINK_MSG_ID_HEARTBEAT) return false;
    if (mRXEndpoint->getDebugPrint()) 
    {
        Print *out = mRXEndpoint->getDebugPrint();
        out->print("Received heartbeat from component ");
        out->print(msg->compid);
        out->print(" of system ");
        out->println(msg->sysid)
    }
    mLastRX = millis();
    return true;
}