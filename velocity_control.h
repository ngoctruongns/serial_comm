#pragma once

#include <stdint.h>

// Define data package types
enum {
    DEBUG_STRING = 0,
    CMD_VEL_COMMAND,
    WHEEL_ENC_COMMAND,
    PID_CONFIG_COMMAND,
    COMM_CTRL_COMMAND,
    LED_CONTROL_COMMAND,
    BUZZER_CONTROL_COMMAND,
    MOTOR_RPM_COMMAND,
    MAX_COMMAND_TYPE
};

// Define feedback data communication
enum {
    FEEDBACK_DEFAULT = 0,
    FEEDBACK_ENCODER = 1 << 0,
    FEEDBACK_MOTOR_RPM = 1 << 1,
    FEEDBACK_ALL = 0xFF
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

struct PIDConfigType {
    uint8_t type;
    float Kp;
    float Ki;
    float Kd;
} __attribute__((packed));

// Communication Control type
struct CommCtrlType {
    uint8_t feedback;
} __attribute__((packed));

// LED control command (solid, blink, rainbow, etc.)
struct LEDControlType {
    uint8_t type; // LED display type (solid, blink, rainbow, etc.)
    uint8_t r; // Red value
    uint8_t g; // Green value
    uint8_t b; // Blue value
    uint16_t param1; // Additional parameter 1 (e.g. blink delay)
    uint16_t param2; // Additional parameter 2 (e.g. breath period)
} __attribute__((packed));

// Buzzer control command
struct BuzzerControlType {
    uint8_t type; // Buzzer type (beep, blink, etc.)
    uint16_t param1; // Additional parameter 1 (e.g. beep duration)
    uint16_t param2; // Additional parameter 2 (e.g. blink on duration)
} __attribute__((packed));