#include "MAVLinkEndpoint.h"

bool MAVLinkEndpoint::registerRXCallback(MAVLinkRXCallback* cb)
{
    // If we're full up on receive callbacks, return false
    if (mRXCallbackCount >= MAVLINK_RX_MAX_HANDLER_COUNT) return false;

    // If we were handed a nullptr, return false
    if (cb == nullptr) return false;

    // Add the incoming pointer to our list of receive callbacks and return true
    mRXCallbacks[mRXCallbackCount] = cb;
    mRXCallbackCount++;
    return true;
}

bool MAVLinkEndpoint::registerRXDefaultCallback(MAVLinkRXCallback* cb)
{
    // Register a receive object to call if we receive a message we don't otherwise understand
    if (cb == nullptr) return false;
    mRXDefaultCallback = cb;
    return true;
}

bool MAVLinkEndpoint::registerTXCallback(MAVLinkTXCallback* cb)
{
    if (mTXCallbackCount >= MAVLINK_TX_MAX_HANDLER_COUNT) return false;
    if (cb == nullptr) return false;
    mTXCallbacks[mTXCallbackCount] = cb;
    mTXCallbackCount++;
    return true;
}

bool MAVLinkEndpoint::registerWriter(MAVlinkTXWriter_t w)
{
    if (w == nullptr) return false;
    mWriter = w;
    mOutputPrint = nullptr;
    return true;
}


bool MAVLinkEndpoint::registerRXStream(Stream *s)
{
    if (s == nullptr) return false;
    mInputStream = s;
    return true;
}

bool MAVLinkEndpoint::registerTXPrint(Print *p)
{
    if (p == nullptr) return false;
    mOutputPrint = p;
    mWriter = nullptr;
    return true;
}

bool MAVLinkEndpoint::registerDebugPrint(Print *p)
{
    if (p == nullptr) return false;
    mDebugPrint = p;
    return true;
}


bool MAVLinkEndpoint::process(uint8_t c)
{
    bool rcvd = false;
    if (mavlink_parse_char(MAVLINK_COMM_0, c, &msg, &stat))
    {
        if (mDebugPrint) mDebugPrint->println("Packet received!");
        bool result = false;
        rcvd = true;
        for (uint8_t i = 0; i < mRXCallbackCount; i++) 
        {
            result |= mRXCallbacks[i]->process(&mMessage, &mStatus));
        }
        if (!result && (mRXDefaultCallback != nullptr)) 
        {
            mRXDefaultCallback->process(&mMessage, &mStatus);
        }
    }
    if (mDebugPrint && mStatus.parse_state == 14) 
    {
        mDebugPrint->println("\n\nReceived bad CRC\n");
    }
    this->tick();
    return rcvd;
}

bool MAVLinkEndpoint::process(uint8_t *data, size_t len)
{
    bool rcvd = false;
    for (int i = 0; i < len; i++) 
    {
        rcvd |= mavlink_parse_char(MAVLINK_COMM_0, data[i], &mMessage, &mStatus);
    }
    return rcvd;
}

size_t MAVLinkEndpoint::transmit(mavlink_message_t *msg_ptr)
{
    uint8_t buf[MAVLINK_MAX_PACKET_LEN];                        // allocate an outbound buffer
    uint16_t len = mavlink_msg_to_send_buffer(buf, msg_ptr);    // pack the outbound message up into a buffer
    if (mWriter)                                                // if we've registered a writer function, call it and return the number of characters written
    {
        return this->mWriter(buf, len);
    } 
    else if (mOutputPrint)                                      // if we've registered a Print object, write to it
    {
        return mOutputPrint->write(buf, len);
    }
    else                                                        // Otherwise nothing happened, so return zero
    {
        return 0;
    }
}

bool MAVLinkEndpoint::requestMessage(uint8_t msg_src, uint8_t msg_id, uint32_t interval)
{
     mavlink_message_t msg;
     if (mDebugPrint) 
     {
         mDebugPrint->print("Requesting message ");
         mDebugPrint->println(msg_id);
     }
     mavlink_msg_command_long_pack(mComponentID, mSystemID, &msg, mSystemID, 
        msg_src, MAV_CMD_SET_MESSAGE_INTERVAL, 0, msg_id, interval, 0, 0, 0, 0, 1);
     return this->transmit(&msg);
}

void MAVLinkEndpoint::tick()
{
    for (int i = 0; i < mTXCallbackCount; i++) 
    {
        // Call the tick function for all of the transmitters
        mTXCallbacks[i]->tick(millis());
    }
    for (int i = 0; i < mRXCallbackCount; i++) 
    {   
        // Check if the receivers have timed out
        mRXCallbacks[i]->timeout(millis());
    }
}

bool MAVLinkEndpoint::poll()
{
    if (!mInputStream) return false;        // If we haven't registered an input stream, return false  
    bool rcvd = false;
    while (mInputStream->available())       // While there are bytes available in the input stream...
    {
        uint8_t c = mInputStream->read()    // Read in a byte
        rcvd |= process(c);                 // process the byte and if there's a packet, set the return value true
    }
    return rcvd;
}