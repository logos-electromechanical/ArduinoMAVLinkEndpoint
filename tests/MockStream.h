#pragma once 

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <stdint.h>
#include "MockPrint.h"
#include "WString.h"
#include <queue>

using namespace std;

namespace ArduinoMockNamespace {

enum LookaheadMode{
    SKIP_ALL,       // All invalid characters are ignored.
    SKIP_NONE,      // Nothing is skipped, and the stream is not touched unless the first waiting character is valid.
    SKIP_WHITESPACE // Only tabs, spaces, line feeds & carriage returns are skipped.
};

#define NO_IGNORE_CHAR  '\x01' // a char not found in a valid ASCII numeric field

class Stream : public Print
{
protected:
    unsigned long _timeout;      // number of milliseconds to wait for the next char before aborting timed read
    unsigned long _startMillis;  // used for timeout measurement
    
public:
    virtual int available() = 0;
    virtual int read() = 0;
    virtual int peek() = 0;

    Stream();
    virtual ~Stream();

    // parsing methods

    virtual void setTimeout(unsigned long timeout) = 0;  // sets maximum milliseconds to wait for stream data, default is 1 second
    virtual unsigned long getTimeout(void) { return _timeout; }
    
    virtual bool find(char *target) = 0;   // reads data from the stream until the target string is found
    virtual bool find(uint8_t *target) { return find ((char *)target); }
    // returns true if target string is found, false if timed out (see setTimeout)

    virtual bool find(char *target, size_t length) = 0;   // reads data from the stream until the target string of given length is found
    virtual bool find(uint8_t *target, size_t length) { return find ((char *)target, length); }
    // returns true if target string is found, false if timed out

    virtual bool find(char target) { return find (&target, 1); }

    virtual bool findUntil(char *target, char *terminator) = 0;   // as find but search ends if the terminator string is found
    virtual bool findUntil(uint8_t *target, char *terminator) { return findUntil((char *)target, terminator); }

    virtual bool findUntil(char *target, size_t targetLen, char *terminate, size_t termLen) = 0;   // as above but search ends if the terminate string is found
    virtual bool findUntil(uint8_t *target, size_t targetLen, char *terminate, size_t termLen) {return findUntil((char *)target, targetLen, terminate, termLen); }

    virtual long parseInt(LookaheadMode lookahead = SKIP_ALL, char ignore = NO_IGNORE_CHAR) = 0;
    // returns the first valid (long) integer value from the current position.
    // lookahead determines how parseInt looks ahead in the stream.
    // See LookaheadMode enumeration at the top of the file.
    // Lookahead is terminated by the first character that is not a valid part of an integer.
    // Once parsing commences, 'ignore' will be skipped in the stream.

    virtual float parseFloat(LookaheadMode lookahead = SKIP_ALL, char ignore = NO_IGNORE_CHAR) = 0;
    // float version of parseInt

    virtual size_t readBytes( char *buffer, size_t length) = 0; // read chars from stream into buffer
    virtual size_t readBytes( uint8_t *buffer, size_t length) { return readBytes((char *)buffer, length); }
    // terminates if length characters have been read or timeout (see setTimeout)
    // returns the number of characters placed in the buffer (0 means no valid data found)

    virtual size_t readBytesUntil( char terminator, char *buffer, size_t length) = 0; // as readBytes with terminator character
    virtual size_t readBytesUntil( char terminator, uint8_t *buffer, size_t length) { return readBytesUntil(terminator, (char *)buffer, length); }
    // terminates if length characters have been read, timeout, or if the terminator character  detected
    // returns the number of characters placed in the buffer (0 means no valid data found)

    // Arduino String functions to be added here
    virtual String readString() = 0;
    virtual String readStringUntil(char terminator) = 0;

    protected:
    virtual long parseInt(char ignore) { return parseInt(SKIP_ALL, ignore); }
    virtual float parseFloat(char ignore) { return parseFloat(SKIP_ALL, ignore); }
    // These overload exists for compatibility with any class that has derived
    // Stream and used parseFloat/Int with a custom ignore character. To keep
    // the public API simple, these overload remains protected.

    struct MultiTarget {
        const char *str;  // string you're searching for
        size_t len;       // length of string you're searching for
        size_t index;     // index used by the search routine.
    };

    // This allows you to search for an arbitrary number of strings.
    // Returns index of the target that is found first or -1 if timeout occurs.
    virtual int findMulti(struct MultiTarget *targets, int tCount) = 0;
};

// We need to inherit MockPrint so that the stuff inherited from Print doesn't break the compile
class MockStream : public Stream
{
public:

