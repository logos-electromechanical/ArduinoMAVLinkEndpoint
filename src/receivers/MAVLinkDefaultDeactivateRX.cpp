#include "receivers/MAVLinkDefaultDeactivateRX.h"

bool MAVLinkDefaultDeactivateRX::process(mavlink_message_t *msg, mavlink_status_t *stat)
{
    return mRXEndpoint->requestMessage(msg->compid, msg->msgid, -1);
}