/*
 * Copyright (c) 2025 Kaz Walker, Thermoquad
 * SPDX-License-Identifier: Apache-2.0
 *
 * Fusain Serial Protocol - Shared Library Implementation
 *
 * Wire format: [START][LENGTH][ADDRESS(8)][CBOR_PAYLOAD][CRC(2)][END]
 * CBOR payload: [msg_type, payload_map] or [msg_type, nil] for empty payloads
 */
#include <stdint.h>
#include <string.h>

#include <fusain/fusain.h>
#include <fusain/generated/cbor_decode.h>
#include <fusain/generated/cbor_encode.h>
#include <fusain/generated/cbor_types.h>

#ifdef CONFIG_FUSAIN
#include <zephyr/sys/crc.h>
#endif

/* CBOR encoding constants */
#define CBOR_ARRAY_2 0x82 /* 2-element array header */
#define CBOR_UINT8_PREFIX 0x18 /* uint8 value follows */
#define CBOR_NIL 0xF6 /* nil/null value */

/* Decoder States
 * Note: TYPE state removed - msg_type is now embedded in CBOR payload
 */
#define DECODER_STATE_IDLE 0
#define DECODER_STATE_LENGTH 1
#define DECODER_STATE_ADDRESS 2
#define DECODER_STATE_PAYLOAD 3 /* CBOR payload (includes msg_type) */
#define DECODER_STATE_CRC1 4
#define DECODER_STATE_CRC2 5
#define DECODER_STATE_END 6

/* CRC-16-CCITT Calculation */
#ifdef CONFIG_FUSAIN
/* Use Zephyr's native CRC implementation when built as Zephyr module */
uint16_t fusain_crc16(const uint8_t* data, size_t length)
{
  return crc16_itu_t(0xFFFF, data, length);
}
#else
/* Built-in CRC implementation for standalone builds */
uint16_t fusain_crc16(const uint8_t* data, size_t length)
{
  uint16_t crc = 0xFFFF; /* Initial value */
  const uint16_t polynomial = 0x1021;

  for (size_t i = 0; i < length; i++) {
    crc ^= (uint16_t)data[i] << 8;
    for (uint8_t bit = 0; bit < 8; bit++) {
      if (crc & 0x8000) {
        crc = (crc << 1) ^ polynomial;
      } else {
        crc = crc << 1;
      }
    }
  }

  return crc;
}
#endif

/* CBOR Message Wrapper Encoding
 *
 * Encodes the CBOR array header and msg_type prefix: [type, ...]
 * Returns number of bytes written (3 for types 0x18-0xFF, 2 for types 0x00-0x17)
 */
static int encode_cbor_message_header(uint8_t* buffer, size_t buffer_size,
    uint8_t msg_type)
{
  if (msg_type <= 0x17) {
    /* Small type value: 2 bytes [0x82, type] */
    if (buffer_size < 2) /* LCOV_EXCL_LINE - always called with 114-byte buffer */
      return -1; /* LCOV_EXCL_LINE */
    buffer[0] = CBOR_ARRAY_2;
    buffer[1] = msg_type;
    return 2;
  } else {
    /* Larger type value: 3 bytes [0x82, 0x18, type] */
    if (buffer_size < 3) /* LCOV_EXCL_LINE - always called with 114-byte buffer */
      return -1; /* LCOV_EXCL_LINE */
    buffer[0] = CBOR_ARRAY_2;
    buffer[1] = CBOR_UINT8_PREFIX;
    buffer[2] = msg_type;
    return 3;
  }
}

/* Encode nil payload for messages with no data (PING_REQUEST, DISCOVERY_REQUEST)
 * Returns number of bytes written (1)
 */
static int encode_cbor_nil(uint8_t* buffer, size_t buffer_size)
{
  if (buffer_size < 1) /* LCOV_EXCL_LINE - always called with 114-byte buffer */
    return -1; /* LCOV_EXCL_LINE */
  buffer[0] = CBOR_NIL;
  return 1;
}

/* Extract msg_type from CBOR payload [type, payload_map]
 * Returns 0 on success, -1 on error
 * Sets *msg_type and *header_len (bytes consumed for array header + type)
 */
static int decode_cbor_message_header(const uint8_t* buffer, size_t buffer_size,
    uint8_t* msg_type, size_t* header_len)
{
  if (buffer_size < 2)
    return -1;

  /* Check for 2-element array header */
  if (buffer[0] != CBOR_ARRAY_2)
    return -1;

  /* Extract msg_type */
  if (buffer[1] <= 0x17) {
    /* Immediate value (0x00-0x17) */
    *msg_type = buffer[1];
    *header_len = 2;
    return 0;
  } else if (buffer[1] == CBOR_UINT8_PREFIX) {
    /* uint8 follows */
    if (buffer_size < 3)
      return -1;
    *msg_type = buffer[2];
    *header_len = 3;
    return 0;
  }

  return -1; /* Unsupported encoding */
}

/* Byte Stuffing Helper */
static int stuff_byte(uint8_t byte, uint8_t* buffer, size_t buffer_size,
    size_t* index)
{
  if (byte == FUSAIN_START_BYTE || byte == FUSAIN_END_BYTE || byte == FUSAIN_ESC_BYTE) {
    // Need to escape this byte
    if (*index + 2 > buffer_size) {
      return -1; // Buffer overflow
    }
    buffer[(*index)++] = FUSAIN_ESC_BYTE;
    buffer[(*index)++] = byte ^ FUSAIN_ESC_XOR;
  } else {
    // Normal byte
    if (*index + 1 > buffer_size) {
      return -1; // Buffer overflow
    }
    buffer[(*index)++] = byte;
  }
  return 0;
}

/* Packet Encoding
 *
 * Wire format: [START][LENGTH][ADDRESS(8)][CBOR_PAYLOAD][CRC(2)][END]
 *
 * The packet->payload field contains CBOR-encoded [type, payload_map] bytes.
 * The packet->length field contains the CBOR byte count.
 * The packet->msg_type field is kept for routing but is also in the CBOR payload.
 */
