/*
 * Copyright (c) 2025 Kaz Walker, Thermoquad
 * SPDX-License-Identifier: Apache-2.0
 *
 * Fusain Protocol Library - Packet Creation Tests
 *
 * These tests verify that packet creation functions produce valid CBOR payloads
 * that can be successfully round-tripped through encode/decode.
 */

#include <fusain/fusain.h>
#include <string.h>
#include <zephyr/ztest.h>

/* Helper to verify a packet can be encoded and decoded */
static bool verify_roundtrip(fusain_packet_t* tx_packet, const char* name)
{
  (void)name; /* Used for debugging, silences unused parameter warning */
  uint8_t buffer[FUSAIN_MAX_PACKET_SIZE * 2];
  int len = fusain_encode_packet(tx_packet, buffer, sizeof(buffer));
  if (len <= 0) {
    return false;
  }

  fusain_decoder_t decoder;
  fusain_reset_decoder(&decoder);

  fusain_packet_t rx_packet;
  fusain_decode_result_t result = FUSAIN_DECODE_INCOMPLETE;

  for (int i = 0; i < len; i++) {
    result = fusain_decode_byte(buffer[i], &rx_packet, &decoder);
  }

  if (result != FUSAIN_DECODE_OK) {
    return false;
  }

  if (rx_packet.msg_type != tx_packet->msg_type) {
    return false;
  }

  if (rx_packet.address != tx_packet->address) {
    return false;
  }

  return true;
}

/* Test control command packet creation */
ZTEST(fusain_packets, test_create_state_command)
{
  fusain_packet_t packet;
  fusain_create_state_command(&packet, 0x1234, FUSAIN_MODE_HEAT, 2500);

  zassert_equal(packet.msg_type, FUSAIN_MSG_STATE_COMMAND, "Type should match");
  zassert_equal(packet.address, 0x1234, "Address should match");
  zassert_true(packet.length > 0, "Payload should not be empty");
  zassert_true(verify_roundtrip(&packet, "state_command"), "Round-trip should succeed");
}

ZTEST(fusain_packets, test_create_pump_command)
{
  fusain_packet_t packet;
  fusain_create_pump_command(&packet, 0x5678, 0, 250);

  zassert_equal(packet.msg_type, FUSAIN_MSG_PUMP_COMMAND, "Type should match");
  zassert_equal(packet.address, 0x5678, "Address should match");
  zassert_true(packet.length > 0, "Payload should not be empty");
  zassert_true(verify_roundtrip(&packet, "pump_command"), "Round-trip should succeed");
}

ZTEST(fusain_packets, test_create_motor_command)
{
  fusain_packet_t packet;
  fusain_create_motor_command(&packet, 0xABCD, 0, 3000);

  zassert_equal(packet.msg_type, FUSAIN_MSG_MOTOR_COMMAND, "Type should match");
  zassert_equal(packet.address, 0xABCD, "Address should match");
  zassert_true(packet.length > 0, "Payload should not be empty");
  zassert_true(verify_roundtrip(&packet, "motor_command"), "Round-trip should succeed");
}

ZTEST(fusain_packets, test_create_glow_command)
{
  fusain_packet_t packet;
  fusain_create_glow_command(&packet, 0xEF01, 0, 30000);

  zassert_equal(packet.msg_type, FUSAIN_MSG_GLOW_COMMAND, "Type should match");
  zassert_equal(packet.address, 0xEF01, "Address should match");
  zassert_true(packet.length > 0, "Payload should not be empty");
  zassert_true(verify_roundtrip(&packet, "glow_command"), "Round-trip should succeed");
}

ZTEST(fusain_packets, test_create_ping_request)
{
  fusain_packet_t packet;
  fusain_create_ping_request(&packet, 0x2345);

  zassert_equal(packet.msg_type, FUSAIN_MSG_PING_REQUEST, "Type should match");
  zassert_equal(packet.address, 0x2345, "Address should match");
  /* CBOR ping_request has [type, nil] = 3-4 bytes */
  zassert_true(packet.length >= 3, "CBOR payload should have header + nil");
  zassert_true(verify_roundtrip(&packet, "ping_request"), "Round-trip should succeed");
}

