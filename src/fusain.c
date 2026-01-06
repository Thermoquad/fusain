/*
 * Copyright (c) 2025 Kaz Walker, Thermoquad
 * SPDX-License-Identifier: Apache-2.0
 *
 * Fusain Serial Protocol - Shared Library Implementation
 */
#include <stdint.h>
#include <string.h>

#include <fusain/fusain.h>

/* Decoder States */
#define DECODER_STATE_IDLE 0
#define DECODER_STATE_LENGTH 1
#define DECODER_STATE_TYPE 2
#define DECODER_STATE_PAYLOAD 3
#define DECODER_STATE_CRC1 4
#define DECODER_STATE_CRC2 5
#define DECODER_STATE_END 6

/* CRC-16-CCITT Calculation */
uint16_t fusain_crc16(const uint8_t* data, size_t length)
{
  uint16_t crc = 0xFFFF; // Initial value
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

/* Packet Encoding */
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
  if (index >= buffer_size) {
    return -3;
  }
  buffer[index++] = FUSAIN_START_BYTE;

  // Calculate CRC over LENGTH + TYPE + PAYLOAD
  uint8_t crc_data[FUSAIN_MAX_PAYLOAD_SIZE + 2];
  crc_data[0] = packet->length;
  crc_data[1] = packet->msg_type;
  memcpy(&crc_data[2], packet->payload, packet->length);
  uint16_t crc = fusain_crc16(crc_data, packet->length + 2);

  // Stuff LENGTH byte
  if (stuff_byte(packet->length, buffer, buffer_size, &index) < 0) {
    return -4;
  }

  // Stuff TYPE byte
  if (stuff_byte(packet->msg_type, buffer, buffer_size, &index) < 0) {
    return -5;
  }

  // Stuff PAYLOAD bytes
  for (uint8_t i = 0; i < packet->length; i++) {
    if (stuff_byte(packet->payload[i], buffer, buffer_size, &index) < 0) {
      return -6;
    }
  }

  // Stuff CRC bytes (big-endian)
  uint8_t crc_high = (crc >> 8) & 0xFF;
  uint8_t crc_low = crc & 0xFF;
  if (stuff_byte(crc_high, buffer, buffer_size, &index) < 0) {
    return -7;
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

/* Packet Decoding */
fusain_decode_result_t fusain_decode_byte(uint8_t rx_byte,
    fusain_packet_t* packet,
    fusain_decoder_t* decoder)
{
  // Handle START byte - reset decoder
  if (rx_byte == FUSAIN_START_BYTE && !(decoder->escape_next)) {
    decoder->state = DECODER_STATE_LENGTH;
    decoder->buffer_index = 0;
    decoder->escape_next = false;
    return FUSAIN_DECODE_INCOMPLETE;
  }

  // Handle escape sequences
  if (rx_byte == FUSAIN_ESC_BYTE && !(decoder->escape_next)) {
    decoder->escape_next = true;
    return FUSAIN_DECODE_INCOMPLETE;
  }

  // Unescape byte if needed
  uint8_t byte = rx_byte;
  if (decoder->escape_next) {
    byte ^= FUSAIN_ESC_XOR;
    decoder->escape_next = false;
  }

  // State machine
  switch (decoder->state) {
  case DECODER_STATE_IDLE:
    // Waiting for START byte
    return FUSAIN_DECODE_INCOMPLETE;

  case DECODER_STATE_LENGTH:
    if (byte > FUSAIN_MAX_PAYLOAD_SIZE) {
      decoder->state = DECODER_STATE_IDLE;
      return FUSAIN_DECODE_INVALID_LENGTH;
    }
    packet->length = byte;
    decoder->buffer[decoder->buffer_index++] = byte;
    decoder->state = DECODER_STATE_TYPE;
    return FUSAIN_DECODE_INCOMPLETE;

  case DECODER_STATE_TYPE:
    packet->msg_type = byte;
    decoder->buffer[decoder->buffer_index++] = byte;
    if (packet->length == 0) {
      // No payload, move to CRC
      decoder->state = DECODER_STATE_CRC1;
    } else {
      decoder->state = DECODER_STATE_PAYLOAD;
    }
    return FUSAIN_DECODE_INCOMPLETE;

  case DECODER_STATE_PAYLOAD:
    packet->payload[decoder->buffer_index - 2] = byte;
    decoder->buffer[decoder->buffer_index++] = byte;
    if (decoder->buffer_index >= packet->length + 2) {
      // Payload complete, move to CRC
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

  case DECODER_STATE_END:
    if (rx_byte != FUSAIN_END_BYTE) {
      decoder->state = DECODER_STATE_IDLE;
      return FUSAIN_DECODE_INVALID_START;
    }

    // Validate CRC
    uint16_t calculated_crc = fusain_crc16(decoder->buffer, packet->length + 2);
    if (calculated_crc != packet->crc) {
      decoder->state = DECODER_STATE_IDLE;
      return FUSAIN_DECODE_INVALID_CRC;
    }

    // Packet complete and valid
    decoder->state = DECODER_STATE_IDLE;
    return FUSAIN_DECODE_OK;

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
}

/* Helper Functions to Create Packets */

void fusain_create_set_mode(fusain_packet_t* packet, fusain_mode_t mode,
    uint32_t parameter)
{
  fusain_cmd_set_mode_t cmd = { .mode = mode, .parameter = parameter };
  packet->length = sizeof(cmd);
  packet->msg_type = FUSAIN_MSG_STATE_COMMAND;
  memcpy(packet->payload, &cmd, sizeof(cmd));
}

void fusain_create_set_pump_rate(fusain_packet_t* packet, uint32_t rate_ms)
{
  fusain_cmd_set_pump_rate_t cmd = { .rate_ms = rate_ms };
  packet->length = sizeof(cmd);
  packet->msg_type = FUSAIN_MSG_PUMP_COMMAND;
  memcpy(packet->payload, &cmd, sizeof(cmd));
}

void fusain_create_set_target_rpm(fusain_packet_t* packet,
    uint32_t target_rpm)
{
  fusain_cmd_set_target_rpm_t cmd = { .target_rpm = target_rpm };
  packet->length = sizeof(cmd);
  packet->msg_type = FUSAIN_MSG_MOTOR_COMMAND;
  memcpy(packet->payload, &cmd, sizeof(cmd));
}

void fusain_create_glow_command(fusain_packet_t* packet, int32_t glow,
    int32_t duration)
{
  fusain_cmd_glow_t cmd = { .glow = glow, .duration = duration };
  packet->length = sizeof(cmd);
  packet->msg_type = FUSAIN_MSG_GLOW_COMMAND;
  memcpy(packet->payload, &cmd, sizeof(cmd));
}

void fusain_create_ping_request(fusain_packet_t* packet)
{
  packet->length = 0;
  packet->msg_type = FUSAIN_MSG_PING_REQUEST;
}

void fusain_create_telemetry_config(fusain_packet_t* packet, bool enabled,
    uint32_t interval_ms, uint32_t mode)
{
  fusain_cmd_telemetry_config_t cmd = { .telemetry_enabled = enabled ? 1 : 0,
    .interval_ms = interval_ms,
    .telemetry_mode = mode };
  packet->length = sizeof(cmd);
  packet->msg_type = FUSAIN_MSG_TELEMETRY_CONFIG;
  memcpy(packet->payload, &cmd, sizeof(cmd));
}

void fusain_create_state_data(fusain_packet_t* packet, fusain_state_t state,
    fusain_error_t error)
{
  fusain_data_state_t data = { .state = state, .error = error };
  packet->length = sizeof(data);
  packet->msg_type = FUSAIN_MSG_STATE_DATA;
  memcpy(packet->payload, &data, sizeof(data));
}

void fusain_create_ping_response(fusain_packet_t* packet, uint64_t uptime_ms)
{
  fusain_data_ping_response_t data = { .uptime_ms = uptime_ms };
  packet->length = sizeof(data);
  packet->msg_type = FUSAIN_MSG_PING_RESPONSE;
  memcpy(packet->payload, &data, sizeof(data));
}

int fusain_create_telemetry_bundle(
    fusain_packet_t* packet, fusain_state_t state, fusain_error_t error,
    const fusain_telemetry_motor_t* motors, uint8_t motor_count,
    const fusain_telemetry_temperature_t* temperatures, uint8_t temp_count)
{
  // Validate counts
  if (motor_count > FUSAIN_MAX_MOTORS || temp_count > FUSAIN_MAX_TEMPERATURES || motor_count == 0 || temp_count == 0) {
    return -1;
  }

  // Calculate payload size
  size_t payload_size = sizeof(fusain_data_telemetry_bundle_t) + (motor_count * sizeof(fusain_telemetry_motor_t)) + (temp_count * sizeof(fusain_telemetry_temperature_t));

  if (payload_size > FUSAIN_MAX_PAYLOAD_SIZE) {
    return -2; // Payload too large
  }

  // Build packet
  fusain_data_telemetry_bundle_t bundle = {
    .state = state,
    .error = error,
    .motor_count = motor_count,
    .temp_count = temp_count,
  };

  uint8_t* payload_ptr = packet->payload;
  memcpy(payload_ptr, &bundle, sizeof(bundle));
  payload_ptr += sizeof(bundle);

  // Copy motor data
  memcpy(payload_ptr, motors, motor_count * sizeof(fusain_telemetry_motor_t));
  payload_ptr += motor_count * sizeof(fusain_telemetry_motor_t);

  // Copy temperature data
  memcpy(payload_ptr, temperatures,
      temp_count * sizeof(fusain_telemetry_temperature_t));

  packet->length = (uint8_t)payload_size;
  packet->msg_type = FUSAIN_MSG_TELEMETRY_BUNDLE;

  return 0;
}

/* v2.0 Configuration Command Functions */

void fusain_create_motor_config(fusain_packet_t* packet,
    const fusain_cmd_motor_config_t* config)
{
  packet->length = sizeof(fusain_cmd_motor_config_t);
  packet->msg_type = FUSAIN_MSG_MOTOR_CONFIG;
  memcpy(packet->payload, config, sizeof(fusain_cmd_motor_config_t));
}

void fusain_create_pump_config(fusain_packet_t* packet,
    const fusain_cmd_pump_config_t* config)
{
  packet->length = sizeof(fusain_cmd_pump_config_t);
  packet->msg_type = FUSAIN_MSG_PUMP_CONFIG;
  memcpy(packet->payload, config, sizeof(fusain_cmd_pump_config_t));
}

void fusain_create_temp_config(fusain_packet_t* packet,
    const fusain_cmd_temp_config_t* config)
{
  packet->length = sizeof(fusain_cmd_temp_config_t);
  packet->msg_type = FUSAIN_MSG_TEMP_CONFIG;
  memcpy(packet->payload, config, sizeof(fusain_cmd_temp_config_t));
}

void fusain_create_glow_config(fusain_packet_t* packet,
    const fusain_cmd_glow_config_t* config)
{
  packet->length = sizeof(fusain_cmd_glow_config_t);
  packet->msg_type = FUSAIN_MSG_GLOW_CONFIG;
  memcpy(packet->payload, config, sizeof(fusain_cmd_glow_config_t));
}

void fusain_create_data_subscription(fusain_packet_t* packet,
    uint64_t appliance_address, uint64_t message_filter)
{
  fusain_cmd_data_subscription_t cmd = {
    .appliance_address = appliance_address,
    .message_filter = message_filter
  };
  packet->length = sizeof(cmd);
  packet->msg_type = FUSAIN_MSG_DATA_SUBSCRIPTION;
  memcpy(packet->payload, &cmd, sizeof(cmd));
}

void fusain_create_data_unsubscribe(fusain_packet_t* packet,
    uint64_t appliance_address)
{
  fusain_cmd_data_unsubscribe_t cmd = {
    .appliance_address = appliance_address
  };
  packet->length = sizeof(cmd);
  packet->msg_type = FUSAIN_MSG_DATA_UNSUBSCRIBE;
  memcpy(packet->payload, &cmd, sizeof(cmd));
}

void fusain_create_discovery_request(fusain_packet_t* packet)
{
  packet->length = 0;
  packet->msg_type = FUSAIN_MSG_DISCOVERY_REQUEST;
}

void fusain_create_device_announce(fusain_packet_t* packet,
    uint32_t device_type, uint32_t capabilities)
{
  fusain_data_device_announce_t data = {
    .device_type = device_type,
    .capabilities = capabilities
  };
  packet->length = sizeof(data);
  packet->msg_type = FUSAIN_MSG_DEVICE_ANNOUNCE;
  memcpy(packet->payload, &data, sizeof(data));
}
