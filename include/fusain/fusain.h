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
  FUSAIN_MSG_TIMEOUT_CONFIG = 0x17,
  FUSAIN_MSG_DISCOVERY_REQUEST = 0x1F,

  /* Control Commands (Controller → Appliance) 0x20-0x2F */
  FUSAIN_MSG_STATE_COMMAND = 0x20,
  FUSAIN_MSG_MOTOR_COMMAND = 0x21,
  FUSAIN_MSG_PUMP_COMMAND = 0x22,
  FUSAIN_MSG_GLOW_COMMAND = 0x23,
  FUSAIN_MSG_TEMP_COMMAND = 0x24,
  FUSAIN_MSG_SEND_TELEMETRY = 0x25,
  FUSAIN_MSG_PING_REQUEST = 0x2F,

  /* Telemetry Data (Appliance → Controller) 0x30-0x3F */
  FUSAIN_MSG_STATE_DATA = 0x30,
  FUSAIN_MSG_MOTOR_DATA = 0x31,
  FUSAIN_MSG_PUMP_DATA = 0x32,
  FUSAIN_MSG_GLOW_DATA = 0x33,
  FUSAIN_MSG_TEMP_DATA = 0x34,
  FUSAIN_MSG_DEVICE_ANNOUNCE = 0x35,
  FUSAIN_MSG_PING_RESPONSE = 0x3F,

  /* Error Messages (Bidirectional) 0xE0-0xEF */
  FUSAIN_MSG_ERROR_INVALID_CMD = 0xE0,
  FUSAIN_MSG_ERROR_STATE_REJECT = 0xE1,
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
  FUSAIN_ERROR_OVERHEAT = 0x01,
  FUSAIN_ERROR_SENSOR_FAULT = 0x02,
  FUSAIN_ERROR_IGNITION_FAIL = 0x03,
  FUSAIN_ERROR_FLAME_OUT = 0x04,
  FUSAIN_ERROR_MOTOR_STALL = 0x05,
  FUSAIN_ERROR_PUMP_FAULT = 0x06,
  FUSAIN_ERROR_COMMANDED_ESTOP = 0x07,
} fusain_error_t;

/* Temperature command types */
typedef enum {
  FUSAIN_TEMP_CMD_WATCH_MOTOR = 0,
  FUSAIN_TEMP_CMD_UNWATCH_MOTOR = 1,
  FUSAIN_TEMP_CMD_ENABLE_RPM_CONTROL = 2,
  FUSAIN_TEMP_CMD_DISABLE_RPM_CONTROL = 3,
  FUSAIN_TEMP_CMD_SET_TARGET_TEMP = 4,
} fusain_temp_cmd_type_t;

/* Configuration Command Payloads (used by fusain_create_*_config APIs) */
typedef struct {
  uint8_t motor;
  uint32_t pwm_period;
  double pid_kp;
  double pid_ki;
  double pid_kd;
  int32_t max_rpm;
  int32_t min_rpm;
  uint32_t min_pwm_duty;
} fusain_cmd_motor_config_t;

typedef struct {
  uint8_t pump;
  uint32_t pulse_ms;
  uint32_t recovery_ms;
} fusain_cmd_pump_config_t;

typedef struct {
  uint8_t thermometer;
  double pid_kp;
  double pid_ki;
  double pid_kd;
} fusain_cmd_temp_config_t;

typedef struct {
  uint8_t glow;
  uint32_t max_duration_ms;
} fusain_cmd_glow_config_t;

/* Pump event types (used by fusain_create_pump_data API) */
typedef enum {
  FUSAIN_PUMP_EVENT_INITIALIZING = 0,
  FUSAIN_PUMP_EVENT_READY = 1,
  FUSAIN_PUMP_EVENT_ERROR = 2,
  FUSAIN_PUMP_EVENT_CYCLE_START = 3,
  FUSAIN_PUMP_EVENT_PULSE_END = 4,
  FUSAIN_PUMP_EVENT_CYCLE_END = 5,
} fusain_pump_event_t;

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
 * Create a STATE_COMMAND packet
 *
 * @param packet Output packet
 * @param address Device address
 * @param mode Operating mode
 * @param argument Mode-specific argument (RPM for FAN, pump rate for HEAT)
 */
void fusain_create_state_command(fusain_packet_t* packet, uint64_t address,
    fusain_mode_t mode, int32_t argument);

/**
 * Create a PUMP_COMMAND packet
 *
 * @param packet Output packet
 * @param address Device address
 * @param pump Pump index (0-9, typically 0)
 * @param rate_ms Pulse interval in milliseconds (0=stop, >=100=run)
 */
void fusain_create_pump_command(fusain_packet_t* packet, uint64_t address,
    uint8_t pump, int32_t rate_ms);