ZTEST(fusain_packets, test_create_telemetry_config)
{
  fusain_packet_t packet;
  fusain_create_telemetry_config(&packet, 0x6789, true, 100);

  zassert_equal(packet.msg_type, FUSAIN_MSG_TELEMETRY_CONFIG, "Type should match");
  zassert_equal(packet.address, 0x6789, "Address should match");
  zassert_true(packet.length > 0, "Payload should not be empty");
  zassert_true(verify_roundtrip(&packet, "telemetry_config"), "Round-trip should succeed");
}

ZTEST(fusain_packets, test_create_timeout_config)
{
  fusain_packet_t packet;
  fusain_create_timeout_config(&packet, 0xABCD, true, 30000);

  zassert_equal(packet.msg_type, FUSAIN_MSG_TIMEOUT_CONFIG, "Type should match");
  zassert_equal(packet.address, 0xABCD, "Address should match");
  zassert_true(packet.length > 0, "Payload should not be empty");
  zassert_true(verify_roundtrip(&packet, "timeout_config"), "Round-trip should succeed");
}

ZTEST(fusain_packets, test_create_send_telemetry)
{
  fusain_packet_t packet;
  fusain_create_send_telemetry(&packet, 0xEF01, 1, 0xFFFFFFFF);

  zassert_equal(packet.msg_type, FUSAIN_MSG_SEND_TELEMETRY, "Type should match");
  zassert_equal(packet.address, 0xEF01, "Address should match");
  zassert_true(packet.length > 0, "Payload should not be empty");
  zassert_true(verify_roundtrip(&packet, "send_telemetry"), "Round-trip should succeed");
}

/* Test config command packet creation */
ZTEST(fusain_packets, test_create_motor_config)
{
  fusain_cmd_motor_config_t config = {
    .motor = 0,
    .pwm_period = 1000,
    .pid_kp = 4.0,
    .pid_ki = 12.0,
    .pid_kd = 0.1,
    .max_rpm = 3400,
    .min_rpm = 800,
    .min_pwm_duty = 10,
  };

  fusain_packet_t packet;
  fusain_create_motor_config(&packet, 0x1234, &config);

  zassert_equal(packet.msg_type, FUSAIN_MSG_MOTOR_CONFIG, "Type should match");
  zassert_equal(packet.address, 0x1234, "Address should match");
  zassert_true(packet.length > 0, "Payload should not be empty");
  zassert_true(verify_roundtrip(&packet, "motor_config"), "Round-trip should succeed");
}

ZTEST(fusain_packets, test_create_pump_config)
{
  fusain_cmd_pump_config_t config = {
    .pump = 0,
    .pulse_ms = 100,
    .recovery_ms = 1000,
  };

  fusain_packet_t packet;
  fusain_create_pump_config(&packet, 0x5678, &config);

  zassert_equal(packet.msg_type, FUSAIN_MSG_PUMP_CONFIG, "Type should match");
  zassert_equal(packet.address, 0x5678, "Address should match");
  zassert_true(packet.length > 0, "Payload should not be empty");
  zassert_true(verify_roundtrip(&packet, "pump_config"), "Round-trip should succeed");
}

ZTEST(fusain_packets, test_create_temp_config)
{
  fusain_cmd_temp_config_t config = {
    .thermometer = 0,
    .pid_kp = 100.0,
    .pid_ki = 10.0,
    .pid_kd = 5.0,
  };

  fusain_packet_t packet;
  fusain_create_temp_config(&packet, 0xABCD, &config);

  zassert_equal(packet.msg_type, FUSAIN_MSG_TEMP_CONFIG, "Type should match");
  zassert_equal(packet.address, 0xABCD, "Address should match");
  zassert_true(packet.length > 0, "Payload should not be empty");
  zassert_true(verify_roundtrip(&packet, "temp_config"), "Round-trip should succeed");
}

ZTEST(fusain_packets, test_create_glow_config)
{
  fusain_cmd_glow_config_t config = {
    .glow = 0,
    .max_duration_ms = 300000,
  };

  fusain_packet_t packet;
  fusain_create_glow_config(&packet, 0xEF01, &config);

  zassert_equal(packet.msg_type, FUSAIN_MSG_GLOW_CONFIG, "Type should match");
  zassert_equal(packet.address, 0xEF01, "Address should match");
  zassert_true(packet.length > 0, "Payload should not be empty");
  zassert_true(verify_roundtrip(&packet, "glow_config"), "Round-trip should succeed");
}

