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
    ODOMETRY_COMMAND,   // encoder + IMU raw, 50 Hz, for EKF
    MAX_COMMAND_TYPE
};

// Define feedback data communication
enum {
    FEEDBACK_DEFAULT = 0,
    FEEDBACK_ENCODER = 1 << 0,
    FEEDBACK_MOTOR_RPM = 1 << 1,
    FEEDBACK_ALL = 0xFF
};

// Define LED types for LED control command
typedef enum
{
    LED_TYPE_OFF        = 0,
    LED_TYPE_SOLID      = 1,
    LED_TYPE_BLINK      = 2,
    LED_TYPE_RAINBOW    = 3,
    LED_TYPE_BREATH     = 4,
    LED_TYPE_CHASE      = 5,
    LED_TYPE_MAX        = 6
} led_display_type_t;

// Define buzzer types for buzzer control command
typedef enum {
    BUZZER_TYPE_SOLID   = 0,
    BUZZER_TYPE_BLINK   = 1,
    BUZZER_TYPE_BEEP    = 2,
    BUZZER_TYPE_MAX     = 3
} BuzzerType;

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
    uint8_t type;
    uint8_t feedback;
} __attribute__((packed));

// LED control command (solid, blink, rainbow, etc.)
struct LEDControlType {
    uint8_t type;
    uint8_t led_type; // LED display type (solid, blink, rainbow, etc.)
    uint8_t r; // Red value
    uint8_t g; // Green value
    uint8_t b; // Blue value
    uint16_t param1; // Additional parameter 1 (e.g. blink delay)
    uint16_t param2; // Additional parameter 2 (e.g. breath period)
} __attribute__((packed));

// Buzzer control command
struct BuzzerControlType {
    uint8_t type;
    uint8_t buzzer_type; // Buzzer type (beep, blink, etc.)
    uint16_t param1; // Additional parameter 1 (e.g. beep duration)
    uint16_t param2; // Additional parameter 2 (e.g. blink on duration)
} __attribute__((packed));

// Odometry packet: wheel encoder + raw IMU, sent at 50 Hz
// timestamp_us: lower 32 bits of STM32 microsecond counter (wraps ~71 min)
// ax/ay/az, gx/gy/gz: raw register values — scale on the ROS2 side
struct OdometryType {
    uint8_t  type;           // ODOMETRY_COMMAND
    uint32_t timestamp_us;   // STM32 relative timestamp in microseconds
    int32_t  left_enc;       // left wheel encoder count
    int32_t  right_enc;      // right wheel encoder count
    int16_t  ax;             // accelerometer X (raw register)
    int16_t  ay;
    int16_t  az;
    int16_t  gx;             // gyroscope X (raw register)
    int16_t  gy;
    int16_t  gz;
} __attribute__((packed));