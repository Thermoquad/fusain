/*
 * Copyright (c) 2025 Kaz Walker, Thermoquad
 * SPDX-License-Identifier: Apache-2.0
 *
 * Fusain Serial Protocol - Shared Library
 *
 * This library provides reusable functions for encoding/decoding Helios serial
 * protocol packets. Can be used by both ICU (slave) and controller (master).
 */

#ifndef FUSAIN_H_
#define FUSAIN_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Protocol Constants */
#define HELIOS_START_BYTE 0x7E
#define HELIOS_END_BYTE 0x7F
#define HELIOS_ESC_BYTE 0x7D
#define HELIOS_ESC_XOR 0x20

#define HELIOS_MAX_PACKET_SIZE 128
#define HELIOS_MAX_PAYLOAD_SIZE 114
#define HELIOS_MIN_PACKET_SIZE 6 // START + LEN + TYPE + CRC(2) + END

/* Message Type Definitions */
typedef enum {
  /* Configuration Commands (Controller → Appliance) 0x10-0x1F */
  HELIOS_MSG_MOTOR_CONFIG = 0x10,
  HELIOS_MSG_PUMP_CONFIG = 0x11,
  HELIOS_MSG_TEMP_CONFIG = 0x12,
  HELIOS_MSG_GLOW_CONFIG = 0x13,
  HELIOS_MSG_DATA_SUBSCRIPTION = 0x14,
  HELIOS_MSG_DATA_UNSUBSCRIPTION = 0x15,
  HELIOS_MSG_TELEMETRY_CONFIG = 0x16,
  HELIOS_MSG_DISCOVERY_REQUEST = 0x1F,

  /* Control Commands (Controller → Appliance) 0x20-0x2F */
  HELIOS_MSG_STATE_COMMAND = 0x20,
  HELIOS_MSG_MOTOR_COMMAND = 0x21,
  HELIOS_MSG_PUMP_COMMAND = 0x22,
  HELIOS_MSG_GLOW_COMMAND = 0x23,
  HELIOS_MSG_TEMP_COMMAND = 0x24,
  HELIOS_MSG_PING_REQUEST = 0x2F,

  /* Telemetry Data (Appliance → Controller) 0x30-0x3F */
  HELIOS_MSG_STATE_DATA = 0x30,
  HELIOS_MSG_MOTOR_DATA = 0x31,
  HELIOS_MSG_PUMP_DATA = 0x32,
  HELIOS_MSG_GLOW_DATA = 0x33,
  HELIOS_MSG_TEMP_DATA = 0x34,
  HELIOS_MSG_TELEMETRY_BUNDLE = 0x35,
  HELIOS_MSG_DEVICE_ANNOUNCE = 0x36,
  HELIOS_MSG_PING_RESPONSE = 0x3F,

  /* Error Messages (Bidirectional) 0xE0-0xEF */
  HELIOS_MSG_ERROR_INVALID_MSG = 0xE0,
  HELIOS_MSG_ERROR_CRC_FAIL = 0xE1,
  HELIOS_MSG_ERROR_INVALID_CMD = 0xE2,
  HELIOS_MSG_ERROR_STATE_REJECT = 0xE3,
} helios_msg_type_t;

/* Operating Modes */
typedef enum {
  HELIOS_MODE_IDLE = 0x00,
  HELIOS_MODE_FAN = 0x01,
  HELIOS_MODE_HEAT = 0x02,
  HELIOS_MODE_EMERGENCY = 0xFF,
} helios_mode_t;

/* State Values (from state machine) */
typedef enum {
  HELIOS_STATE_INITIALIZING = 0x00,
  HELIOS_STATE_IDLE = 0x01,
  HELIOS_STATE_BLOWING = 0x02,
  HELIOS_STATE_PREHEAT = 0x03,
  HELIOS_STATE_PREHEAT_STAGE_2 = 0x04,
  HELIOS_STATE_HEATING = 0x05,
  HELIOS_STATE_COOLING = 0x06,
  HELIOS_STATE_ERROR = 0x07,
  HELIOS_STATE_E_STOP = 0x08,
} helios_state_t;

/* Error Codes */
typedef enum {
  HELIOS_ERROR_NONE = 0x00,
  HELIOS_ERROR_PREHEAT_FAILED = 0x01,
  HELIOS_ERROR_FLAME_OUT = 0x02,
  HELIOS_ERROR_OVERHEAT = 0x03,
  HELIOS_ERROR_PUMP_FAULT = 0x04,
  HELIOS_ERROR_TIMEOUT = 0x05,
} helios_error_t;