ZTEST(fusain_packets, test_create_data_subscription)
{
  fusain_packet_t packet;
  fusain_create_data_subscription(&packet, 0x2345, 0x123456789ABCDEF0ULL);

  zassert_equal(packet.msg_type, FUSAIN_MSG_DATA_SUBSCRIPTION, "Type should match");
  zassert_equal(packet.address, 0x2345, "Address should match");
  zassert_true(packet.length > 0, "Payload should not be empty");
  zassert_true(verify_roundtrip(&packet, "data_subscription"), "Round-trip should succeed");
}

ZTEST(fusain_packets, test_create_data_unsubscribe)
{
  fusain_packet_t packet;
  fusain_create_data_unsubscribe(&packet, 0x6789, 0x123456789ABCDEF0ULL);

  zassert_equal(packet.msg_type, FUSAIN_MSG_DATA_UNSUBSCRIBE, "Type should match");
  zassert_equal(packet.address, 0x6789, "Address should match");
  zassert_true(packet.length > 0, "Payload should not be empty");
  zassert_true(verify_roundtrip(&packet, "data_unsubscribe"), "Round-trip should succeed");
}

ZTEST(fusain_packets, test_create_discovery_request)
{
  fusain_packet_t packet;
  fusain_create_discovery_request(&packet, 0xABCD);

  zassert_equal(packet.msg_type, FUSAIN_MSG_DISCOVERY_REQUEST, "Type should match");
  zassert_equal(packet.address, 0xABCD, "Address should match");
  /* CBOR discovery_request has [type, nil] = 3-4 bytes */
  zassert_true(packet.length >= 3, "CBOR payload should have header + nil");
  zassert_true(verify_roundtrip(&packet, "discovery_request"), "Round-trip should succeed");
}

/* Test data packet creation */
ZTEST(fusain_packets, test_create_state_data)
{
  fusain_packet_t packet;
  fusain_create_state_data(&packet, 0xEF01, 1, 0x42, FUSAIN_STATE_HEATING, 12345);

  zassert_equal(packet.msg_type, FUSAIN_MSG_STATE_DATA, "Type should match");
  zassert_equal(packet.address, 0xEF01, "Address should match");
  zassert_true(packet.length > 0, "Payload should not be empty");
  zassert_true(verify_roundtrip(&packet, "state_data"), "Round-trip should succeed");
}

ZTEST(fusain_packets, test_create_ping_response)
{
  fusain_packet_t packet;
  fusain_create_ping_response(&packet, 0x2345, 123456789);

  zassert_equal(packet.msg_type, FUSAIN_MSG_PING_RESPONSE, "Type should match");
  zassert_equal(packet.address, 0x2345, "Address should match");
  zassert_true(packet.length > 0, "Payload should not be empty");
  zassert_true(verify_roundtrip(&packet, "ping_response"), "Round-trip should succeed");
}

ZTEST(fusain_packets, test_create_device_announce)
{
  fusain_packet_t packet;
  fusain_create_device_announce(&packet, 0x6789, 1, 1, 1, 1);

  zassert_equal(packet.msg_type, FUSAIN_MSG_DEVICE_ANNOUNCE, "Type should match");
  zassert_equal(packet.address, 0x6789, "Address should match");
  zassert_true(packet.length > 0, "Payload should not be empty");
  zassert_true(verify_roundtrip(&packet, "device_announce"), "Round-trip should succeed");
}

/* Test packet creation round-trip for different message types */
ZTEST(fusain_packets, test_packet_roundtrip)
{
  fusain_packet_t tx_packet;
  const char* test_names[] = { "ping_request", "set_mode", "state_data",
    "discovery_request", "device_announce" };

  for (int test_idx = 0; test_idx < 5; test_idx++) {
    /* Create different packet types */
    switch (test_idx) {
    case 0:
      fusain_create_ping_request(&tx_packet, 0x1111);
      break;
    case 1:
      fusain_create_state_command(&tx_packet, 0x2222, FUSAIN_MODE_HEAT, 2500);
      break;
    case 2:
      fusain_create_state_data(&tx_packet, 0x3333, 0, 0, FUSAIN_STATE_HEATING, 12345);
      break;
    case 3:
      fusain_create_discovery_request(&tx_packet, 0x4444);
      break;
    case 4:
      fusain_create_device_announce(&tx_packet, 0x5555, 1, 1, 1, 1);
      break;
    }

    zassert_true(verify_roundtrip(&tx_packet, test_names[test_idx]),
        "%s: Round-trip should succeed", test_names[test_idx]);
  }
}

