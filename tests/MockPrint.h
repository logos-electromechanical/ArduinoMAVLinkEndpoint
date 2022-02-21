#pragma once

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <stdint.h>
#include "WString.h"
#include <vector>

#define DEC 10
#define HEX 16
#define OCT 8
#ifdef BIN // Prevent warnings if BIN is previously defined in "iotnx4.h" or similar
#undef BIN
#endif
#define BIN 2

namespace ArduinoMockNamespace {

class Print;

using namespace testing;
using namespace std;

class Printable
{
public:
    virtual size_t printTo(Print& p) const = 0;
};

class Print
{
private:
    int write_error;
    void setWriteError(int e) { write_error = e; }
public:
    Print();
    virtual ~Print();

    virtual int getWriteError() { return write_error; }
    virtual void clearWriteError() { setWriteError(0); }

    virtual size_t write(uint8_t) = 0;
    virtual size_t write(const char *str) {
        if (str == NULL) return 0;
        return write((const uint8_t *)str, strlen(str));
    }
    virtual size_t write(const uint8_t *buffer, size_t size) = 0;
    virtual size_t write(const char *buffer, size_t size) {
        return write((const uint8_t *)buffer, size);
    }

    // default to zero, meaning "a single write may block"
    // should be overridden by subclasses with buffering
    virtual int availableForWrite() { return 0; }

    virtual size_t print(const String &) = 0;
    virtual size_t print(const char*) = 0;
    virtual size_t print(char) = 0;
    virtual size_t print(unsigned char, int = DEC) = 0;
    virtual size_t print(int, int = DEC) = 0;
    virtual size_t print(unsigned int, int = DEC) = 0;
    virtual size_t print(long, int = DEC) = 0;
    virtual size_t print(unsigned long, int = DEC) = 0;
    virtual size_t print(double, int = 2) = 0;
    virtual size_t print(const Printable&) = 0;

    virtual size_t println(const String &s) = 0;
    virtual size_t println(const char*) = 0;
    virtual size_t println(char) = 0;
    virtual size_t println(unsigned char, int = DEC) = 0;
    virtual size_t println(int, int = DEC) = 0;
    virtual size_t println(unsigned int, int = DEC) = 0;
    virtual size_t println(long, int = DEC) = 0;
    virtual size_t println(unsigned long, int = DEC) = 0;
    virtual size_t println(double, int = 2) = 0;
    virtual size_t println(const Printable&) = 0;
    virtual size_t println(void) = 0;

    virtual void flush() { /* Empty implementation for backward compatibility */ }
};

class MockPrint : public Print
{
public:
    MockPrint();
    ~MockPrint();
    vector<uint8_t> write_data;
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
    MOCK_METHOD(size_t, write, (uint8_t), (override));
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