int fusain_encode_packet(const fusain_packet_t* packet, uint8_t* buffer,
    size_t buffer_size)
{
  if (!packet || !buffer || buffer_size < FUSAIN_MIN_PACKET_SIZE) {
    return -1;
  }

  if (packet->length > FUSAIN_MAX_PAYLOAD_SIZE) {
    return -2; // Invalid payload length
  }

  size_t index = 0;

  // START byte (never escaped)
  buffer[index++] = FUSAIN_START_BYTE;

  // Calculate CRC over LENGTH + ADDRESS + CBOR_PAYLOAD
  uint8_t crc_data[FUSAIN_MAX_PAYLOAD_SIZE + 9]; // +1 length +8 address
  crc_data[0] = packet->length;
  // Add address bytes (little-endian)
  for (int i = 0; i < 8; i++) {
    crc_data[1 + i] = (packet->address >> (i * 8)) & 0xFF;
  }
  // CBOR payload (includes msg_type in the encoded data)
  memcpy(&crc_data[9], packet->payload, packet->length);
  uint16_t crc = fusain_crc16(crc_data, packet->length + 9);

  // Write LENGTH byte directly (never needs escaping: 0-114 range, escape bytes are 125-127)
  buffer[index++] = packet->length;

  // Stuff ADDRESS bytes (little-endian)
  for (int i = 0; i < 8; i++) {
    uint8_t addr_byte = (packet->address >> (i * 8)) & 0xFF;
    if (stuff_byte(addr_byte, buffer, buffer_size, &index) < 0) {
      return -4;
    }
  }

  // Stuff CBOR PAYLOAD bytes (msg_type is embedded in CBOR, not separate)
  for (uint8_t i = 0; i < packet->length; i++) {
    if (stuff_byte(packet->payload[i], buffer, buffer_size, &index) < 0) {
      return -6;
    }
  }

  // Stuff CRC bytes (big-endian)
  uint8_t crc_high = (crc >> 8) & 0xFF;
  uint8_t crc_low = crc & 0xFF;
  if (stuff_byte(crc_high, buffer, buffer_size, &index) < 0) { /* LCOV_EXCL_LINE */
    return -7; /* LCOV_EXCL_LINE */
  }
  if (stuff_byte(crc_low, buffer, buffer_size, &index) < 0) {
    return -8;
  }

  // END byte (never escaped)
  if (index >= buffer_size) {
    return -9;
  }
  buffer[index++] = FUSAIN_END_BYTE;

  return (int)index;
}

/* Packet Decoding
 *
 * Wire format: [START][LENGTH][ADDRESS(8)][CBOR_PAYLOAD][CRC(2)][END]
 * CBOR payload is [msg_type, payload_map] - msg_type extracted after CRC validation
 */
fusain_decode_result_t fusain_decode_byte(uint8_t rx_byte,
    fusain_packet_t* packet,
    fusain_decoder_t* decoder)
{
  /* Handle START byte - reset decoder */
  if (rx_byte == FUSAIN_START_BYTE && !(decoder->escape_next)) {
    decoder->state = DECODER_STATE_LENGTH;
    decoder->buffer_index = 0;
    decoder->escape_next = false;
    decoder->addr_byte_count = 0;
    packet->address = 0;
    return FUSAIN_DECODE_INCOMPLETE;
  }

  /* Handle escape sequences */
  if (rx_byte == FUSAIN_ESC_BYTE && !(decoder->escape_next)) {
    decoder->escape_next = true;
    return FUSAIN_DECODE_INCOMPLETE;
  }

  /* Unescape byte if needed */
  uint8_t byte = rx_byte;
  if (decoder->escape_next) {
    byte ^= FUSAIN_ESC_XOR;
    decoder->escape_next = false;
  }

  /* State machine */
  switch (decoder->state) {
  case DECODER_STATE_IDLE:
    return FUSAIN_DECODE_INCOMPLETE;

  case DECODER_STATE_LENGTH:
    if (byte > FUSAIN_MAX_PAYLOAD_SIZE) {
      decoder->state = DECODER_STATE_IDLE;
      return FUSAIN_DECODE_INVALID_LENGTH;
    }
    packet->length = byte;
    decoder->buffer[decoder->buffer_index++] = byte;
    decoder->addr_byte_count = 0;
    decoder->state = DECODER_STATE_ADDRESS;
    return FUSAIN_DECODE_INCOMPLETE;

  case DECODER_STATE_ADDRESS:
    /* Accumulate address bytes (little-endian) */
    packet->address |= ((uint64_t)byte) << (decoder->addr_byte_count * 8);
    decoder->buffer[decoder->buffer_index++] = byte;
    decoder->addr_byte_count++;
    if (decoder->addr_byte_count >= 8) {
      /* All 8 address bytes received, move to CBOR payload */
      if (packet->length == 0) {
        decoder->state = DECODER_STATE_CRC1;
      } else {
        decoder->state = DECODER_STATE_PAYLOAD;
      }
    }
    return FUSAIN_DECODE_INCOMPLETE;

  case DECODER_STATE_PAYLOAD:
    /* Store CBOR payload bytes directly */
    packet->payload[decoder->buffer_index - 9] = byte; /* -1 length -8 addr */
    decoder->buffer[decoder->buffer_index++] = byte;
    if (decoder->buffer_index >= (size_t)(packet->length + 9)) {
      decoder->state = DECODER_STATE_CRC1;
    }
    return FUSAIN_DECODE_INCOMPLETE;

  case DECODER_STATE_CRC1:
    packet->crc = (uint16_t)byte << 8;
    decoder->state = DECODER_STATE_CRC2;
    return FUSAIN_DECODE_INCOMPLETE;

  case DECODER_STATE_CRC2:
    packet->crc |= byte;
    decoder->state = DECODER_STATE_END;
    return FUSAIN_DECODE_INCOMPLETE;

  case DECODER_STATE_END: {
    if (rx_byte != FUSAIN_END_BYTE) {
      decoder->state = DECODER_STATE_IDLE;
      return FUSAIN_DECODE_INVALID_START;
    }

    /* Validate CRC (LENGTH + ADDRESS + CBOR_PAYLOAD) */
    uint16_t calculated_crc = fusain_crc16(decoder->buffer, packet->length + 9);
    if (calculated_crc != packet->crc) {
      decoder->state = DECODER_STATE_IDLE;
      return FUSAIN_DECODE_INVALID_CRC;
    }

    /* Extract msg_type from CBOR payload [type, payload_map] */
    size_t header_len = 0;
    if (decode_cbor_message_header(packet->payload, packet->length,
            &packet->msg_type, &header_len)
        != 0) {
      decoder->state = DECODER_STATE_IDLE;
      return FUSAIN_DECODE_INVALID_START; /* Invalid CBOR format */
    }

    /* Packet complete and valid */
    decoder->state = DECODER_STATE_IDLE;
    return FUSAIN_DECODE_OK;
  }

  default:
    decoder->state = DECODER_STATE_IDLE;
    return FUSAIN_DECODE_INVALID_START;
  }
}

