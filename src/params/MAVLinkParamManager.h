#pragma once

#include <stdint.h>
#include "MAVLinkEndpoint.h"
#include "params/MAVLinkParamListener.h"
#include "params/MAVLinkParamNotifiable.h"

const uint8_t mavLinkParamLen   = 17;
const uint8_t maxNumParams      = 16;
const uint8_t maxNumNotifiers   = 16;

class MAVLinkParamManager
{
public:
    MAVLinkParamManager (
        MAVLinkEndpoint     *e,
        uint32_t            transmitDelayMS
    );
    virtual ~MAVLinkParamManager() {};
    virtual bool addParam(const char *name);
    virtual mavlink_param_union_t *getParam(const char *name);
    virtual mavlink_param_union_t *getParam(const uint8_t idx);    
    virtual int16_t getParamIdx(const char *name);
    virtual uint8_t getParamCount() { return mParamCount; };   
    virtual bool setParam(const char *name, const mavlink_param_union_t *val);
    virtual bool setParam(const uint8_t idx, const mavlink_param_union_t *val);
    virtual bool registerNotifier(const char *name, MAVLinkParamNotifiable *p);
    virtual bool registerNotifier(MAVLinkParamNotifiable *p);
    virtual void transmitParam(const char* name);
    virtual void transmitParam(const uint8_t idx);
    virtual void transmitParamList();

private:
    bool isNameEqual(char *compString);

    typedef struct paramElementStruct {
        char                    name[mavLinkParamLen];
        mavlink_param_union_t   value;  
    } paramElementStruct;

    paramElementStruct      mParams[maxNumParams];
    MAVLinkParamNotifiable  *mNotifiers[maxNumNotifiers];
    uint32_t                mTransmitDelayMS;
    uint8_t                 mParamCount;
    uint8_t                 mNotifierCount;
};