/* Command Payloads */
typedef struct __attribute__((packed)) {
  uint8_t mode; // helios_mode_t
  uint32_t parameter; // RPM for FAN mode, 0 for others
} helios_cmd_set_mode_t;

typedef struct __attribute__((packed)) {
  uint32_t rate_ms; // Pump interval in milliseconds
} helios_cmd_set_pump_rate_t;

typedef struct __attribute__((packed)) {
  uint32_t target_rpm;
} helios_cmd_set_target_rpm_t;

typedef struct __attribute__((packed)) {
  int32_t glow; // Glow plug index (0-9, typically 0)
  int32_t duration; // Burn duration in milliseconds (0-300000)
} helios_cmd_glow_t;

typedef struct __attribute__((packed)) {
  uint32_t telemetry_enabled; // 0 = disabled, 1 = enabled
  uint32_t interval_ms; // Telemetry broadcast interval (100-5000 ms)
  uint32_t telemetry_mode; // 0 = bundled, 1 = individual
} helios_cmd_telemetry_config_t;

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
} helios_cmd_motor_config_t;

typedef struct __attribute__((packed)) {
  int32_t pump;
  uint32_t min_rate_ms;
  uint32_t max_rate_ms;
  uint8_t padding[4];
} helios_cmd_pump_config_t;

typedef struct __attribute__((packed)) {
  int32_t thermometer;
  double pid_kp;
  double pid_ki;
  double pid_kd;
  uint32_t sample_count;
  uint32_t read_rate;
  uint8_t padding[12];
} helios_cmd_temp_config_t;

typedef struct __attribute__((packed)) {
  int32_t glow;
  uint32_t max_duration_ms;
  uint8_t padding[8];
} helios_cmd_glow_config_t;

typedef struct __attribute__((packed)) {
  uint64_t appliance_address;
  uint64_t message_filter;
} helios_cmd_data_subscription_t;

typedef struct __attribute__((packed)) {
  uint64_t appliance_address;
} helios_cmd_data_unsubscription_t;

/* Data Payloads */
typedef struct __attribute__((packed)) {
  uint32_t state; // helios_state_t
  uint8_t error; // helios_error_t
} helios_data_state_t;

typedef struct __attribute__((packed)) {
  int32_t rpm;
  int32_t target_rpm;
  int32_t pwm_duty;
  int32_t pwm_period;
  int32_t min_rpm;
  int32_t max_rpm;
} helios_data_motor_t;

typedef struct __attribute__((packed)) {
  double temperature;
  uint8_t pid_enabled;
  double pid_setpoint;
  int32_t pid_output_rpm;
} helios_data_temperature_t;

typedef struct __attribute__((packed)) {
  uint8_t enabled;
  uint32_t rate_ms;
  uint64_t pulse_count;
} helios_data_pump_t;

typedef struct __attribute__((packed)) {
  uint8_t lit;
  uint64_t lit_timestamp;
  uint64_t total_burn_time;
} helios_data_glow_t;

typedef struct __attribute__((packed)) {
  uint64_t uptime_ms;
} helios_data_ping_response_t;

typedef struct __attribute__((packed)) {
  uint32_t device_type;
  uint32_t capabilities;
} helios_data_device_announce_t;

/* Variable-length telemetry bundle */
#define HELIOS_MAX_MOTORS 5
#define HELIOS_MAX_TEMPERATURES 4

typedef struct __attribute__((packed)) {
  int32_t rpm;
  int32_t target_rpm;
  int32_t pwm_duty;
  int32_t pwm_period;
} helios_telemetry_motor_t;

typedef struct __attribute__((packed)) {
  double temperature;
} helios_telemetry_temperature_t;

typedef struct __attribute__((packed)) {
  uint32_t state; // helios_state_t
  uint8_t error; // helios_error_t
  uint8_t motor_count; // Number of motors (1-3)
  uint8_t temp_count; // Number of temperature sensors (1-3)
  // Followed by:
  // - motor_count × helios_telemetry_motor_t
  // - temp_count × helios_telemetry_temperature_t
} helios_data_telemetry_bundle_t;

/* Error Message Payloads */
typedef struct __attribute__((packed)) {
  uint8_t invalid_command;
} helios_error_invalid_command_t;

typedef struct __attribute__((packed)) {
  uint16_t received_crc;
  uint16_t calculated_crc;
} helios_error_invalid_crc_t;

typedef struct __attribute__((packed)) {
  uint8_t received_length;
  uint8_t expected_length;
} helios_error_invalid_length_t;

