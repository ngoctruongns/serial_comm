#pragma once

#ifndef ARDUINO
    #include <stdio.h>
    #define LOG(fmt, ...) printf(fmt, ##__VA_ARGS__)
#elif defined(ENABLE_LOG)
    #include <Arduino.h>
    #define LOG(fmt, ...) Serial.printf(fmt, ##__VA_ARGS__)
#else
    #define LOG(fmt, ...) ((void)0)
#endif