/* Reset Decoder */
void fusain_reset_decoder(fusain_decoder_t* decoder)
{
  decoder->state = DECODER_STATE_IDLE;
  decoder->buffer_index = 0;
  decoder->escape_next = false;
  decoder->addr_byte_count = 0;
}

/* Helper Functions to Create Packets */

void fusain_create_state_command(fusain_packet_t* packet, uint64_t address,
    fusain_mode_t mode, int32_t argument)
{
  packet->address = address;
  packet->msg_type = FUSAIN_MSG_STATE_COMMAND;

  size_t offset = 0;
  int header_len = encode_cbor_message_header(packet->payload,
      FUSAIN_MAX_PAYLOAD_SIZE, FUSAIN_MSG_STATE_COMMAND);
  if (header_len < 0) { /* LCOV_EXCL_START - unreachable with 114-byte buffer */
    packet->length = 0;
    return;
  } /* LCOV_EXCL_STOP */
  offset = (size_t)header_len;

  struct state_command_payload cbor_payload = {
    .state_command_payload_mode_m = (uint32_t)mode,
    .state_command_payload_uint1int = { .state_command_payload_uint1int = argument },
    .state_command_payload_uint1int_present = true,
  };
  size_t payload_len = 0;
  int ret = cbor_encode_state_command_payload(
      packet->payload + offset, FUSAIN_MAX_PAYLOAD_SIZE - offset,
      &cbor_payload, &payload_len);
  if (ret != 0) { /* LCOV_EXCL_START - zcbor always succeeds with 114-byte buffer */
    packet->length = 0;
    return;
  } /* LCOV_EXCL_STOP */

  packet->length = (uint8_t)(offset + payload_len);
}

void fusain_create_pump_command(fusain_packet_t* packet, uint64_t address,
    uint8_t pump, int32_t rate_ms)
{
  packet->address = address;
  packet->msg_type = FUSAIN_MSG_PUMP_COMMAND;

  size_t offset = 0;
  int header_len = encode_cbor_message_header(packet->payload,
      FUSAIN_MAX_PAYLOAD_SIZE, FUSAIN_MSG_PUMP_COMMAND);
  if (header_len < 0) { /* LCOV_EXCL_START - unreachable with 114-byte buffer */
    packet->length = 0;
    return;
  } /* LCOV_EXCL_STOP */
  offset = (size_t)header_len;

  struct pump_command_payload cbor_payload = {
    .pump_command_payload_pump_index_m = (uint32_t)pump,
    .pump_command_payload_uint1int = rate_ms,
  };
  size_t payload_len = 0;
  int ret = cbor_encode_pump_command_payload(
      packet->payload + offset, FUSAIN_MAX_PAYLOAD_SIZE - offset,
      &cbor_payload, &payload_len);
  if (ret != 0) { /* LCOV_EXCL_START - zcbor always succeeds with 114-byte buffer */
    packet->length = 0;
    return;
  } /* LCOV_EXCL_STOP */

  packet->length = (uint8_t)(offset + payload_len);
}

void fusain_create_motor_command(fusain_packet_t* packet, uint64_t address,
    uint8_t motor, int32_t rpm)
{
  packet->address = address;
  packet->msg_type = FUSAIN_MSG_MOTOR_COMMAND;

  /* Encode CBOR: [type, payload_map] */
  size_t offset = 0;

  /* Encode array header and msg_type */
  int header_len = encode_cbor_message_header(packet->payload,
      FUSAIN_MAX_PAYLOAD_SIZE, FUSAIN_MSG_MOTOR_COMMAND);
  if (header_len < 0) { /* LCOV_EXCL_START - unreachable with 114-byte buffer */
    packet->length = 0;
    return;
  } /* LCOV_EXCL_STOP */
  offset = (size_t)header_len;

  /* Encode payload map using zcbor-generated encoder */
  struct motor_command_payload cbor_payload = {
    .motor_command_payload_motor_index_m = (uint32_t)motor,
    .motor_command_payload_uint1int = rpm,
  };
  size_t payload_len = 0;
  int ret = cbor_encode_motor_command_payload(
      packet->payload + offset,
      FUSAIN_MAX_PAYLOAD_SIZE - offset,
      &cbor_payload,
      &payload_len);
  if (ret != 0) { /* LCOV_EXCL_START - zcbor always succeeds with 114-byte buffer */
    packet->length = 0;
    return;
  } /* LCOV_EXCL_STOP */

  packet->length = (uint8_t)(offset + payload_len);
}

void fusain_create_glow_command(fusain_packet_t* packet, uint64_t address, uint8_t glow,
    int32_t duration)
{
  packet->address = address;
  packet->msg_type = FUSAIN_MSG_GLOW_COMMAND;

  size_t offset = 0;
  int header_len = encode_cbor_message_header(packet->payload,
      FUSAIN_MAX_PAYLOAD_SIZE, FUSAIN_MSG_GLOW_COMMAND);
  if (header_len < 0) { /* LCOV_EXCL_START - unreachable with 114-byte buffer */
    packet->length = 0;
    return;
  } /* LCOV_EXCL_STOP */
  offset = (size_t)header_len;

  struct glow_command_payload cbor_payload = {
    .glow_command_payload_glow_index_m = (uint32_t)glow,
    .glow_command_payload_uint1int = duration,
  };
  size_t payload_len = 0;
  int ret = cbor_encode_glow_command_payload(
      packet->payload + offset, FUSAIN_MAX_PAYLOAD_SIZE - offset,
      &cbor_payload, &payload_len);
  if (ret != 0) { /* LCOV_EXCL_START - zcbor always succeeds with 114-byte buffer */
    packet->length = 0;
    return;
  } /* LCOV_EXCL_STOP */

  packet->length = (uint8_t)(offset + payload_len);
}

