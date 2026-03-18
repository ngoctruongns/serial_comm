# serial_comm

UART packet framing/decoding library with the following frame format:

- `STX (0xAA) | payload | CRC(XOR) | ETX (0xDD)`
- Byte escaping uses `SPACE (0x7D)`.

## 1) Quick Integration (Platform-Agnostic)

This library is transport-agnostic and can be used with UART, USB CDC, TCP serial bridges, or desktop serial ports.

- TX path: build payload -> call `encoderAllPackage()` -> send encoded frame over your transport.
- RX path: feed bytes one by one into `handleRxByteConcurrent()`.
- When `handleRxByteConcurrent()` returns `> 0`, a full payload is ready in your destination buffer.
- Dispatch payload by `payload[0]` command ID.
- Schedule feedback packets (encoder/RPM) from your own app loop/timer.

## 2) Command Types (`payload[0]`)

Command IDs are defined in `velocity_control.h`:

- `CMD_VEL_COMMAND`
- `PID_CONFIG_COMMAND`
- `COMM_CTRL_COMMAND`
- `LED_CONTROL_COMMAND`
- `BUZZER_CONTROL_COMMAND`

### 2.1 `CMD_VEL_COMMAND`

Uses struct `CmdVelType`:

- `type`: `CMD_VEL_COMMAND`
- `left_rpm`: left wheel RPM
- `right_rpm`: right wheel RPM

### 2.2 `PID_CONFIG_COMMAND`

Uses struct `PIDConfigType`:

- `type`: `PID_CONFIG_COMMAND`
- `Kp`, `Ki`, `Kd`

### 2.3 `COMM_CTRL_COMMAND`

Payload format: `COMM_CTRL_COMMAND + CommCtrlType`

`CommCtrlType.feedback` supports:

- `FEEDBACK_DEFAULT`: encoder only
- `FEEDBACK_MOTOR_RPM`: encoder + motor RPM
- `FEEDBACK_ALL`: encoder + motor RPM

> Encoder feedback is always sent periodically by default; when `FEEDBACK_MOTOR_RPM` is enabled, an additional RPM feedback packet is sent on the same feedback cycle.

### 2.4 `LED_CONTROL_COMMAND`

Payload format: `LED_CONTROL_COMMAND + LEDControlType`

`LEDControlType`:

- `type`: display mode (`LED_TYPE_SOLID`, `LED_TYPE_BLINK`, `LED_TYPE_RAINBOW`, `LED_TYPE_BREATH`, `LED_TYPE_OFF`)
- `r,g,b`: color
- `param1,param2`: mode-specific parameters

Parameter mapping:

- `LED_TYPE_SOLID`: uses `r,g,b`
- `LED_TYPE_BLINK`: uses `r,g,b`, `param1 = ON(ms)`, `param2 = OFF(ms)`
- `LED_TYPE_RAINBOW`: uses `param1 = period/speed(ms)`
- `LED_TYPE_BREATH`: uses `r,g,b`, `param1 = period_ms`

### 2.5 `BUZZER_CONTROL_COMMAND`

Payload format: `BUZZER_CONTROL_COMMAND + BuzzerControlType`

Parameter mapping:

- `BUZZER_TYPE_SOLID`: buzzer ON continuously
- `BUZZER_TYPE_BEEP`: `param1 = ON(ms)`
- `BUZZER_TYPE_BLINK`: `param1 = ON(ms)`, `param2 = OFF(ms)`

## 3) Feedback Packets

### 3.1 Encoder (default)

Packet `WheelEncType`:

- `type = WHEEL_ENC_COMMAND`
- `left_enc`, `right_enc`

### 3.2 Motor RPM (when enabled)

Packet `CmdVelType` (reused for smaller payload size):

- `type = MOTOR_RPM_COMMAND`
- `left_rpm`, `right_rpm` (`int16_t`, rounded from measured RPM)

## 4) Compatibility Notes

- Struct layout and endianness between host and MCU must match (little-endian, packed).
- If the host is not C/C++, encode/decode by fields to avoid struct layout mismatch.