/* Packet Structure */
typedef struct {
  uint8_t start; // HELIOS_START_BYTE
  uint8_t length; // Payload length (0-58)
  uint8_t msg_type; // helios_msg_type_t
  uint8_t payload[HELIOS_MAX_PAYLOAD_SIZE];
  uint16_t crc; // CRC-16-CCITT
  uint8_t end; // HELIOS_END_BYTE
} helios_packet_t;

/* Decode Result */
typedef enum {
  HELIOS_DECODE_OK = 0,
  HELIOS_DECODE_INCOMPLETE = 1,
  HELIOS_DECODE_INVALID_START = 2,
  HELIOS_DECODE_INVALID_CRC = 3,
  HELIOS_DECODE_INVALID_LENGTH = 4,
  HELIOS_DECODE_BUFFER_OVERFLOW = 5,
} helios_decode_result_t;

/* Decoder State */
typedef struct {
  uint8_t state; // Internal state machine state
  uint8_t buffer[HELIOS_MAX_PACKET_SIZE]; // Internal decode buffer
  size_t buffer_index; // Current position in buffer
  bool escape_next; // Escape sequence flag
} helios_decoder_t;

/* Function Declarations */

/**
 * Calculate CRC-16-CCITT (polynomial 0x1021, initial 0xFFFF)
 *
 * @param data Pointer to data buffer
 * @param length Length of data in bytes
 * @return CRC-16 value
 */
uint16_t helios_crc16(const uint8_t* data, size_t length);

/**
 * Encode a packet into a byte buffer with byte stuffing
 *
 * @param packet Packet structure to encode
 * @param buffer Output buffer for encoded bytes
 * @param buffer_size Size of output buffer
 * @return Number of bytes written, or negative error code
 */
int helios_encode_packet(const helios_packet_t* packet, uint8_t* buffer,
    size_t buffer_size);

/**
 * Decode received bytes into a packet structure
 *
 * This function handles byte unstuffing and CRC validation.
 * Call repeatedly as bytes are received until HELIOS_DECODE_OK or error.
 *
 * @param rx_byte Received byte to process
 * @param packet Output packet structure
 * @param decoder Decoder state (initialize with helios_reset_decoder)
 * @return Decode result status
 */
helios_decode_result_t helios_decode_byte(uint8_t rx_byte,
    helios_packet_t* packet,
    helios_decoder_t* decoder);

/**
 * Reset decoder state
 *
 * @param decoder Decoder state to reset
 */
void helios_reset_decoder(helios_decoder_t* decoder);

/**
 * Create a SET_MODE command packet
 *
 * @param packet Output packet
 * @param mode Operating mode
 * @param parameter Mode parameter (RPM for FAN mode)
 */
void helios_create_set_mode(helios_packet_t* packet, helios_mode_t mode,
    uint32_t parameter);

/**
 * Create a SET_PUMP_RATE command packet
 *
 * @param packet Output packet
 * @param rate_ms Pump rate in milliseconds
 */
void helios_create_set_pump_rate(helios_packet_t* packet, uint32_t rate_ms);

/**
 * Create a SET_TARGET_RPM command packet
 *
 * @param packet Output packet
 * @param target_rpm Target RPM
 */
void helios_create_set_target_rpm(helios_packet_t* packet, uint32_t target_rpm);

/**
 * Create a GLOW_COMMAND packet
 *
 * @param packet Output packet
 * @param glow Glow plug index (0-9, typically 0)
 * @param duration Burn duration in milliseconds (0-300000)
 */
void helios_create_glow_command(helios_packet_t* packet, int32_t glow,
    int32_t duration);

/**
 * Create a PING_REQUEST packet
 *
 * @param packet Output packet
 */
void helios_create_ping_request(helios_packet_t* packet);

/**
 * Create a TELEMETRY_CONFIG packet
 *
 * @param packet Output packet
 * @param enabled Telemetry broadcast enabled (0=disabled, 1=enabled)
 * @param interval_ms Telemetry broadcast interval (100-5000 ms)
 * @param mode Telemetry mode (0=bundled, 1=individual)
 */
void helios_create_telemetry_config(helios_packet_t* packet, bool enabled,
    uint32_t interval_ms, uint32_t mode);

/**
 * Create a MOTOR_CONFIG packet (v2.0)
 *
 * @param packet Output packet
 * @param config Motor configuration parameters
 */
void helios_create_motor_config(helios_packet_t* packet,
    const helios_cmd_motor_config_t* config);

/**
 * Create a PUMP_CONFIG packet (v2.0)
 *
 * @param packet Output packet
 * @param config Pump configuration parameters
 */
