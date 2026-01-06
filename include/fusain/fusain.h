/*
 * Copyright (c) 2025 Kaz Walker, Thermoquad
 * SPDX-License-Identifier: Apache-2.0
 *
 * Fusain Serial Protocol - Shared Library
 *
 * This library provides reusable functions for encoding/decoding Fusain serial
 * protocol packets. Can be used by both ICU (slave) and controller (master).
 */

#ifndef FUSAIN_H_
#define FUSAIN_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Protocol Constants */
#define FUSAIN_START_BYTE 0x7E
#define FUSAIN_END_BYTE 0x7F
#define FUSAIN_ESC_BYTE 0x7D
#define FUSAIN_ESC_XOR 0x20

#define FUSAIN_MAX_PACKET_SIZE 128
#define FUSAIN_MAX_PAYLOAD_SIZE 114
#define FUSAIN_MIN_PACKET_SIZE 14 // START + LEN + ADDR(8) + TYPE + CRC(2) + END

/* Message Type Definitions */
typedef enum {
  /* Configuration Commands (Controller → Appliance) 0x10-0x1F */
  FUSAIN_MSG_MOTOR_CONFIG = 0x10,
  FUSAIN_MSG_PUMP_CONFIG = 0x11,
  FUSAIN_MSG_TEMP_CONFIG = 0x12,
  FUSAIN_MSG_GLOW_CONFIG = 0x13,
  FUSAIN_MSG_DATA_SUBSCRIPTION = 0x14,
  FUSAIN_MSG_DATA_UNSUBSCRIBE = 0x15,
  FUSAIN_MSG_TELEMETRY_CONFIG = 0x16,
  FUSAIN_MSG_DISCOVERY_REQUEST = 0x1F,

  /* Control Commands (Controller → Appliance) 0x20-0x2F */
  FUSAIN_MSG_STATE_COMMAND = 0x20,
  FUSAIN_MSG_MOTOR_COMMAND = 0x21,
  FUSAIN_MSG_PUMP_COMMAND = 0x22,
  FUSAIN_MSG_GLOW_COMMAND = 0x23,
  FUSAIN_MSG_TEMP_COMMAND = 0x24,
  FUSAIN_MSG_PING_REQUEST = 0x2F,

  /* Telemetry Data (Appliance → Controller) 0x30-0x3F */
  FUSAIN_MSG_STATE_DATA = 0x30,
  FUSAIN_MSG_MOTOR_DATA = 0x31,
  FUSAIN_MSG_PUMP_DATA = 0x32,
  FUSAIN_MSG_GLOW_DATA = 0x33,
  FUSAIN_MSG_TEMP_DATA = 0x34,
  FUSAIN_MSG_TELEMETRY_BUNDLE = 0x35,
  FUSAIN_MSG_DEVICE_ANNOUNCE = 0x36,
  FUSAIN_MSG_PING_RESPONSE = 0x3F,

  /* Error Messages (Bidirectional) 0xE0-0xEF */
  FUSAIN_MSG_ERROR_INVALID_MSG = 0xE0,
  FUSAIN_MSG_ERROR_CRC_FAIL = 0xE1,
  FUSAIN_MSG_ERROR_INVALID_CMD = 0xE2,
  FUSAIN_MSG_ERROR_STATE_REJECT = 0xE3,
} fusain_msg_type_t;

/* Operating Modes */
typedef enum {
  FUSAIN_MODE_IDLE = 0x00,
  FUSAIN_MODE_FAN = 0x01,
  FUSAIN_MODE_HEAT = 0x02,
  FUSAIN_MODE_EMERGENCY = 0xFF,
} fusain_mode_t;

/* State Values (from state machine) */
typedef enum {
  FUSAIN_STATE_INITIALIZING = 0x00,
  FUSAIN_STATE_IDLE = 0x01,
  FUSAIN_STATE_BLOWING = 0x02,
  FUSAIN_STATE_PREHEAT = 0x03,
  FUSAIN_STATE_PREHEAT_STAGE_2 = 0x04,
  FUSAIN_STATE_HEATING = 0x05,
  FUSAIN_STATE_COOLING = 0x06,
  FUSAIN_STATE_ERROR = 0x07,
  FUSAIN_STATE_E_STOP = 0x08,
} fusain_state_t;

/* Error Codes */
typedef enum {
  FUSAIN_ERROR_NONE = 0x00,
  FUSAIN_ERROR_PREHEAT_FAILED = 0x01,
  FUSAIN_ERROR_FLAME_OUT = 0x02,
  FUSAIN_ERROR_OVERHEAT = 0x03,
  FUSAIN_ERROR_PUMP_FAULT = 0x04,
  FUSAIN_ERROR_TIMEOUT = 0x05,
} fusain_error_t;