/**
 * Create a MOTOR_COMMAND packet
 *
 * @param packet Output packet
 * @param address Device address
 * @param motor Motor index (0-9, typically 0)
 * @param rpm Target RPM (0=stop, 800-3400=run)
 */
void fusain_create_motor_command(fusain_packet_t* packet, uint64_t address,
    uint8_t motor, int32_t rpm);

/**
 * Create a GLOW_COMMAND packet
 *
 * @param packet Output packet
 * @param address Device address
 * @param glow Glow plug index (0-9, typically 0)
 * @param duration Burn duration in milliseconds (0-300000)
 */
void fusain_create_glow_command(fusain_packet_t* packet, uint64_t address, uint8_t glow,
    int32_t duration);

/**
 * Create a TEMP_COMMAND packet
 *
 * @param packet Output packet
 * @param address Device address
 * @param thermometer Temperature controller index (0-9, typically 0)
 * @param type Command type (fusain_temp_cmd_type_t)
 * @param motor_index Motor to control (used with WATCH_MOTOR)
 * @param target_temp Target temperature in Celsius (used with SET_TARGET_TEMP)
 */
void fusain_create_temp_command(fusain_packet_t* packet, uint64_t address,
    uint8_t thermometer, fusain_temp_cmd_type_t type, uint8_t motor_index,
    float target_temp);

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
 * @param interval_ms Telemetry broadcast interval (0=polling, 100-5000 ms)
 */
void fusain_create_telemetry_config(fusain_packet_t* packet, uint64_t address, bool enabled,
    uint32_t interval_ms);

/**
 * Create a TIMEOUT_CONFIG packet
 *
 * @param packet Output packet
 * @param address Device address
 * @param enabled Timeout enabled (0=disabled, 1=enabled)
 * @param timeout_ms Timeout interval in milliseconds (5000-60000)
 */
void fusain_create_timeout_config(fusain_packet_t* packet, uint64_t address, bool enabled,
    uint32_t timeout_ms);

/**
 * Create a SEND_TELEMETRY packet (polling mode)
 *
 * @param packet Output packet
 * @param address Device address
 * @param telemetry_type Type (0=STATE, 1=MOTOR, 2=TEMP, 3=PUMP, 4=GLOW)
 * @param index Peripheral index or 0xFFFFFFFF for all
 */
void fusain_create_send_telemetry(fusain_packet_t* packet, uint64_t address,
    uint32_t telemetry_type, uint32_t index);

/**
 * Create a MOTOR_CONFIG packet
 *
 * @param packet Output packet
 * @param config Motor configuration parameters
 */
void fusain_create_motor_config(fusain_packet_t* packet, uint64_t address,
    const fusain_cmd_motor_config_t* config);

/**
 * Create a PUMP_CONFIG packet
 *
 * @param packet Output packet
 * @param config Pump configuration parameters
 */
void fusain_create_pump_config(fusain_packet_t* packet, uint64_t address,
    const fusain_cmd_pump_config_t* config);

/**
 * Create a TEMP_CONFIG packet
 *
 * @param packet Output packet
 * @param config Temperature configuration parameters
 */
void fusain_create_temp_config(fusain_packet_t* packet, uint64_t address,
    const fusain_cmd_temp_config_t* config);

/**
 * Create a GLOW_CONFIG packet
 *
 * @param packet Output packet
 * @param config Glow plug configuration parameters
 */
void fusain_create_glow_config(fusain_packet_t* packet, uint64_t address,
    const fusain_cmd_glow_config_t* config);

/**
 * Create a DATA_SUBSCRIPTION packet
 *
 * @param packet Output packet
 * @param address Device address
 * @param appliance_address Address of appliance to subscribe to
 */
void fusain_create_data_subscription(fusain_packet_t* packet, uint64_t address,
    uint64_t appliance_address);

/**
 * Create a DATA_UNSUBSCRIBE packet
 *
 * @param packet Output packet
 * @param appliance_address Address of appliance to unsubscribe from
 */
void fusain_create_data_unsubscribe(fusain_packet_t* packet, uint64_t address,
    uint64_t appliance_address);

/**
 * Create a DISCOVERY_REQUEST packet
 *
 * @param packet Output packet
 */
void fusain_create_discovery_request(fusain_packet_t* packet, uint64_t address);

/**
 * Create a STATE_DATA packet
 *
 * @param packet Output packet
 * @param address Device address
 * @param error Error flag (0=no error, 1=error)
 * @param code Error code (fusain_error_t)
 * @param state Current state
 * @param timestamp Timestamp in milliseconds since boot
 */
