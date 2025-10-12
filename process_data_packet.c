#include "process_data_packet.h"

uint8_t calculateCRC(const uint8_t *data, uint8_t length)
{
    uint8_t crc = 0;
    for (uint8_t i = 0; i < length; ++i) {
        crc ^= data[i];
    }
    return crc;
}

uint8_t encoderData(const uint8_t *src, uint8_t len, uint8_t *dst)
{
    uint8_t idx = 0;

    for (uint8_t i = 0; i < len; i++) {
        uint8_t b = src[i];
        if (b == STX || b == ETX || b == SPACE) {
            dst[idx++] = SPACE;
            dst[idx++] = b ^ SPACE; // escape
        } else {
            dst[idx++] = b;
        }
    }

    return idx;
}

uint8_t decoderData(const uint8_t *src, uint8_t len, uint8_t *dst)
{
    uint8_t idx = 0;

    for (uint8_t i = 0; i < len; i++) {
        uint8_t b = src[i];
        if (b == SPACE) {
            if (i + 1 >= len) {
#ifndef __ARDUINO__
                printf("Decoding error: SPACE at end of buffer.\n");
#endif // __ARDUINO__
                break;
            }
            dst[idx++] = src[++i] ^ SPACE; // unescape
        } else {
            dst[idx++] = b;
        }
    }
    return idx;
}

uint8_t encoderAllPackage(const uint8_t *data, uint8_t len, uint8_t *dst)
{
    uint8_t idx = 0;
    uint8_t crc = calculateCRC(data, len);

    // Create full package
    dst[idx++] = STX; // Start byte

    // Encode data
    idx += encoderData(data, len, &dst[idx]);

    // Encode CRC
    idx += encoderData(&crc, CRC_SIZE, &dst[idx]);

    dst[idx++] = ETX; // End byte

    return idx;
}

uint8_t decoderAllPackage(const uint8_t *src, uint8_t len, uint8_t *dst)
{
    uint8_t idx = 0;
    uint8_t buff[BUFFER_SIZE] = {0};

    // Decode data
    idx = decoderData(src, len, buff);

    // Check CRC validity
    if (idx < 2) {
#ifndef __ARDUINO__
        printf("Invalid package size, discarding.\n");
#endif // __ARDUINO__
        return 0;
    }

    uint8_t received_crc = buff[idx - 1];
    uint8_t calculated_crc = calculateCRC(buff, idx - 1);
    if (received_crc != calculated_crc) {
#ifndef __ARDUINO__
        printf("CRC mismatch, discarding package. Received: %02X, Calculated: %02X\n", received_crc,
               calculated_crc);
#endif // __ARDUINO__
        return 0;
    }

    // Copy to destination without CRC
    idx -= 1; // Exclude CRC byte
    memcpy(dst, buff, idx);

    return idx;
}

// Get a single byte and process it
uint8_t handleRxByteConcurrent(uint8_t byte, uint8_t *dest)
{
    // Buffer for reading only serial data (not STX, ETX)
    static uint8_t rx_buffer[BUFFER_SIZE] = {0};
    static uint8_t step = 0;
    static uint8_t buf_index = 0;

    // step 1:
    switch (step) {
        case 0: // Wait STX
            if (byte == STX) {
                buf_index = 0;
                step = 1;
            }
            break;
        case 1: // Read data until ETX

            // Check for buffer overflow
            if (buf_index >= (BUFFER_SIZE - 1)) {
#ifndef __ARDUINO__
                printf("Buffer overflow, discarding data.\n");
#endif // __ARDUINO__
                step = 0;
                break;
            }

            // Check missing ETX
            if (byte == STX) {
#ifndef __ARDUINO__
                printf("Missing ETX, discarding data.\n");
#endif // __ARDUINO__
                buf_index = 0;
                break;
            }

            if (byte == ETX) {
                // Complete package received
                uint8_t buf[BUFFER_SIZE] = {0};
                uint8_t decoded_length = decoderAllPackage(rx_buffer, buf_index, buf);

                // Check data length minimum length: Type
                if (decoded_length < 1) {
#ifndef __ARDUINO__
                    printf("Invalid package size, discarding.\n");
#endif // __ARDUINO__
                    step = 0;
                    break;
                } else {
                    // Copy decoded data to destination
                    memcpy(dest, buf, decoded_length);
                    step = 0;              // Reset for next package
                    return decoded_length; // Return length of decoded data
                }
            }

            // Save byte validated to buffer
            rx_buffer[buf_index++] = byte;

            break;
        default:
            step = 0; // Reset on unexpected state
            break;
    }

    return 0;
}
