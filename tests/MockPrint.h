#pragma once

#include <gmock/gmock.h>
#include <stdint.h>
#include "WString.h"

#define DEC 10
#define HEX 16
#define OCT 8
#ifdef BIN // Prevent warnings if BIN is previously defined in "iotnx4.h" or similar
#undef BIN
#endif
#define BIN 2

class Print;

using namespace testing;
// using namespace std;

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
    Print() : write_error(0) {}

    virtual int getWriteError() { return write_error; }
    virtual void clearWriteError() { setWriteError(0); }

    virtual size_t write(uint8_t) = 0;
    virtual size_t write(const char *str) {
        if (str == NULL) return 0;
        return write((const uint8_t *)str, strlen(str));
    }
    virtual size_t write(const uint8_t *buffer, size_t size);
    virtual size_t write(const char *buffer, size_t size) {
        return write((const uint8_t *)buffer, size);
    }

    // default to zero, meaning "a single write may block"
    // should be overridden by subclasses with buffering
    virtual int availableForWrite() { return 0; }

    virtual size_t print(const String &);
    virtual size_t print(const char[]);
    virtual size_t print(char);
    virtual size_t print(unsigned char, int = DEC);
    virtual size_t print(int, int = DEC);
    virtual size_t print(unsigned int, int = DEC);
    virtual size_t print(long, int = DEC);
    virtual size_t print(unsigned long, int = DEC);
    virtual size_t print(double, int = 2);
    virtual size_t print(const Printable&);

    virtual size_t println(const String &s);
    virtual size_t println(const char[]);
    virtual size_t println(char);
    virtual size_t println(unsigned char, int = DEC);
    virtual size_t println(int, int = DEC);
    virtual size_t println(unsigned int, int = DEC);
    virtual size_t println(long, int = DEC);
    virtual size_t println(unsigned long, int = DEC);
    virtual size_t println(double, int = 2);
    virtual size_t println(const Printable&);
    virtual size_t println(void);

    virtual void flush() { /* Empty implementation for backward compatibility */ }
};

class MockPrint : public Print
{
public:

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
    MOCK_METHOD((size_t), write, (const uint8_t *s, size_t len), (override));
    MOCK_METHOD(size_t, write, (uint8_t), (override));
    MOCK_METHOD(int, availableForWrite, (), (override)); 
    MOCK_METHOD(size_t, print, (const String& s), (override));
    MOCK_METHOD(size_t, print, (char c), (override));
    MOCK_METHOD(size_t, print, (char *c));
    MOCK_METHOD(size_t, print, (unsigned char c, int f), (override));
    MOCK_METHOD(size_t, print, (int c, int f), (override));
    MOCK_METHOD(size_t, print, (unsigned int c, int f), (override));
    MOCK_METHOD(size_t, print, (long c, int f), (override));
    MOCK_METHOD(size_t, print, (unsigned long c, int f), (override));
    MOCK_METHOD(size_t, print, (double c, int f), (override));
    MOCK_METHOD(size_t, print, (const Printable& p), (override));
    MOCK_METHOD(size_t, println, (const String& s), (override));
    MOCK_METHOD(size_t, println, (char c), (override));
    MOCK_METHOD(size_t, println, (char *c));
    MOCK_METHOD(size_t, println, (unsigned char c, int f), (override));
    MOCK_METHOD(size_t, println, (int c, int f), (override));
    MOCK_METHOD(size_t, println, (unsigned int c, int f), (override));
    MOCK_METHOD(size_t, println, (long c, int f), (override));
    MOCK_METHOD(size_t, println, (unsigned long c, int f), (override));
    MOCK_METHOD(size_t, println, (double c, int f), (override));
    MOCK_METHOD(size_t, println, (const Printable& p), (override));
};