void fusain_create_temp_command(fusain_packet_t* packet, uint64_t address,
    uint8_t thermometer, fusain_temp_cmd_type_t type, uint8_t motor_index,
    float target_temp)
{
  packet->address = address;
  packet->msg_type = FUSAIN_MSG_TEMP_COMMAND;

  size_t offset = 0;
  int header_len = encode_cbor_message_header(packet->payload,
      FUSAIN_MAX_PAYLOAD_SIZE, FUSAIN_MSG_TEMP_COMMAND);
  if (header_len < 0) { /* LCOV_EXCL_START - unreachable with 114-byte buffer */
    packet->length = 0;
    return;
  } /* LCOV_EXCL_STOP */
  offset = (size_t)header_len;

  struct temp_command_payload cbor_payload = {
    .temp_command_payload_thermometer_index_m = (uint32_t)thermometer,
    .temp_command_payload_temp_cmd_type_m = (uint32_t)type,
    .temp_command_payload_motor_index_m = { .temp_command_payload_motor_index_m = (uint32_t)motor_index },
    .temp_command_payload_motor_index_m_present = (type == FUSAIN_TEMP_CMD_WATCH_MOTOR),
    .temp_command_payload_uint3float = { .temp_command_payload_uint3float = target_temp },
    .temp_command_payload_uint3float_present = (type == FUSAIN_TEMP_CMD_SET_TARGET_TEMP),
  };
  size_t payload_len = 0;
  int ret = cbor_encode_temp_command_payload(
      packet->payload + offset, FUSAIN_MAX_PAYLOAD_SIZE - offset,
      &cbor_payload, &payload_len);
  if (ret != 0) { /* LCOV_EXCL_START - zcbor always succeeds with 114-byte buffer */
    packet->length = 0;
    return;
  } /* LCOV_EXCL_STOP */

  packet->length = (uint8_t)(offset + payload_len);
}

void fusain_create_ping_request(fusain_packet_t* packet, uint64_t address)
{
  packet->address = address;
  packet->msg_type = FUSAIN_MSG_PING_REQUEST;

  size_t offset = 0;
  int header_len = encode_cbor_message_header(packet->payload,
      FUSAIN_MAX_PAYLOAD_SIZE, FUSAIN_MSG_PING_REQUEST);
  if (header_len < 0) { /* LCOV_EXCL_START - unreachable with 114-byte buffer */
    packet->length = 0;
    return;
  } /* LCOV_EXCL_STOP */
  offset = (size_t)header_len;

  /* Encode nil for empty payload */
  int nil_len = encode_cbor_nil(packet->payload + offset, FUSAIN_MAX_PAYLOAD_SIZE - offset);
  if (nil_len < 0) { /* LCOV_EXCL_START - unreachable with 114-byte buffer */
    packet->length = 0;
    return;
  } /* LCOV_EXCL_STOP */

  packet->length = (uint8_t)(offset + (size_t)nil_len);
}

void fusain_create_telemetry_config(fusain_packet_t* packet, uint64_t address, bool enabled,
    uint32_t interval_ms)
{
  packet->address = address;
  packet->msg_type = FUSAIN_MSG_TELEMETRY_CONFIG;

  size_t offset = 0;
  int header_len = encode_cbor_message_header(packet->payload,
      FUSAIN_MAX_PAYLOAD_SIZE, FUSAIN_MSG_TELEMETRY_CONFIG);
  if (header_len < 0) { /* LCOV_EXCL_START - unreachable with 114-byte buffer */
    packet->length = 0;
    return;
  } /* LCOV_EXCL_STOP */
  offset = (size_t)header_len;

  struct telemetry_config_payload cbor_payload = {
    .telemetry_config_payload_uint0bool = enabled,
    .telemetry_config_payload_uint1uint = interval_ms,
  };
  size_t payload_len = 0;
  int ret = cbor_encode_telemetry_config_payload(
      packet->payload + offset, FUSAIN_MAX_PAYLOAD_SIZE - offset,
      &cbor_payload, &payload_len);
  if (ret != 0) { /* LCOV_EXCL_START - zcbor always succeeds with 114-byte buffer */
    packet->length = 0;
    return;
  } /* LCOV_EXCL_STOP */

  packet->length = (uint8_t)(offset + payload_len);
}

void fusain_create_timeout_config(fusain_packet_t* packet, uint64_t address, bool enabled,
    uint32_t timeout_ms)
{
  packet->address = address;
  packet->msg_type = FUSAIN_MSG_TIMEOUT_CONFIG;

  size_t offset = 0;
  int header_len = encode_cbor_message_header(packet->payload,
      FUSAIN_MAX_PAYLOAD_SIZE, FUSAIN_MSG_TIMEOUT_CONFIG);
  if (header_len < 0) { /* LCOV_EXCL_START - unreachable with 114-byte buffer */
    packet->length = 0;
    return;
  } /* LCOV_EXCL_STOP */
  offset = (size_t)header_len;

  struct timeout_config_payload cbor_payload = {
    .timeout_config_payload_uint0bool = enabled,
    .timeout_config_payload_uint1uint = timeout_ms,
  };
  size_t payload_len = 0;
  int ret = cbor_encode_timeout_config_payload(
      packet->payload + offset, FUSAIN_MAX_PAYLOAD_SIZE - offset,
      &cbor_payload, &payload_len);
  if (ret != 0) { /* LCOV_EXCL_START - zcbor always succeeds with 114-byte buffer */
    packet->length = 0;
    return;
  } /* LCOV_EXCL_STOP */

  packet->length = (uint8_t)(offset + payload_len);
}

void fusain_create_send_telemetry(fusain_packet_t* packet, uint64_t address,
    uint32_t telemetry_type, uint32_t index)
{
  packet->address = address;
  packet->msg_type = FUSAIN_MSG_SEND_TELEMETRY;

  size_t offset = 0;
  int header_len = encode_cbor_message_header(packet->payload,
      FUSAIN_MAX_PAYLOAD_SIZE, FUSAIN_MSG_SEND_TELEMETRY);
  if (header_len < 0) { /* LCOV_EXCL_START - unreachable with 114-byte buffer */
    packet->length = 0;
    return;
  } /* LCOV_EXCL_STOP */
  offset = (size_t)header_len;

  struct send_telemetry_payload cbor_payload = {
    .send_telemetry_payload_telemetry_type_m = telemetry_type,
    .send_telemetry_payload_uint1uint = { .send_telemetry_payload_uint1uint = index },
    .send_telemetry_payload_uint1uint_present = true,
  };
  size_t payload_len = 0;
  int ret = cbor_encode_send_telemetry_payload(
      packet->payload + offset, FUSAIN_MAX_PAYLOAD_SIZE - offset,
      &cbor_payload, &payload_len);
  if (ret != 0) { /* LCOV_EXCL_START - zcbor always succeeds with 114-byte buffer */
    packet->length = 0;
    return;
  } /* LCOV_EXCL_STOP */

  packet->length = (uint8_t)(offset + payload_len);
}

