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
                printf("Decoding error: SPACE at end of buffer.\n");
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
        printf("Invalid package size, discarding.\n");
        return 0;
    }

    uint8_t received_crc = buff[idx - 1];
    uint8_t calculated_crc = calculateCRC(buff, idx - 1);
    if (received_crc != calculated_crc) {
        printf("CRC mismatch, discarding package. Received: %02X, Calculated: %02X\n", received_crc,
               calculated_crc);
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
    static uint8_t str_buffer[BUFFER_SIZE] = {0};
    static uint8_t step = 0;
    static uint8_t buf_index = 0;
    static uint16_t str_index = 0;


    // step 1:
    switch (step) {
        case 0: // Wait STX
            if (byte == STX) {
                buf_index = 0;
                step = 1;
            } else {
                str_buffer[str_index++] = byte;
                // Check for buffer overflow
                if (str_index >= (BUFFER_SIZE-1)) {
                    // Print debug string
                    str_buffer[BUFFER_SIZE - 1] = '\0'; // Ensure null-termination
                    printf("UART_MSG: %s\n", str_buffer);
                    memset(str_buffer, 0, BUFFER_SIZE);
                    str_index = 0; // Reset on overflow
                }

                if (byte == '\n' || byte == '\r') {
                    // Print debug string
                    if (str_index > 1) {
                        printf("UART_MSG: %s\n", str_buffer);
                    }
                    memset(str_buffer, 0, BUFFER_SIZE);
                    str_index = 0; // Reset after printing
                }
            }
            break;
        case 1: // Read data until ETX

            // Check for buffer overflow
            if (buf_index >= (BUFFER_SIZE - 1)) {
                printf("Buffer overflow, discarding data.\n");
                step = 0;
                break;
            }

            // Check missing ETX
            if (byte == STX) {
                printf("Missing ETX, discarding data.\n");
                buf_index = 0;
                break;
            }

            if (byte == ETX) {
                // Complete package received
                uint8_t buf[BUFFER_SIZE] = {0};
                uint8_t decoded_length = decoderAllPackage(rx_buffer, buf_index, buf);

                // Check data length minimum length: Type
                if (decoded_length < 1) {
                    printf("Invalid package size, discarding.\n");
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