void helios_create_pump_config(helios_packet_t* packet,
    const helios_cmd_pump_config_t* config);

/**
 * Create a TEMP_CONFIG packet (v2.0)
 *
 * @param packet Output packet
 * @param config Temperature configuration parameters
 */
void helios_create_temp_config(helios_packet_t* packet,
    const helios_cmd_temp_config_t* config);

/**
 * Create a GLOW_CONFIG packet (v2.0)
 *
 * @param packet Output packet
 * @param config Glow plug configuration parameters
 */
void helios_create_glow_config(helios_packet_t* packet,
    const helios_cmd_glow_config_t* config);

/**
 * Create a DATA_SUBSCRIPTION packet (v2.0)
 *
 * @param packet Output packet
 * @param appliance_address Address of appliance to subscribe to
 * @param message_filter Message type filter bitmask
 */
void helios_create_data_subscription(helios_packet_t* packet,
    uint64_t appliance_address, uint64_t message_filter);

/**
 * Create a DATA_UNSUBSCRIPTION packet (v2.0)
 *
 * @param packet Output packet
 * @param appliance_address Address of appliance to unsubscribe from
 */
void helios_create_data_unsubscription(helios_packet_t* packet,
    uint64_t appliance_address);

/**
 * Create a DISCOVERY_REQUEST packet (v2.0)
 *
 * @param packet Output packet
 */
void helios_create_discovery_request(helios_packet_t* packet);

/**
 * Create a STATE_DATA packet
 *
 * @param packet Output packet
 * @param state Current state
 * @param error Error code
 */
void helios_create_state_data(helios_packet_t* packet, helios_state_t state,
    helios_error_t error);

/**
 * Create a PING_RESPONSE packet
 *
 * @param packet Output packet
 * @param uptime_ms System uptime in milliseconds
 */
void helios_create_ping_response(helios_packet_t* packet, uint64_t uptime_ms);

/**
 * Create a TELEMETRY_BUNDLE packet
 *
 * @param packet Output packet
 * @param state Current state
 * @param error Error code
 * @param motors Array of motor data
 * @param motor_count Number of motors (1-5)
 * @param temperatures Array of temperature data
 * @param temp_count Number of temperatures (1-4)
 * @return 0 on success, negative on error
 */
int helios_create_telemetry_bundle(helios_packet_t* packet,
    helios_state_t state, helios_error_t error,
    const helios_telemetry_motor_t* motors,
    uint8_t motor_count,
    const helios_telemetry_temperature_t* temperatures,
    uint8_t temp_count);

/**
 * Create a DEVICE_ANNOUNCE packet (v2.0)
 *
 * @param packet Output packet
 * @param device_type Device type identifier
 * @param capabilities Capabilities bitmask
 */
void helios_create_device_announce(helios_packet_t* packet,
    uint32_t device_type, uint32_t capabilities);

/**
 * Helios state command message (for Zbus/IPC)
 *
 * This structure is used for inter-process communication (e.g., Zbus)
 * to request mode changes on Helios. It is NOT the protocol packet format.
 *
 * For the wire protocol representation, see helios_cmd_set_mode_t.
 */
typedef struct {
  helios_mode_t mode; // Target mode (IDLE, FAN, HEAT, EMERGENCY)
  uint32_t parameter; // Mode parameter (RPM for FAN, pump rate for HEAT)
} helios_state_command_msg_t;

/**
 * Human-readable names for helios_mode_t values
 *
 * Index corresponds to helios_mode_t enum values:
 * - [0] = HELIOS_MODE_IDLE
 * - [1] = HELIOS_MODE_FAN
 * - [2] = HELIOS_MODE_HEAT
 * - [0xFF] = HELIOS_MODE_EMERGENCY (not in array, use special handling)
 */
extern const char* const helios_mode_names[3];

/**
 * Human-readable names for helios_state_t values
 *
 * Index corresponds to helios_state_t enum values:
 * - [0] = HELIOS_STATE_INITIALIZING
 * - [1] = HELIOS_STATE_IDLE
 * - [2] = HELIOS_STATE_BLOWING
 * - [3] = HELIOS_STATE_PREHEAT
 * - [4] = HELIOS_STATE_PREHEAT_STAGE_2
 * - [5] = HELIOS_STATE_HEATING
 * - [6] = HELIOS_STATE_COOLING
 * - [7] = HELIOS_STATE_ERROR
 * - [8] = HELIOS_STATE_E_STOP
 */
extern const char* const helios_state_names[9];

#endif /* FUSAIN_H_ */