void fusain_create_state_data(fusain_packet_t* packet, uint64_t address,
    uint32_t error, uint8_t code, fusain_state_t state, uint32_t timestamp)
{
  packet->address = address;
  packet->msg_type = FUSAIN_MSG_STATE_DATA;

  size_t offset = 0;
  int header_len = encode_cbor_message_header(packet->payload,
      FUSAIN_MAX_PAYLOAD_SIZE, FUSAIN_MSG_STATE_DATA);
  if (header_len < 0) { /* LCOV_EXCL_START - unreachable with 114-byte buffer */
    packet->length = 0;
    return;
  } /* LCOV_EXCL_STOP */
  offset = (size_t)header_len;

  struct state_data_payload cbor_payload = {
    .state_data_payload_uint0bool = (error != 0),
    .state_data_payload_error_code_m = (uint32_t)code,
    .state_data_payload_state_m = (uint32_t)state,
    .state_data_payload_timestamp_m = timestamp,
  };
  size_t payload_len = 0;
  int ret = cbor_encode_state_data_payload(
      packet->payload + offset, FUSAIN_MAX_PAYLOAD_SIZE - offset,
      &cbor_payload, &payload_len);
  if (ret != 0) { /* LCOV_EXCL_START - zcbor always succeeds with 114-byte buffer */
    packet->length = 0;
    return;
  } /* LCOV_EXCL_STOP */

  packet->length = (uint8_t)(offset + payload_len);
}

void fusain_create_ping_response(fusain_packet_t* packet, uint64_t address,
    uint32_t uptime_ms)
{
  packet->address = address;
  packet->msg_type = FUSAIN_MSG_PING_RESPONSE;

  size_t offset = 0;
  int header_len = encode_cbor_message_header(packet->payload,
      FUSAIN_MAX_PAYLOAD_SIZE, FUSAIN_MSG_PING_RESPONSE);
  if (header_len < 0) { /* LCOV_EXCL_START - unreachable with 114-byte buffer */
    packet->length = 0;
    return;
  } /* LCOV_EXCL_STOP */
  offset = (size_t)header_len;

  struct ping_response_payload cbor_payload = {
    .ping_response_payload_timestamp_m = uptime_ms,
  };
  size_t payload_len = 0;
  int ret = cbor_encode_ping_response_payload(
      packet->payload + offset, FUSAIN_MAX_PAYLOAD_SIZE - offset,
      &cbor_payload, &payload_len);
  if (ret != 0) { /* LCOV_EXCL_START - zcbor always succeeds with 114-byte buffer */
    packet->length = 0;
    return;
  } /* LCOV_EXCL_STOP */

  packet->length = (uint8_t)(offset + payload_len);
}

/* v3.0 Configuration Command Functions (CBOR) */

void fusain_create_motor_config(fusain_packet_t* packet, uint64_t address,
    const fusain_cmd_motor_config_t* config)
{
  packet->address = address;
  packet->msg_type = FUSAIN_MSG_MOTOR_CONFIG;

  size_t offset = 0;
  int header_len = encode_cbor_message_header(packet->payload,
      FUSAIN_MAX_PAYLOAD_SIZE, FUSAIN_MSG_MOTOR_CONFIG);
  if (header_len < 0) { /* LCOV_EXCL_START - unreachable with 114-byte buffer */
    packet->length = 0;
    return;
  } /* LCOV_EXCL_STOP */
  offset = (size_t)header_len;

  /* Map C struct fields to CBOR optional fields
   * CDDL key mapping: 0=motor, 1=pwm-period, 2=pid-kp, 3=pid-ki, 4=pid-kd,
   *                   5=max-rpm, 6=min-rpm, 7=min-pwm-duty
   */
  struct motor_config_payload cbor_payload = {
    .motor_config_payload_motor_index_m = (uint32_t)config->motor,
    .motor_config_payload_uint1uint = { .motor_config_payload_uint1uint = config->pwm_period },
    .motor_config_payload_uint1uint_present = true,
    .motor_config_payload_uint2float = { .motor_config_payload_uint2float = config->pid_kp },
    .motor_config_payload_uint2float_present = true,
    .motor_config_payload_uint3float = { .motor_config_payload_uint3float = config->pid_ki },
    .motor_config_payload_uint3float_present = true,
    .motor_config_payload_uint4float = { .motor_config_payload_uint4float = config->pid_kd },
    .motor_config_payload_uint4float_present = true,
    .motor_config_payload_uint5int = { .motor_config_payload_uint5int = config->max_rpm },
    .motor_config_payload_uint5int_present = true,
    .motor_config_payload_uint6int = { .motor_config_payload_uint6int = config->min_rpm },
    .motor_config_payload_uint6int_present = true,
    .motor_config_payload_uint7uint = { .motor_config_payload_uint7uint = config->min_pwm_duty },
    .motor_config_payload_uint7uint_present = true,
  };
  size_t payload_len = 0;
  int ret = cbor_encode_motor_config_payload(
      packet->payload + offset, FUSAIN_MAX_PAYLOAD_SIZE - offset,
      &cbor_payload, &payload_len);
  if (ret != 0) { /* LCOV_EXCL_START - zcbor always succeeds with 114-byte buffer */
    packet->length = 0;
    return;
  } /* LCOV_EXCL_STOP */

  packet->length = (uint8_t)(offset + payload_len);
}