/* Command Payloads */
typedef struct __attribute__((packed)) {
  uint8_t mode; // fusain_mode_t
  uint32_t parameter; // RPM for FAN mode, 0 for others
} fusain_cmd_set_mode_t;

typedef struct __attribute__((packed)) {
  uint32_t rate_ms; // Pump interval in milliseconds
} fusain_cmd_set_pump_rate_t;

typedef struct __attribute__((packed)) {
  uint32_t target_rpm;
} fusain_cmd_set_target_rpm_t;

typedef struct __attribute__((packed)) {
  int32_t glow; // Glow plug index (0-9, typically 0)
  int32_t duration; // Burn duration in milliseconds (0-300000)
} fusain_cmd_glow_t;

typedef struct __attribute__((packed)) {
  uint32_t telemetry_enabled; // 0 = disabled, 1 = enabled
  uint32_t interval_ms; // Telemetry broadcast interval (100-5000 ms)
  uint32_t telemetry_mode; // 0 = bundled, 1 = individual
} fusain_cmd_telemetry_config_t;

/* Configuration Command Payloads (v2.0) */
typedef struct __attribute__((packed)) {
  int32_t motor;
  uint32_t pwm_period;
  double pid_kp;
  double pid_ki;
  double pid_kd;
  int32_t max_rpm;
  int32_t min_rpm;
  uint32_t min_pwm_duty;
  uint8_t padding[4];
} fusain_cmd_motor_config_t;

typedef struct __attribute__((packed)) {
  int32_t pump;
  uint32_t min_rate_ms;
  uint32_t max_rate_ms;
  uint8_t padding[4];
} fusain_cmd_pump_config_t;

typedef struct __attribute__((packed)) {
  int32_t thermometer;
  double pid_kp;
  double pid_ki;
  double pid_kd;
  uint32_t sample_count;
  uint32_t read_rate;
  uint8_t padding[12];
} fusain_cmd_temp_config_t;

typedef struct __attribute__((packed)) {
  int32_t glow;
  uint32_t max_duration_ms;
  uint8_t padding[8];
} fusain_cmd_glow_config_t;

typedef struct __attribute__((packed)) {
  uint64_t appliance_address;
  uint64_t message_filter;
} fusain_cmd_data_subscription_t;

typedef struct __attribute__((packed)) {
  uint64_t appliance_address;
} fusain_cmd_data_unsubscribe_t;

/* Data Payloads */
typedef struct __attribute__((packed)) {
  uint32_t state; // fusain_state_t
  uint8_t error; // fusain_error_t
} fusain_data_state_t;

typedef struct __attribute__((packed)) {
  int32_t rpm;
  int32_t target_rpm;
  int32_t pwm_duty;
  int32_t pwm_period;
  int32_t min_rpm;
  int32_t max_rpm;
} fusain_data_motor_t;

typedef struct __attribute__((packed)) {
  double temperature;
  uint8_t pid_enabled;
  double pid_setpoint;
  int32_t pid_output_rpm;
} fusain_data_temperature_t;

typedef struct __attribute__((packed)) {
  uint8_t enabled;
  uint32_t rate_ms;
  uint64_t pulse_count;
} fusain_data_pump_t;

typedef struct __attribute__((packed)) {
  uint8_t lit;
  uint64_t lit_timestamp;
  uint64_t total_burn_time;
} fusain_data_glow_t;

typedef struct __attribute__((packed)) {
  uint64_t uptime_ms;
} fusain_data_ping_response_t;

typedef struct __attribute__((packed)) {
  uint32_t device_type;
  uint32_t capabilities;
} fusain_data_device_announce_t;

/* Variable-length telemetry bundle */
#define FUSAIN_MAX_MOTORS 5
#define FUSAIN_MAX_TEMPERATURES 4

typedef struct __attribute__((packed)) {
  int32_t rpm;
  int32_t target_rpm;
  int32_t pwm_duty;
  int32_t pwm_period;
} fusain_telemetry_motor_t;

typedef struct __attribute__((packed)) {
  double temperature;
} fusain_telemetry_temperature_t;

typedef struct __attribute__((packed)) {
  uint32_t state; // fusain_state_t
  uint8_t error; // fusain_error_t
  uint8_t motor_count; // Number of motors (1-3)
  uint8_t temp_count; // Number of temperature sensors (1-3)
  // Followed by:
  // - motor_count × fusain_telemetry_motor_t
  // - temp_count × fusain_telemetry_temperature_t
} fusain_data_telemetry_bundle_t;

