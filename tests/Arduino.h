#pragma once

#include "MockPrint.h"
#include "MockStream.h"
#include "WString.h"

static uint32_t currentMillis = 0;

inline uint32_t millis() {return currentMillis;}