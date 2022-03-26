#pragma once

#include "MockPrint.h"
#include "MockStream.h"
#include "WString.h"

extern uint32_t currentMillis;

inline uint32_t millis() { return currentMillis; }

inline void set_millis(uint32_t v) { currentMillis = v; }