/* Error Message Payloads */
typedef struct __attribute__((packed)) {
  uint8_t invalid_command;
} fusain_error_invalid_command_t;

typedef struct __attribute__((packed)) {
  uint16_t received_crc;
  uint16_t calculated_crc;
} fusain_error_invalid_crc_t;

typedef struct __attribute__((packed)) {
  uint8_t received_length;
  uint8_t expected_length;
} fusain_error_invalid_length_t;

/* Packet Structure */
typedef struct {
  uint8_t start; // FUSAIN_START_BYTE
  uint8_t length; // Payload length (0-114)
  uint64_t address; // Device address
  uint8_t msg_type; // fusain_msg_type_t
  uint8_t payload[FUSAIN_MAX_PAYLOAD_SIZE];
  uint16_t crc; // CRC-16-CCITT
  uint8_t end; // FUSAIN_END_BYTE
} fusain_packet_t;

/* Decode Result */
typedef enum {
  FUSAIN_DECODE_OK = 0,
  FUSAIN_DECODE_INCOMPLETE = 1,
  FUSAIN_DECODE_INVALID_START = 2,
  FUSAIN_DECODE_INVALID_CRC = 3,
  FUSAIN_DECODE_INVALID_LENGTH = 4,
  FUSAIN_DECODE_BUFFER_OVERFLOW = 5,
} fusain_decode_result_t;

/* Decoder State */
typedef struct {
  uint8_t state; // Internal state machine state
  uint8_t buffer[FUSAIN_MAX_PACKET_SIZE]; // Internal decode buffer
  size_t buffer_index; // Current position in buffer
  bool escape_next; // Escape sequence flag
  uint8_t addr_byte_count; // Number of address bytes received (0-8)
} fusain_decoder_t;

/* Function Declarations */

/**
 * Calculate CRC-16-CCITT (polynomial 0x1021, initial 0xFFFF)
 *
 * @param data Pointer to data buffer
 * @param length Length of data in bytes
 * @return CRC-16 value
 */
uint16_t fusain_crc16(const uint8_t* data, size_t length);

/**
 * Encode a packet into a byte buffer with byte stuffing
 *
 * @param packet Packet structure to encode
 * @param buffer Output buffer for encoded bytes
 * @param buffer_size Size of output buffer
 * @return Number of bytes written, or negative error code
 */
int fusain_encode_packet(const fusain_packet_t* packet, uint8_t* buffer,
    size_t buffer_size);

/**
 * Decode received bytes into a packet structure
 *
 * This function handles byte unstuffing and CRC validation.
 * Call repeatedly as bytes are received until FUSAIN_DECODE_OK or error.
 *
 * @param rx_byte Received byte to process
 * @param packet Output packet structure
 * @param decoder Decoder state (initialize with fusain_reset_decoder)
 * @return Decode result status
 */
fusain_decode_result_t fusain_decode_byte(uint8_t rx_byte,
    fusain_packet_t* packet,
    fusain_decoder_t* decoder);

/**
 * Reset decoder state
 *
 * @param decoder Decoder state to reset
 */
void fusain_reset_decoder(fusain_decoder_t* decoder);

/**
 * Create a SET_MODE command packet
 *
 * @param packet Output packet
 * @param address Device address
 * @param mode Operating mode
 * @param parameter Mode parameter (RPM for FAN mode)
 */
void fusain_create_set_mode(fusain_packet_t* packet, uint64_t address,
    fusain_mode_t mode, uint32_t parameter);

/**
 * Create a SET_PUMP_RATE command packet
 *
 * @param packet Output packet
 * @param address Device address
 * @param rate_ms Pump rate in milliseconds
 */
void fusain_create_set_pump_rate(fusain_packet_t* packet, uint64_t address,
    uint32_t rate_ms);

/**
 * Create a SET_TARGET_RPM command packet
 *
 * @param packet Output packet
 * @param address Device address
 * @param target_rpm Target RPM
 */
void fusain_create_set_target_rpm(fusain_packet_t* packet, uint64_t address,
    uint32_t target_rpm);

/**
 * Create a GLOW_COMMAND packet
 *
 * @param packet Output packet
 * @param address Device address
 * @param glow Glow plug index (0-9, typically 0)
 * @param duration Burn duration in milliseconds (0-300000)
 */
void fusain_create_glow_command(fusain_packet_t* packet, uint64_t address, int32_t glow,
    int32_t duration);

/**
 * Create a PING_REQUEST packet
 *
 * @param packet Output packet
 */
void fusain_create_ping_request(fusain_packet_t* packet, uint64_t address);