void fusain_create_state_data(fusain_packet_t* packet, uint64_t address,
    uint32_t error, uint8_t code, fusain_state_t state, uint32_t timestamp);

/**
 * Create a PING_RESPONSE packet
 *
 * @param packet Output packet
 * @param address Device address
 * @param uptime_ms System uptime in milliseconds (wraps at 2^32)
 */
void fusain_create_ping_response(fusain_packet_t* packet, uint64_t address, uint32_t uptime_ms);

/**
 * Create a DEVICE_ANNOUNCE packet
 *
 * @param packet Output packet
 * @param address Device address
 * @param motor_count Number of motors this device has
 * @param thermometer_count Number of temperature sensors
 * @param pump_count Number of pumps
 * @param glow_count Number of glow plugs
 */
void fusain_create_device_announce(fusain_packet_t* packet, uint64_t address,
    uint8_t motor_count, uint8_t thermometer_count, uint8_t pump_count,
    uint8_t glow_count);

/**
 * Create a MOTOR_DATA packet
 *
 * @param packet Output packet
 * @param address Device address
 * @param motor Motor index
 * @param timestamp Timestamp in milliseconds since boot
 * @param rpm Current measured RPM
 * @param target Target RPM setpoint
 */
void fusain_create_motor_data(fusain_packet_t* packet, uint64_t address,
    uint8_t motor, uint32_t timestamp, int32_t rpm, int32_t target);

/**
 * Create a PUMP_DATA packet
 *
 * @param packet Output packet
 * @param address Device address
 * @param pump Pump index
 * @param timestamp Timestamp in milliseconds since boot
 * @param type Event type (fusain_pump_event_t)
 * @param rate Current pump rate in milliseconds
 */
void fusain_create_pump_data(fusain_packet_t* packet, uint64_t address,
    uint8_t pump, uint32_t timestamp, fusain_pump_event_t type, int32_t rate);

/**
 * Create a GLOW_DATA packet
 *
 * @param packet Output packet
 * @param address Device address
 * @param glow Glow plug index
 * @param timestamp Timestamp in milliseconds since boot
 * @param lit Lit status (true = on, false = off)
 */
void fusain_create_glow_data(fusain_packet_t* packet, uint64_t address,
    uint8_t glow, uint32_t timestamp, bool lit);

/**
 * Create a TEMP_DATA packet
 *
 * @param packet Output packet
 * @param address Device address
 * @param thermometer Thermometer index
 * @param timestamp Timestamp in milliseconds since boot
 * @param reading Temperature reading in Celsius
 */
void fusain_create_temp_data(fusain_packet_t* packet, uint64_t address,
    uint8_t thermometer, uint32_t timestamp, float reading);

/**
 * Create an ERROR_INVALID_CMD packet
 *
 * @param packet Output packet
 * @param address Device address
 * @param error_code Error code (1 = invalid param, 2 = invalid device index)
 */
void fusain_create_error_invalid_cmd(fusain_packet_t* packet, uint64_t address,
    int32_t error_code);

/**
 * Create an ERROR_STATE_REJECT packet
 *
 * @param packet Output packet
 * @param address Device address
 * @param state Current state that rejected the command
 */
void fusain_create_error_state_reject(fusain_packet_t* packet, uint64_t address,
    fusain_state_t state);

/* Net Buffer API (Zephyr only) */
#ifdef CONFIG_FUSAIN_NET_BUF

/* Forward declarations - include <zephyr/net_buf.h> before using these APIs */
struct net_buf;
struct net_buf_pool;

/**
 * Decode a byte and return net_buf on packet completion
 *
 * Wrapper around fusain_decode_byte() that allocates a net_buf
 * when a complete packet is decoded.
 *
 * Note: Include <zephyr/net_buf.h> before calling this function.
 *
 * @param byte Received byte to process
 * @param pool Net buffer pool to allocate from
 * @param decoder Decoder state (must be initialized and persistent)
 * @return Pointer to net_buf containing decoded packet on completion,
 *         or NULL if packet incomplete. Caller must unref when done.
 */
struct net_buf* fusain_decode_byte_to_net_buf(uint8_t byte,
    struct net_buf_pool* pool,
    fusain_decoder_t* decoder);

/**
 * Get fusain_packet_t pointer from net_buf
 *
 * Returns a pointer to the fusain_packet_t stored in the net_buf data.
 *
 * Note: Include <zephyr/net_buf.h> before calling this function.
 *
 * @param buf Net buffer containing decoded packet
 * @return Pointer to packet structure (valid while buf is referenced)
 */
fusain_packet_t* fusain_packet_from_buf(struct net_buf* buf);

#endif /* CONFIG_FUSAIN_NET_BUF */

#endif /* FUSAIN_H_ */
