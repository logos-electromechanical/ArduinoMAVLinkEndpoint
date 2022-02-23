#include "receivers/MAVLinkRXCallback.h"

bool MAVLinkRXCallback::isTimeout(uint32_t t)
{
    if ((t < mLastRX) || ((t - mLastRX) > mTimeout)) return true;
    return false;
}