/**
 * Create a TELEMETRY_CONFIG packet
 *
 * @param packet Output packet
 * @param address Device address
 * @param enabled Telemetry broadcast enabled (0=disabled, 1=enabled)
 * @param interval_ms Telemetry broadcast interval (100-5000 ms)
 * @param mode Telemetry mode (0=bundled, 1=individual)
 */
void fusain_create_telemetry_config(fusain_packet_t* packet, uint64_t address, bool enabled,
    uint32_t interval_ms, uint32_t mode);

/**
 * Create a MOTOR_CONFIG packet (v2.0)
 *
 * @param packet Output packet
 * @param config Motor configuration parameters
 */
void fusain_create_motor_config(fusain_packet_t* packet, uint64_t address,
    const fusain_cmd_motor_config_t* config);

/**
 * Create a PUMP_CONFIG packet (v2.0)
 *
 * @param packet Output packet
 * @param config Pump configuration parameters
 */
void fusain_create_pump_config(fusain_packet_t* packet, uint64_t address,
    const fusain_cmd_pump_config_t* config);

/**
 * Create a TEMP_CONFIG packet (v2.0)
 *
 * @param packet Output packet
 * @param config Temperature configuration parameters
 */
void fusain_create_temp_config(fusain_packet_t* packet, uint64_t address,
    const fusain_cmd_temp_config_t* config);

/**
 * Create a GLOW_CONFIG packet (v2.0)
 *
 * @param packet Output packet
 * @param config Glow plug configuration parameters
 */
void fusain_create_glow_config(fusain_packet_t* packet, uint64_t address,
    const fusain_cmd_glow_config_t* config);

/**
 * Create a DATA_SUBSCRIPTION packet (v2.0)
 *
 * @param packet Output packet
 * @param appliance_address Address of appliance to subscribe to
 * @param message_filter Message type filter bitmask
 */
void fusain_create_data_subscription(fusain_packet_t* packet, uint64_t address,
    uint64_t appliance_address, uint64_t message_filter);

/**
 * Create a DATA_UNSUBSCRIBE packet (v2.0)
 *
 * @param packet Output packet
 * @param appliance_address Address of appliance to unsubscribe from
 */
void fusain_create_data_unsubscribe(fusain_packet_t* packet, uint64_t address,
    uint64_t appliance_address);

/**
 * Create a DISCOVERY_REQUEST packet (v2.0)
 *
 * @param packet Output packet
 */
void fusain_create_discovery_request(fusain_packet_t* packet, uint64_t address);

/**
 * Create a STATE_DATA packet
 *
 * @param packet Output packet
 * @param address Device address
 * @param state Current state
 * @param error Error code
 */
void fusain_create_state_data(fusain_packet_t* packet, uint64_t address, fusain_state_t state,
    fusain_error_t error);

/**
 * Create a PING_RESPONSE packet
 *
 * @param packet Output packet
 * @param uptime_ms System uptime in milliseconds
 */
void fusain_create_ping_response(fusain_packet_t* packet, uint64_t address, uint64_t uptime_ms);

/**
 * Create a TELEMETRY_BUNDLE packet
 *
 * @param packet Output packet
 * @param address Device address
 * @param state Current state
 * @param error Error code
 * @param motors Array of motor data
 * @param motor_count Number of motors (1-5)
 * @param temperatures Array of temperature data
 * @param temp_count Number of temperatures (1-4)
 * @return 0 on success, negative on error
 */
int fusain_create_telemetry_bundle(fusain_packet_t* packet, uint64_t address,
    fusain_state_t state, fusain_error_t error,
    const fusain_telemetry_motor_t* motors,
    uint8_t motor_count,
    const fusain_telemetry_temperature_t* temperatures,
    uint8_t temp_count);

/**
 * Create a DEVICE_ANNOUNCE packet (v2.0)
 *
 * @param packet Output packet
 * @param address Device address
 * @param device_type Device type identifier
 * @param capabilities Capabilities bitmask
 */
void fusain_create_device_announce(fusain_packet_t* packet, uint64_t address,
    uint32_t device_type, uint32_t capabilities);

/**
 * Fusain state command message (for Zbus/IPC)
 *
 * This structure is used for inter-process communication (e.g., Zbus)
 * to request mode changes via Fusain. It is NOT the protocol packet format.
 *
 * For the wire protocol representation, see fusain_cmd_set_mode_t.
 */
typedef struct {
  fusain_mode_t mode; // Target mode (IDLE, FAN, HEAT, EMERGENCY)
  uint32_t parameter; // Mode parameter (RPM for FAN, pump rate for HEAT)
} fusain_state_command_msg_t;

#endif /* FUSAIN_H_ */