void fusain_create_pump_config(fusain_packet_t* packet, uint64_t address,
    const fusain_cmd_pump_config_t* config)
{
  packet->address = address;
  packet->msg_type = FUSAIN_MSG_PUMP_CONFIG;

  size_t offset = 0;
  int header_len = encode_cbor_message_header(packet->payload,
      FUSAIN_MAX_PAYLOAD_SIZE, FUSAIN_MSG_PUMP_CONFIG);
  if (header_len < 0) { /* LCOV_EXCL_START - unreachable with 114-byte buffer */
    packet->length = 0;
    return;
  } /* LCOV_EXCL_STOP */
  offset = (size_t)header_len;

  /* CDDL key mapping: 0=pump, 1=pulse-ms, 2=recovery-ms */
  struct pump_config_payload cbor_payload = {
    .pump_config_payload_pump_index_m = (uint32_t)config->pump,
    .pump_config_payload_uint1uint = { .pump_config_payload_uint1uint = config->pulse_ms },
    .pump_config_payload_uint1uint_present = true,
    .pump_config_payload_uint2uint = { .pump_config_payload_uint2uint = config->recovery_ms },
    .pump_config_payload_uint2uint_present = true,
  };
  size_t payload_len = 0;
  int ret = cbor_encode_pump_config_payload(
      packet->payload + offset, FUSAIN_MAX_PAYLOAD_SIZE - offset,
      &cbor_payload, &payload_len);
  if (ret != 0) { /* LCOV_EXCL_START - zcbor always succeeds with 114-byte buffer */
    packet->length = 0;
    return;
  } /* LCOV_EXCL_STOP */

  packet->length = (uint8_t)(offset + payload_len);
}

void fusain_create_temp_config(fusain_packet_t* packet, uint64_t address,
    const fusain_cmd_temp_config_t* config)
{
  packet->address = address;
  packet->msg_type = FUSAIN_MSG_TEMP_CONFIG;

  size_t offset = 0;
  int header_len = encode_cbor_message_header(packet->payload,
      FUSAIN_MAX_PAYLOAD_SIZE, FUSAIN_MSG_TEMP_CONFIG);
  if (header_len < 0) { /* LCOV_EXCL_START - unreachable with 114-byte buffer */
    packet->length = 0;
    return;
  } /* LCOV_EXCL_STOP */
  offset = (size_t)header_len;

  /* CDDL key mapping: 0=thermometer, 1=pid-kp, 2=pid-ki, 3=pid-kd */
  struct temp_config_payload cbor_payload = {
    .temp_config_payload_thermometer_index_m = (uint32_t)config->thermometer,
    .temp_config_payload_uint1float = { .temp_config_payload_uint1float = config->pid_kp },
    .temp_config_payload_uint1float_present = true,
    .temp_config_payload_uint2float = { .temp_config_payload_uint2float = config->pid_ki },
    .temp_config_payload_uint2float_present = true,
    .temp_config_payload_uint3float = { .temp_config_payload_uint3float = config->pid_kd },
    .temp_config_payload_uint3float_present = true,
  };
  size_t payload_len = 0;
  int ret = cbor_encode_temp_config_payload(
      packet->payload + offset, FUSAIN_MAX_PAYLOAD_SIZE - offset,
      &cbor_payload, &payload_len);
  if (ret != 0) { /* LCOV_EXCL_START - zcbor always succeeds with 114-byte buffer */
    packet->length = 0;
    return;
  } /* LCOV_EXCL_STOP */

  packet->length = (uint8_t)(offset + payload_len);
}

void fusain_create_glow_config(fusain_packet_t* packet, uint64_t address,
    const fusain_cmd_glow_config_t* config)
{
  packet->address = address;
  packet->msg_type = FUSAIN_MSG_GLOW_CONFIG;

  size_t offset = 0;
  int header_len = encode_cbor_message_header(packet->payload,
      FUSAIN_MAX_PAYLOAD_SIZE, FUSAIN_MSG_GLOW_CONFIG);
  if (header_len < 0) { /* LCOV_EXCL_START - unreachable with 114-byte buffer */
    packet->length = 0;
    return;
  } /* LCOV_EXCL_STOP */
  offset = (size_t)header_len;

  /* CDDL key mapping: 0=glow, 1=max-duration */
  struct glow_config_payload cbor_payload = {
    .glow_config_payload_glow_index_m = (uint32_t)config->glow,
    .glow_config_payload_uint1uint = { .glow_config_payload_uint1uint = config->max_duration_ms },
    .glow_config_payload_uint1uint_present = true,
  };
  size_t payload_len = 0;
  int ret = cbor_encode_glow_config_payload(
      packet->payload + offset, FUSAIN_MAX_PAYLOAD_SIZE - offset,
      &cbor_payload, &payload_len);
  if (ret != 0) { /* LCOV_EXCL_START - zcbor always succeeds with 114-byte buffer */
    packet->length = 0;
    return;
  } /* LCOV_EXCL_STOP */

  packet->length = (uint8_t)(offset + payload_len);
}

void fusain_create_data_subscription(fusain_packet_t* packet, uint64_t address,
    uint64_t appliance_address)
{
  packet->address = address;
  packet->msg_type = FUSAIN_MSG_DATA_SUBSCRIPTION;

  size_t offset = 0;
  int header_len = encode_cbor_message_header(packet->payload,
      FUSAIN_MAX_PAYLOAD_SIZE, FUSAIN_MSG_DATA_SUBSCRIPTION);
  if (header_len < 0) { /* LCOV_EXCL_START - unreachable with 114-byte buffer */
    packet->length = 0;
    return;
  } /* LCOV_EXCL_STOP */
  offset = (size_t)header_len;

  struct data_subscription_payload cbor_payload = {
    .data_subscription_payload_address_m = appliance_address,
  };
  size_t payload_len = 0;
  int ret = cbor_encode_data_subscription_payload(
      packet->payload + offset, FUSAIN_MAX_PAYLOAD_SIZE - offset,
      &cbor_payload, &payload_len);
  if (ret != 0) { /* LCOV_EXCL_START - zcbor always succeeds with 114-byte buffer */
    packet->length = 0;
    return;
  } /* LCOV_EXCL_STOP */

  packet->length = (uint8_t)(offset + payload_len);
}

void fusain_create_data_unsubscribe(fusain_packet_t* packet, uint64_t address,
    uint64_t appliance_address)
{
  packet->address = address;
  packet->msg_type = FUSAIN_MSG_DATA_UNSUBSCRIBE;

  size_t offset = 0;
  int header_len = encode_cbor_message_header(packet->payload,
      FUSAIN_MAX_PAYLOAD_SIZE, FUSAIN_MSG_DATA_UNSUBSCRIBE);
  if (header_len < 0) { /* LCOV_EXCL_START - unreachable with 114-byte buffer */
    packet->length = 0;
    return;
  } /* LCOV_EXCL_STOP */
  offset = (size_t)header_len;

  /* DATA_UNSUBSCRIBE uses same payload structure as DATA_SUBSCRIPTION */
  struct data_subscription_payload cbor_payload = {
    .data_subscription_payload_address_m = appliance_address,
  };
  size_t payload_len = 0;
  int ret = cbor_encode_data_subscription_payload(
      packet->payload + offset, FUSAIN_MAX_PAYLOAD_SIZE - offset,
      &cbor_payload, &payload_len);
  if (ret != 0) { /* LCOV_EXCL_START - zcbor always succeeds with 114-byte buffer */
    packet->length = 0;
    return;
  } /* LCOV_EXCL_STOP */

  packet->length = (uint8_t)(offset + payload_len);
}

