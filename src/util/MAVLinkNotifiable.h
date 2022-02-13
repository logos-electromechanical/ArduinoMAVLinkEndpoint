#pragma once

#include <stdint.h>
#include "MAVLinkEndpoint.h"

template <typename T>
class MAVLinkNotifiable
{
public:
    MAVLinkNotifiable() {};
    virtual ~MAVLinkNotifiable() {};
    virtual void notify(const char* name, const uint8_t idx, const T p) = 0;
};