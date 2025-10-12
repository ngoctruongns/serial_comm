#pragma once

#include <stdint.h>

// Define data package types
enum {
    DEBUG_STRING = 0,
    CMD_VEL_COMMAND,
    WHEEL_ENC_COMMAND,
};

// Velocity command package format:
struct CmdVelType {
    uint8_t type;
    int16_t left_rpm;  // Left wheel RPM
    int16_t right_rpm; // Right wheel RPM
} __attribute__((packed));

struct WheelEncType {
    uint8_t type;
    int32_t left_enc;   // Left wheel encoder
    int32_t right_enc;  // Right wheel encoder
} __attribute__((packed));