void fusain_create_discovery_request(fusain_packet_t* packet, uint64_t address)
{
  packet->address = address;
  packet->msg_type = FUSAIN_MSG_DISCOVERY_REQUEST;

  size_t offset = 0;
  int header_len = encode_cbor_message_header(packet->payload,
      FUSAIN_MAX_PAYLOAD_SIZE, FUSAIN_MSG_DISCOVERY_REQUEST);
  if (header_len < 0) { /* LCOV_EXCL_START - unreachable with 114-byte buffer */
    packet->length = 0;
    return;
  } /* LCOV_EXCL_STOP */
  offset = (size_t)header_len;

  /* Encode nil for empty payload */
  int nil_len = encode_cbor_nil(packet->payload + offset, FUSAIN_MAX_PAYLOAD_SIZE - offset);
  if (nil_len < 0) { /* LCOV_EXCL_START - unreachable with 114-byte buffer */
    packet->length = 0;
    return;
  } /* LCOV_EXCL_STOP */

  packet->length = (uint8_t)(offset + (size_t)nil_len);
}

void fusain_create_device_announce(fusain_packet_t* packet, uint64_t address,
    uint8_t motor_count, uint8_t thermometer_count, uint8_t pump_count,
    uint8_t glow_count)
{
  packet->address = address;
  packet->msg_type = FUSAIN_MSG_DEVICE_ANNOUNCE;

  size_t offset = 0;
  int header_len = encode_cbor_message_header(packet->payload,
      FUSAIN_MAX_PAYLOAD_SIZE, FUSAIN_MSG_DEVICE_ANNOUNCE);
  if (header_len < 0) { /* LCOV_EXCL_START - unreachable with 114-byte buffer */
    packet->length = 0;
    return;
  } /* LCOV_EXCL_STOP */
  offset = (size_t)header_len;

  struct device_announce_payload cbor_payload = {
    .device_announce_payload_uint0uint = motor_count,
    .device_announce_payload_uint1uint = thermometer_count,
    .device_announce_payload_uint2uint = pump_count,
    .device_announce_payload_uint3uint = glow_count,
  };
  size_t payload_len = 0;
  int ret = cbor_encode_device_announce_payload(
      packet->payload + offset, FUSAIN_MAX_PAYLOAD_SIZE - offset,
      &cbor_payload, &payload_len);
  if (ret != 0) { /* LCOV_EXCL_START - zcbor always succeeds with 114-byte buffer */
    packet->length = 0;
    return;
  } /* LCOV_EXCL_STOP */

  packet->length = (uint8_t)(offset + payload_len);
}

void fusain_create_motor_data(fusain_packet_t* packet, uint64_t address,
    uint8_t motor, uint32_t timestamp, int32_t rpm, int32_t target)
{
  packet->address = address;
  packet->msg_type = FUSAIN_MSG_MOTOR_DATA;

  size_t offset = 0;
  int header_len = encode_cbor_message_header(packet->payload,
      FUSAIN_MAX_PAYLOAD_SIZE, FUSAIN_MSG_MOTOR_DATA);
  if (header_len < 0) { /* LCOV_EXCL_START - unreachable with 114-byte buffer */
    packet->length = 0;
    return;
  } /* LCOV_EXCL_STOP */
  offset = (size_t)header_len;

  struct motor_data_payload cbor_payload = {
    .motor_data_payload_motor_index_m = (uint32_t)motor,
    .motor_data_payload_timestamp_m = timestamp,
    .motor_data_payload_uint2int = rpm,
    .motor_data_payload_uint3int = target,
    .motor_data_payload_uint4int_present = false,
    .motor_data_payload_uint5int_present = false,
    .motor_data_payload_uint6uint_present = false,
    .motor_data_payload_uint7uint_present = false,
  };
  size_t payload_len = 0;
  int ret = cbor_encode_motor_data_payload(
      packet->payload + offset, FUSAIN_MAX_PAYLOAD_SIZE - offset,
      &cbor_payload, &payload_len);
  if (ret != 0) { /* LCOV_EXCL_START - zcbor always succeeds with 114-byte buffer */
    packet->length = 0;
    return;
  } /* LCOV_EXCL_STOP */

  packet->length = (uint8_t)(offset + payload_len);
}

void fusain_create_pump_data(fusain_packet_t* packet, uint64_t address,
    uint8_t pump, uint32_t timestamp, fusain_pump_event_t type, int32_t rate)
{
  packet->address = address;
  packet->msg_type = FUSAIN_MSG_PUMP_DATA;

  size_t offset = 0;
  int header_len = encode_cbor_message_header(packet->payload,
      FUSAIN_MAX_PAYLOAD_SIZE, FUSAIN_MSG_PUMP_DATA);
  if (header_len < 0) { /* LCOV_EXCL_START - unreachable with 114-byte buffer */
    packet->length = 0;
    return;
  } /* LCOV_EXCL_STOP */
  offset = (size_t)header_len;

  struct pump_data_payload cbor_payload = {
    .pump_data_payload_pump_index_m = (uint32_t)pump,
    .pump_data_payload_timestamp_m = timestamp,
    .pump_data_payload_pump_event_m = (uint32_t)type,
    .pump_data_payload_uint3int = { .pump_data_payload_uint3int = rate },
    .pump_data_payload_uint3int_present = true,
  };
  size_t payload_len = 0;
  int ret = cbor_encode_pump_data_payload(
      packet->payload + offset, FUSAIN_MAX_PAYLOAD_SIZE - offset,
      &cbor_payload, &payload_len);
  if (ret != 0) { /* LCOV_EXCL_START - zcbor always succeeds with 114-byte buffer */
    packet->length = 0;
    return;
  } /* LCOV_EXCL_STOP */

  packet->length = (uint8_t)(offset + payload_len);
}

