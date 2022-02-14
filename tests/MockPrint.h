#pragma once

#include <stdint.h>

#define DEC 10
#define HEX 16
#define OCT 8
#ifdef BIN // Prevent warnings if BIN is previously defined in "iotnx4.h" or similar
#undef BIN
#endif
#define BIN 2

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

    virtual size_t print(const __FlashStringHelper *);
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

    virtual size_t println(const __FlashStringHelper *);
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