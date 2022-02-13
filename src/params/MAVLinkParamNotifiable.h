#pragma once

#include <stdint.h>
#include "MAVLinkEndpoint.h"

class MAVLinkParamNotifiable
{
public:
    MAVLinkParamNotifiable() {};
    virtual ~MAVLinkParamNotifiable() {};
    virtual void notify(const char* name, const uint8_t idx, const mavlink_param_union_t *p) = 0;
};