void fusain_create_glow_data(fusain_packet_t* packet, uint64_t address,
    uint8_t glow, uint32_t timestamp, bool lit)
{
  packet->address = address;
  packet->msg_type = FUSAIN_MSG_GLOW_DATA;

  size_t offset = 0;
  int header_len = encode_cbor_message_header(packet->payload,
      FUSAIN_MAX_PAYLOAD_SIZE, FUSAIN_MSG_GLOW_DATA);
  if (header_len < 0) { /* LCOV_EXCL_START - unreachable with 114-byte buffer */
    packet->length = 0;
    return;
  } /* LCOV_EXCL_STOP */
  offset = (size_t)header_len;

  struct glow_data_payload cbor_payload = {
    .glow_data_payload_glow_index_m = (uint32_t)glow,
    .glow_data_payload_timestamp_m = timestamp,
    .glow_data_payload_uint2bool = lit,
  };
  size_t payload_len = 0;
  int ret = cbor_encode_glow_data_payload(
      packet->payload + offset, FUSAIN_MAX_PAYLOAD_SIZE - offset,
      &cbor_payload, &payload_len);
  if (ret != 0) { /* LCOV_EXCL_START - zcbor always succeeds with 114-byte buffer */
    packet->length = 0;
    return;
  } /* LCOV_EXCL_STOP */

  packet->length = (uint8_t)(offset + payload_len);
}

void fusain_create_temp_data(fusain_packet_t* packet, uint64_t address,
    uint8_t thermometer, uint32_t timestamp, float reading)
{
  packet->address = address;
  packet->msg_type = FUSAIN_MSG_TEMP_DATA;

  size_t offset = 0;
  int header_len = encode_cbor_message_header(packet->payload,
      FUSAIN_MAX_PAYLOAD_SIZE, FUSAIN_MSG_TEMP_DATA);
  if (header_len < 0) { /* LCOV_EXCL_START - unreachable with 114-byte buffer */
    packet->length = 0;
    return;
  } /* LCOV_EXCL_STOP */
  offset = (size_t)header_len;

  struct temp_data_payload cbor_payload = {
    .temp_data_payload_thermometer_index_m = (uint32_t)thermometer,
    .temp_data_payload_timestamp_m = timestamp,
    .temp_data_payload_uint2float = reading,
    .temp_data_payload_uint3bool_present = false,
    .temp_data_payload_uint4int_present = false,
    .temp_data_payload_uint5float_present = false,
  };
  size_t payload_len = 0;
  int ret = cbor_encode_temp_data_payload(
      packet->payload + offset, FUSAIN_MAX_PAYLOAD_SIZE - offset,
      &cbor_payload, &payload_len);
  if (ret != 0) { /* LCOV_EXCL_START - zcbor always succeeds with 114-byte buffer */
    packet->length = 0;
    return;
  } /* LCOV_EXCL_STOP */

  packet->length = (uint8_t)(offset + payload_len);
}

void fusain_create_error_invalid_cmd(fusain_packet_t* packet, uint64_t address,
    fusain_invalid_cmd_error_t error_code, int32_t rejected_field,
    int32_t constraint)
{
  packet->address = address;
  packet->msg_type = FUSAIN_MSG_ERROR_INVALID_CMD;

  size_t offset = 0;
  int header_len = encode_cbor_message_header(packet->payload,
      FUSAIN_MAX_PAYLOAD_SIZE, FUSAIN_MSG_ERROR_INVALID_CMD);
  if (header_len < 0) { /* LCOV_EXCL_START - unreachable with 114-byte buffer */
    packet->length = 0;
    return;
  } /* LCOV_EXCL_STOP */
  offset = (size_t)header_len;

  struct error_invalid_cmd_payload cbor_payload = {
    .error_invalid_cmd_payload_uint0int = (int32_t)error_code,
    .error_invalid_cmd_payload_uint1uint_present = (rejected_field >= 0),
    .error_invalid_cmd_payload_constraint_m_present = (constraint >= 0),
  };
  if (rejected_field >= 0) {
    cbor_payload.error_invalid_cmd_payload_uint1uint
        .error_invalid_cmd_payload_uint1uint
        = (uint32_t)rejected_field;
  }
  if (constraint >= 0) {
    cbor_payload.error_invalid_cmd_payload_constraint_m
        .error_invalid_cmd_payload_constraint_m
        = (uint32_t)constraint;
  }
  size_t payload_len = 0;
  int ret = cbor_encode_error_invalid_cmd_payload(
      packet->payload + offset, FUSAIN_MAX_PAYLOAD_SIZE - offset,
      &cbor_payload, &payload_len);
  if (ret != 0) { /* LCOV_EXCL_START - zcbor always succeeds with 114-byte buffer */
    packet->length = 0;
    return;
  } /* LCOV_EXCL_STOP */

  packet->length = (uint8_t)(offset + payload_len);
}

void fusain_create_error_state_reject(fusain_packet_t* packet, uint64_t address,
    fusain_state_t state, int32_t rejection_reason)
{
  packet->address = address;
  packet->msg_type = FUSAIN_MSG_ERROR_STATE_REJECT;

  size_t offset = 0;
  int header_len = encode_cbor_message_header(packet->payload,
      FUSAIN_MAX_PAYLOAD_SIZE, FUSAIN_MSG_ERROR_STATE_REJECT);
  if (header_len < 0) { /* LCOV_EXCL_START - unreachable with 114-byte buffer */
    packet->length = 0;
    return;
  } /* LCOV_EXCL_STOP */
  offset = (size_t)header_len;

  struct error_state_reject_payload cbor_payload = {
    .error_state_reject_payload_uint0int = (int32_t)state,
    .error_state_reject_payload_rejection_reason_m_present = (rejection_reason >= 0),
  };
  if (rejection_reason >= 0) {
    cbor_payload.error_state_reject_payload_rejection_reason_m
        .error_state_reject_payload_rejection_reason_m
        = (uint32_t)rejection_reason;
  }
  size_t payload_len = 0;
  int ret = cbor_encode_error_state_reject_payload(
      packet->payload + offset, FUSAIN_MAX_PAYLOAD_SIZE - offset,
      &cbor_payload, &payload_len);
  if (ret != 0) { /* LCOV_EXCL_START - zcbor always succeeds with 114-byte buffer */
    packet->length = 0;
    return;
  } /* LCOV_EXCL_STOP */

  packet->length = (uint8_t)(offset + payload_len);
}
