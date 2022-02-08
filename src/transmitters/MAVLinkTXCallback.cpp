#include "transmitters/MAVLinkTXCallback.h"

bool MAVLinkTXCallback::isDue(uint64_t t)
{
    if ((t < mLastTX) || ((t - mLastTX) >= mTXInterval)) return true;
    return false;
}
