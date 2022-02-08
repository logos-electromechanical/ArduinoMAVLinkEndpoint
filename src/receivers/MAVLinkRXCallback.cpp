#include "receivers/MAVLinkRXCallback.h"

bool MAVLinkRXCallback::isTimeout(uint64_t t)
{
    if ((t < mLastRX) || ((t - mLastRX) > mTimeout)) return true;
    return false;
}