/* Test TEMP_COMMAND packet creation */
ZTEST(fusain_packets, test_create_temp_command)
{
  fusain_packet_t packet;
  fusain_create_temp_command(&packet, 0xABCD, 0, FUSAIN_TEMP_CMD_SET_TARGET_TEMP, 0, 220.5);

  zassert_equal(packet.msg_type, FUSAIN_MSG_TEMP_COMMAND, "Type should match");
  zassert_equal(packet.address, 0xABCD, "Address should match");
  zassert_true(packet.length > 0, "Payload should not be empty");
  zassert_true(verify_roundtrip(&packet, "temp_command"), "Round-trip should succeed");
}

/* Test MOTOR_DATA packet creation */
ZTEST(fusain_packets, test_create_motor_data)
{
  fusain_packet_t packet;
  fusain_create_motor_data(&packet, 0x1234, 0, 12345, 3000, 3500);

  zassert_equal(packet.msg_type, FUSAIN_MSG_MOTOR_DATA, "Type should match");
  zassert_equal(packet.address, 0x1234, "Address should match");
  zassert_true(packet.length > 0, "Payload should not be empty");
  zassert_true(verify_roundtrip(&packet, "motor_data"), "Round-trip should succeed");
}

/* Test PUMP_DATA packet creation */
ZTEST(fusain_packets, test_create_pump_data)
{
  fusain_packet_t packet;
  fusain_create_pump_data(&packet, 0x2345, 0, 12345, FUSAIN_PUMP_EVENT_CYCLE_START, 100);

  zassert_equal(packet.msg_type, FUSAIN_MSG_PUMP_DATA, "Type should match");
  zassert_equal(packet.address, 0x2345, "Address should match");
  zassert_true(packet.length > 0, "Payload should not be empty");
  zassert_true(verify_roundtrip(&packet, "pump_data"), "Round-trip should succeed");
}

/* Test GLOW_DATA packet creation */
ZTEST(fusain_packets, test_create_glow_data)
{
  fusain_packet_t packet;
  fusain_create_glow_data(&packet, 0x3456, 0, 12345, true);

  zassert_equal(packet.msg_type, FUSAIN_MSG_GLOW_DATA, "Type should match");
  zassert_equal(packet.address, 0x3456, "Address should match");
  zassert_true(packet.length > 0, "Payload should not be empty");
  zassert_true(verify_roundtrip(&packet, "glow_data"), "Round-trip should succeed");
}

/* Test TEMP_DATA packet creation */
ZTEST(fusain_packets, test_create_temp_data)
{
  fusain_packet_t packet;
  fusain_create_temp_data(&packet, 0x4567, 0, 12345, 85.5);

  zassert_equal(packet.msg_type, FUSAIN_MSG_TEMP_DATA, "Type should match");
  zassert_equal(packet.address, 0x4567, "Address should match");
  zassert_true(packet.length > 0, "Payload should not be empty");
  zassert_true(verify_roundtrip(&packet, "temp_data"), "Round-trip should succeed");
}

/* Test ERROR_INVALID_CMD packet creation */
ZTEST(fusain_packets, test_create_error_invalid_cmd)
{
  fusain_packet_t packet;
  fusain_create_error_invalid_cmd(&packet, 0x5678, 0x42);

  zassert_equal(packet.msg_type, FUSAIN_MSG_ERROR_INVALID_CMD, "Type should match");
  zassert_equal(packet.address, 0x5678, "Address should match");
  zassert_true(packet.length > 0, "Payload should not be empty");
  zassert_true(verify_roundtrip(&packet, "error_invalid_cmd"), "Round-trip should succeed");
}

/* Test ERROR_STATE_REJECT packet creation */
ZTEST(fusain_packets, test_create_error_state_reject)
{
  fusain_packet_t packet;
  fusain_create_error_state_reject(&packet, 0x6789, FUSAIN_STATE_HEATING);

  zassert_equal(packet.msg_type, FUSAIN_MSG_ERROR_STATE_REJECT, "Type should match");
  zassert_equal(packet.address, 0x6789, "Address should match");
  zassert_true(packet.length > 0, "Payload should not be empty");
  zassert_true(verify_roundtrip(&packet, "error_state_reject"), "Round-trip should succeed");
}

/* Test suite setup */
ZTEST_SUITE(fusain_packets, NULL, NULL, NULL, NULL, NULL);
