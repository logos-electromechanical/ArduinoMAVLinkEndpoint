#pragma once

#include "MAVLinkEndpoint.h"
#include "receivers/MAVLinkRXCallback.h"
#include <vector>
#include <algorithm>

class MAVLinkForwarder : public MAVLinkRXCallback
{
public:
    MAVLinkForwarder() = delete;
    MAVLinkForwarder(MAVLinkEndpoint *in, MAVLinkEndpoint *out, MAVLinkEndpoint *report = nullptr): 
        MAVLinkRXCallback(0, "forwarder", in), mTXEndpoint(out), mReportEndpoint(report), mAllowList(nullptr) {};
    MAVLinkForwarder(MAVLinkEndpoint *in, MAVLinkEndpoint *out, std::vector<uint16_t> *allow, MAVLinkEndpoint *report = nullptr): 
        MAVLinkRXCallback(0, "forwarder", in), mTXEndpoint(out), mReportEndpoint(report), mAllowList(allow) {};
    virtual ~MAVLinkForwarder() {};
    virtual bool process(mavlink_message_t *msg, mavlink_status_t *stat)
    {
        if ((mAllowList == nullptr) || (std::find(mAllowList->begin(), mAllowList->end(), msg->msgid) != mAllowList->end()))
        {
            bool result = mTXEndpoint->transmit(msg);
            // if (mReportEndpoint || mTXEndpoint->getDebugPrint() || mRXEndpoint->getDebugPrint()) 
            if (mReportEndpoint) 
            {
                mavlink_message_t repmsg;
                char msgbuf[50];
                snprintf(msgbuf, 50, "Fwding msg# %lu from %d to %d; last %lu", 
                    msg->msgid, mRXEndpoint->getSystemID(), 
                    mTXEndpoint->getSystemID(), millis() - mLastRX);
                mavlink_msg_statustext_pack(
                    mRXEndpoint->getSystemID(), mReportEndpoint->getComponentID(), &repmsg,
                    7, msgbuf, 0, 0);
                // if (mTXEndpoint->getDebugPrint()) mTXEndpoint->getDebugPrint()->println(msgbuf);
                // if (mRXEndpoint->getDebugPrint()) mRXEndpoint->getDebugPrint()->println(msgbuf);
                mReportEndpoint->transmit(&repmsg);
            }
            mLastRX = millis();
            return result;
        }
        return false;
    }
    virtual void timeout(uint32_t t) {};
    virtual bool isTimeout(uint32_t t) {return false;};
    virtual String getName() {return mRXName;};
    virtual uint32_t getLastRX() {return mLastRX;};
private:
    MAVLinkEndpoint *mTXEndpoint;
    MAVLinkEndpoint *mReportEndpoint;
    std::vector<uint16_t> *mAllowList;
};