#pragma once

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define STX         0xAA
#define ETX         0xDD
#define SPACE       0x7D
#define CRC_SIZE    1


#ifndef Arduino_h
#define BUFFER_SIZE 256
#else
#define BUFFER_SIZE 128
#endif // Arduino_h

// Format packet: STX | Data... | CRC | ETX

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Calculate CRC with simple XOR checksum
 *
 * @param data Pointer to data array
 * @param length Length of data array (only data bytes, exclude STX, ETX and CRC byte)
 * @return uint8_t Calculated CRC value
 */
uint8_t calculateCRC(const uint8_t *data, uint8_t length);

/**
 * @brief Encoder data and escaping
 *
 * @param src Pointer to source data array
 * @param len Length of source data array
 * @param dst Pointer to destination data array
 * @return uint8_t Length of encoded data
 */
uint8_t encoderData(const uint8_t *src, uint8_t len, uint8_t *dst);

/**
 * @brief Decoder bytes data and escaping
 *
 * @param src Pointer to source data array
 * @param len Length of source data array
 * @param dst Pointer to destination data array
 * @return uint8_t Length of decoded data
 */
uint8_t decoderData(const uint8_t *src, uint8_t len, uint8_t *dst);

/**
 * @brief Add STX, ETX and CRC to data package and encode
 *
 * @param data Pointer to data array with only data bytes
 * @param len Length of data array
 * @param dst Pointer to destination data array
 * @return uint8_t Length of complete encoded package
 */
uint8_t encoderAllPackage(const uint8_t *data, uint8_t len, uint8_t *dst);

/**
 * @brief Decode data package by decoding escaped bytes, check CRC and remove it
 *
 * @param src Pointer to source data array with only data bytes and CRC
 * @param len Length of source data array
 * @param dst Pointer to destination data array
 * @return uint8_t Length of decoded data
 */
uint8_t decoderAllPackage(const uint8_t *src, uint8_t len, uint8_t *dst);

/**
 * @brief Handle received byte in a concurrent manner
 *
 * @param byte Received byte
 * @param dest Pointer to destination data array to store decoded data (without STX, ETX, CRC)
 * @return uint8_t Length of decoded data, or 0 if no complete package is received
 */
uint8_t handleRxByteConcurrent(uint8_t byte, uint8_t *dest);

#ifdef __cplusplus
}
#endif