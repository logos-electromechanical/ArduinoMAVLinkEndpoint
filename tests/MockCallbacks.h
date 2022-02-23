#include "MAVLinkEndpoint.h"
#include "receivers/MAVLinkRXCallback.h"
#include "transmitters/MAVLinkTXCallback.h"

class MockReceiver : public MAVLinkRXCallback
{
public:
    MockReceiver(uint32_t timeout, String name, MAVLinkEndpoint *e):
        MAVLinkRXCallback(timeout, name, e), timeout_count(0) {}
    MockReceiver(uint32_t timeout, uint32_t interval, String name, MAVLinkEndpoint *e):
        MAVLinkRXCallback(timeout, interval, name, e), timeout_count(0) {}
    virtual ~MockReceiver() {};
    void timeout(uint32_t t)
    {
        if (isTimeout(t)) { 
            timeout_count++;
            timeout_action(); 
        }
    }
    void set_last_rx(uint32_t v) { mLastRX = v; }
    MOCK_METHOD(bool, process, (mavlink_message_t, mavlink_status_t), (override));
    MOCK_METHOD(void, timeout_action, ());

    int timeout_count;
}