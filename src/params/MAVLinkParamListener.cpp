#include "MAVLinkEndpoint.h"
#include "params/MAVLinkParamListener.h"
#include "params/MAVLinkParamManager.h"

bool MAVLinkParamListener::process(mavlink_message_t *msg, mavlink_status_t *stat)
{
    switch(msg->msgid) {
        case MAVLINK_MSG_ID_PARAM_REQUEST_LIST:
            return processListRequest(msg, stat);
            break;
        case MAVLINK_MSG_ID_PARAM_REQUEST_READ:
            return processReadRequest(msg, stat);
            break;
        case MAVLINK_MSG_ID_PARAM_SET:
            return processSetRequest(msg, stat);
            break;
        default:
            return false;
    }
}

bool MAVLinkParamListener::processListRequest(mavlink_message_t *msg, mavlink_status_t *stat) 
{
    mavlink_param_request_list_t input;
    mavlink_msg_param_request_list_decode(msg, &input);
    if (mRXEndpoint->getDebugPrint())
    {
        Print *dbg = mRXEndpoint->getDebugPrint();
        dbg->print("Received param list request from system ");
        dbg->print(input.target_system);
        dbg->print(" targeting component ");
        dbg->println(input.target_component);
    }
    if (((mRXEndpoint->getSystemID() == input.target_system)
        && (MAV_COMP_ID_ALL == input.target_component))
        || (mRXEndpoint->getComponentID() == input.target_component)
    )
    {
        mManager->transmitParamList();
        return true;
    }
    return false;
}

bool MAVLinkParamListener::processReadRequest(mavlink_message_t *msg, mavlink_status_t *stat) 
{
    mavlink_param_request_read_t input;
    mavlink_msg_param_request_read_decode(msg, &input);
    if ((mRXEndpoint->getSystemID() != input.target_system) 
        || (mRXEndpoint->getComponentID() != input.target_component)
    )
    {
        if (mRXEndpoint->getDebugPrint()) mRXEndpoint->getDebugPrint()->println("Received param read request addressed to another system");
        return false;
    }
    int16_t idx = input.param_index;
    if ((idx < 0) || (idx > mManager->getParamCount()))
    {
        idx = mManager->getParamIdx(input.param_id);
        if ((idx < 0) || (idx > mManager->getParamCount())) return false;  // i.e. if the requested param does not exist       
    }
    mManager->transmitParam(idx);
    if (mRXEndpoint->getDebugPrint()) 
    {
        mRXEndpoint->getDebugPrint()->print("Received a parameter read request for idx ");
        mRXEndpoint->getDebugPrint()->println(idx);
    }
    return true;
}

bool MAVLinkParamListener::processSetRequest(mavlink_message_t *msg, mavlink_status_t *stat) 
{
    return false;
}