    queue<uint8_t> stream_data;
    vector<uint8_t> write_data;

    MockStream();
    ~MockStream();
    long parseInt() { return parseInt (SKIP_ALL, NO_IGNORE_CHAR); }
    long parseInt(LookaheadMode l) { return parseInt (l, NO_IGNORE_CHAR); }
    float parseFloat() { return parseInt (SKIP_ALL, NO_IGNORE_CHAR); }
    float parseFloat(LookaheadMode l) { return parseInt (l, NO_IGNORE_CHAR); }
    int available() { return stream_data.size(); }
    int read() { int result = stream_data.front(); stream_data.pop(); return result; }
    int peek() { return stream_data.front(); }
    MOCK_METHOD(void, setTimeout, (unsigned long t), (override));
    MOCK_METHOD(unsigned long, getTimeout, (), (override));
    MOCK_METHOD(bool, find, (char *t), (override));
    MOCK_METHOD(bool, find, (char *t, size_t len), (override));
    MOCK_METHOD(bool, findUntil, (char*, char*), (override));
    MOCK_METHOD(bool, findUntil, (char*, size_t, char*, size_t), (override));
    MOCK_METHOD(long, parseInt, (LookaheadMode, char), (override));
    MOCK_METHOD(float, parseFloat, (LookaheadMode, char), (override));
    MOCK_METHOD(size_t, readBytes, (char*, size_t), (override));
    MOCK_METHOD(size_t, readBytesUntil, (char, char*, size_t), (override));
    MOCK_METHOD(String, readString, (), (override));
    MOCK_METHOD(String, readStringUntil, (char), (override));
    MOCK_METHOD(size_t, write, (uint8_t), (override));
    MOCK_METHOD(int, findMulti, (struct MultiTarget*, int), (override));

    // mocking all the Print interface functions so this thing will link
    size_t print(unsigned char c) { return print(c, DEC); }
    size_t print(int c) { return print(c, DEC); }
    size_t print(unsigned int c) { return print(c, DEC); }
    size_t print(long c) { return print(c, DEC); }
    size_t print(unsigned long c) { return print(c, DEC); }
    size_t print(double c) { return print(c, DEC); }
    size_t println(unsigned char c) { return print(c, DEC); }
    size_t println(int c) { return print(c, DEC); }
    size_t println(unsigned int c) { return print(c, DEC); }
    size_t println(long c) { return print(c, DEC); }
    size_t println(unsigned long c) { return print(c, DEC); }
    size_t println(double c) { return print(c, DEC); }
    size_t write(const uint8_t *buffer, size_t size)
    {
        for (int i = 0; i < size; i++) { write_data.push_back(buffer[i]); }
        // This means we can always use the mocked version to check how many times it's called. 
        return write(size);
    }
    MOCK_METHOD(int, availableForWrite, (), (override)); 
    MOCK_METHOD(size_t, print, (const String&), (override));
    MOCK_METHOD(size_t, print, (char), (override));
    MOCK_METHOD(size_t, print, (const char*), (override));
    MOCK_METHOD(size_t, print, (unsigned char, int), (override));
    MOCK_METHOD(size_t, print, (int, int), (override));
    MOCK_METHOD(size_t, print, (unsigned int, int), (override));
    MOCK_METHOD(size_t, print, (long, int), (override));
    MOCK_METHOD(size_t, print, (unsigned long, int), (override));
    MOCK_METHOD(size_t, print, (double, int), (override));
    MOCK_METHOD(size_t, print, (const Printable&), (override));
    MOCK_METHOD(size_t, println, (const String&), (override));
    MOCK_METHOD(size_t, println, (char), (override));
    MOCK_METHOD(size_t, println, (const char*));
    MOCK_METHOD(size_t, println, (unsigned char c, int f), (override));
    MOCK_METHOD(size_t, println, (int, int), (override));
    MOCK_METHOD(size_t, println, (unsigned int, int f), (override));
    MOCK_METHOD(size_t, println, (long, int), (override));
    MOCK_METHOD(size_t, println, (unsigned long, int), (override));
    MOCK_METHOD(size_t, println, (double, int), (override));
    MOCK_METHOD(size_t, println, (const Printable&), (override));
    MOCK_METHOD(size_t, println, (), (override));
    MOCK_METHOD(size_t, printTo, (Print &));
};

}