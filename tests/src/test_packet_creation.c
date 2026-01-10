/*
 * Copyright (c) 2025 Kaz Walker, Thermoquad
 * SPDX-License-Identifier: Apache-2.0
 *
 * Fusain Protocol Library - Packet Creation Tests
 */

#include <fusain/fusain.h>
#include <string.h>
#include <zephyr/ztest.h>

/* Test control command packet creation */
ZTEST(fusain_packets, test_create_set_mode)
{
  fusain_packet_t packet;
  fusain_create_set_mode(&packet, 0, FUSAIN_MODE_HEAT, 2500);

  zassert_equal(packet.msg_type, FUSAIN_MSG_STATE_COMMAND, "Type should match");
  zassert_equal(packet.length, sizeof(fusain_cmd_set_mode_t),
      "Length should match");
  zassert_equal(packet.length, 8, "STATE_COMMAND should be 8 bytes");

  fusain_cmd_set_mode_t* cmd = (fusain_cmd_set_mode_t*)packet.payload;
  zassert_equal(cmd->mode, FUSAIN_MODE_HEAT, "Mode should match");
  zassert_equal(cmd->argument, 2500, "Argument should match");
}

ZTEST(fusain_packets, test_create_pump_command)
{
  fusain_packet_t packet;
  fusain_create_pump_command(&packet, 0, 0, 250);

  zassert_equal(packet.msg_type, FUSAIN_MSG_PUMP_COMMAND, "Type should match");
  zassert_equal(packet.length, sizeof(fusain_cmd_set_pump_rate_t),
      "Length should match");
  zassert_equal(packet.length, 8, "PUMP_COMMAND should be 8 bytes");

  fusain_cmd_set_pump_rate_t* cmd = (fusain_cmd_set_pump_rate_t*)packet.payload;
  zassert_equal(cmd->pump, 0, "Pump index should match");
  zassert_equal(cmd->rate_ms, 250, "Rate should match");
}

ZTEST(fusain_packets, test_create_motor_command)
{
  fusain_packet_t packet;
  fusain_create_motor_command(&packet, 0, 0, 3000);

  zassert_equal(packet.msg_type, FUSAIN_MSG_MOTOR_COMMAND, "Type should match");
  zassert_equal(packet.length, sizeof(fusain_cmd_set_target_rpm_t),
      "Length should match");
  zassert_equal(packet.length, 8, "MOTOR_COMMAND should be 8 bytes");

  fusain_cmd_set_target_rpm_t* cmd = (fusain_cmd_set_target_rpm_t*)packet.payload;
  zassert_equal(cmd->motor, 0, "Motor index should match");
  zassert_equal(cmd->rpm, 3000, "RPM should match");
}

ZTEST(fusain_packets, test_create_glow_command)
{
  fusain_packet_t packet;
  fusain_create_glow_command(&packet, 0, 0, 30000);

  zassert_equal(packet.msg_type, FUSAIN_MSG_GLOW_COMMAND, "Type should match");
  zassert_equal(packet.length, sizeof(fusain_cmd_glow_t), "Length should match");

  fusain_cmd_glow_t* cmd = (fusain_cmd_glow_t*)packet.payload;
  zassert_equal(cmd->glow, 0, "Glow index should match");
  zassert_equal(cmd->duration, 30000, "Duration should match");
}

ZTEST(fusain_packets, test_create_ping_request)
{
  fusain_packet_t packet;
  fusain_create_ping_request(&packet, 0);

  zassert_equal(packet.msg_type, FUSAIN_MSG_PING_REQUEST, "Type should match");
  zassert_equal(packet.length, 0, "Ping should have no payload");
}

ZTEST(fusain_packets, test_create_telemetry_config)
{
  fusain_packet_t packet;
  fusain_create_telemetry_config(&packet, 0, true, 100);

  zassert_equal(packet.msg_type, FUSAIN_MSG_TELEMETRY_CONFIG,
      "Type should match");
  zassert_equal(packet.length, sizeof(fusain_cmd_telemetry_config_t),
      "Length should match");
  zassert_equal(packet.length, 8, "TELEMETRY_CONFIG should be 8 bytes");

  fusain_cmd_telemetry_config_t* cmd = (fusain_cmd_telemetry_config_t*)packet.payload;
  zassert_equal(cmd->telemetry_enabled, 1, "Enabled should match");
  zassert_equal(cmd->interval_ms, 100, "Interval should match");
}

ZTEST(fusain_packets, test_create_timeout_config)
{
  fusain_packet_t packet;
  fusain_create_timeout_config(&packet, 0, true, 30000);

  zassert_equal(packet.msg_type, FUSAIN_MSG_TIMEOUT_CONFIG,
      "Type should match");
  zassert_equal(packet.length, sizeof(fusain_cmd_timeout_config_t),
      "Length should match");
  zassert_equal(packet.length, 8, "TIMEOUT_CONFIG should be 8 bytes");

  fusain_cmd_timeout_config_t* cmd = (fusain_cmd_timeout_config_t*)packet.payload;
  zassert_equal(cmd->enabled, 1, "Enabled should match");
  zassert_equal(cmd->timeout_ms, 30000, "Timeout should match");
}

ZTEST(fusain_packets, test_create_send_telemetry)
{
  fusain_packet_t packet;
  fusain_create_send_telemetry(&packet, 0, 1, 0xFFFFFFFF);

  zassert_equal(packet.msg_type, FUSAIN_MSG_SEND_TELEMETRY,
      "Type should match");
  zassert_equal(packet.length, sizeof(fusain_cmd_send_telemetry_t),
      "Length should match");
  zassert_equal(packet.length, 8, "SEND_TELEMETRY should be 8 bytes");

  fusain_cmd_send_telemetry_t* cmd = (fusain_cmd_send_telemetry_t*)packet.payload;
  zassert_equal(cmd->telemetry_type, 1, "Type should match");
  zassert_equal(cmd->index, 0xFFFFFFFF, "Index should match");
}

/* Test v2.0 config command packet creation */
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
  fusain_create_motor_config(&packet, 0, &config);

  zassert_equal(packet.msg_type, FUSAIN_MSG_MOTOR_CONFIG, "Type should match");
  zassert_equal(packet.length, sizeof(fusain_cmd_motor_config_t),
      "Length should match");

  fusain_cmd_motor_config_t* cmd = (fusain_cmd_motor_config_t*)packet.payload;
  zassert_equal(cmd->motor, 0, "Motor index should match");
  zassert_equal(cmd->max_rpm, 3400, "Max RPM should match");
}

ZTEST(fusain_packets, test_create_pump_config)
{
  fusain_cmd_pump_config_t config = {
    .pump = 0,
    .pulse_ms = 100,
    .recovery_ms = 1000,
  };

  fusain_packet_t packet;
  fusain_create_pump_config(&packet, 0, &config);

  zassert_equal(packet.msg_type, FUSAIN_MSG_PUMP_CONFIG, "Type should match");
  zassert_equal(packet.length, sizeof(fusain_cmd_pump_config_t),
      "Length should match");
}

ZTEST(fusain_packets, test_create_temp_config)
{
  fusain_cmd_temp_config_t config = {
    .thermometer = 0,
    .pid_kp = 100.0,
    .pid_ki = 10.0,
    .pid_kd = 5.0,
    .sample_count = 60,
    .read_rate = 100,
  };

  fusain_packet_t packet;
  fusain_create_temp_config(&packet, 0, &config);

  zassert_equal(packet.msg_type, FUSAIN_MSG_TEMP_CONFIG, "Type should match");
  zassert_equal(packet.length, sizeof(fusain_cmd_temp_config_t),
      "Length should match");
}

ZTEST(fusain_packets, test_create_glow_config)
{
  fusain_cmd_glow_config_t config = {
    .glow = 0,
    .max_duration_ms = 300000,
  };

  fusain_packet_t packet;
  fusain_create_glow_config(&packet, 0, &config);

  zassert_equal(packet.msg_type, FUSAIN_MSG_GLOW_CONFIG, "Type should match");
  zassert_equal(packet.length, sizeof(fusain_cmd_glow_config_t),
      "Length should match");
}

ZTEST(fusain_packets, test_create_data_subscription)
{
  fusain_packet_t packet;
  fusain_create_data_subscription(&packet, 0, 0x123456789ABCDEF0ULL);

  zassert_equal(packet.msg_type, FUSAIN_MSG_DATA_SUBSCRIPTION,
      "Type should match");
  zassert_equal(packet.length, sizeof(fusain_cmd_data_subscription_t),
      "Length should match");
  zassert_equal(packet.length, 8, "DATA_SUBSCRIPTION should be 8 bytes");

  fusain_cmd_data_subscription_t* cmd = (fusain_cmd_data_subscription_t*)packet.payload;
  zassert_equal(cmd->appliance_address, 0x123456789ABCDEF0ULL,
      "Address should match");
}

ZTEST(fusain_packets, test_create_data_unsubscribe)
{
  fusain_packet_t packet;
  fusain_create_data_unsubscribe(&packet, 0, 0x123456789ABCDEF0ULL);

  zassert_equal(packet.msg_type, FUSAIN_MSG_DATA_UNSUBSCRIBE,
      "Type should match");
  zassert_equal(packet.length, sizeof(fusain_cmd_data_unsubscribe_t),
      "Length should match");
}

ZTEST(fusain_packets, test_create_discovery_request)
{
  fusain_packet_t packet;
  fusain_create_discovery_request(&packet, 0);

  zassert_equal(packet.msg_type, FUSAIN_MSG_DISCOVERY_REQUEST,
      "Type should match");
  zassert_equal(packet.length, 0, "Discovery should have no payload");
}

/* Test data packet creation */
ZTEST(fusain_packets, test_create_state_data)
{
  fusain_packet_t packet;
  fusain_create_state_data(&packet, 0, 1, 0x42, FUSAIN_STATE_HEATING, 12345);

  zassert_equal(packet.msg_type, FUSAIN_MSG_STATE_DATA, "Type should match");
  zassert_equal(packet.length, sizeof(fusain_data_state_t),
      "Length should match");
  zassert_equal(packet.length, 16, "STATE_DATA should be 16 bytes");

  fusain_data_state_t* data = (fusain_data_state_t*)packet.payload;
  zassert_equal(data->error, 1, "Error flag should match");
  zassert_equal(data->code, 0x42, "Error code should match");
  zassert_equal(data->state, FUSAIN_STATE_HEATING, "State should match");
  zassert_equal(data->timestamp, 12345, "Timestamp should match");
}

ZTEST(fusain_packets, test_create_ping_response)
{
  fusain_packet_t packet;
  fusain_create_ping_response(&packet, 0, 123456789);

  zassert_equal(packet.msg_type, FUSAIN_MSG_PING_RESPONSE, "Type should match");
  zassert_equal(packet.length, sizeof(fusain_data_ping_response_t),
      "Length should match");
  zassert_equal(packet.length, 4, "PING_RESPONSE should be 4 bytes");

  fusain_data_ping_response_t* data = (fusain_data_ping_response_t*)packet.payload;
  zassert_equal(data->uptime_ms, 123456789, "Uptime should match");
}

ZTEST(fusain_packets, test_create_device_announce)
{
  fusain_packet_t packet;
  fusain_create_device_announce(&packet, 0, 1, 1, 1, 1);

  zassert_equal(packet.msg_type, FUSAIN_MSG_DEVICE_ANNOUNCE,
      "Type should match");
  zassert_equal(packet.length, sizeof(fusain_data_device_announce_t),
      "Length should match");
  zassert_equal(packet.length, 8, "DEVICE_ANNOUNCE should be 8 bytes");

  fusain_data_device_announce_t* data = (fusain_data_device_announce_t*)packet.payload;
  zassert_equal(data->motor_count, 1, "Motor count should match");
  zassert_equal(data->thermometer_count, 1, "Thermometer count should match");
  zassert_equal(data->pump_count, 1, "Pump count should match");
  zassert_equal(data->glow_count, 1, "Glow count should match");
}

/* Test packet creation round-trip for different message types */
ZTEST(fusain_packets, test_packet_roundtrip)
{
  /* Helper to create packets - using inline approach since we can't return stack vars */
  fusain_packet_t tx_packet;
  const char* test_names[] = { "ping_request", "set_mode", "state_data",
    "discovery_request", "device_announce" };

  for (int test_idx = 0; test_idx < 5; test_idx++) {
    /* Create different packet types */
    switch (test_idx) {
    case 0:
      fusain_create_ping_request(&tx_packet, 0);
      break;
    case 1:
      fusain_create_set_mode(&tx_packet, 0, FUSAIN_MODE_HEAT, 2500);
      break;
    case 2:
      fusain_create_state_data(&tx_packet, 0, 0, 0, FUSAIN_STATE_HEATING, 12345);
      break;
    case 3:
      fusain_create_discovery_request(&tx_packet, 0);
      break;
    case 4:
      fusain_create_device_announce(&tx_packet, 0, 1, 1, 1, 1);
      break;
    }

    /* Encode and decode */
    uint8_t buffer[FUSAIN_MAX_PACKET_SIZE * 2];
    int len = fusain_encode_packet(&tx_packet, buffer, sizeof(buffer));
    zassert_true(len > 0, "%s: Encoding should succeed", test_names[test_idx]);

    fusain_decoder_t decoder;
    fusain_reset_decoder(&decoder);

    fusain_packet_t rx_packet;
    fusain_decode_result_t result = FUSAIN_DECODE_INCOMPLETE;

    for (int i = 0; i < len; i++) {
      result = fusain_decode_byte(buffer[i], &rx_packet, &decoder);
    }

    zassert_equal(result, FUSAIN_DECODE_OK, "%s: Decoding should succeed",
        test_names[test_idx]);
    zassert_equal(rx_packet.msg_type, tx_packet.msg_type, "%s: Type should match",
        test_names[test_idx]);
  }
}

/* Test TEMP_COMMAND packet creation */
ZTEST(fusain_packets, test_create_temp_command)
{
  fusain_packet_t packet;
  fusain_create_temp_command(&packet, 0, 0, FUSAIN_TEMP_CMD_SET_TARGET_TEMP, 0, 220.5);

  zassert_equal(packet.msg_type, FUSAIN_MSG_TEMP_COMMAND, "Type should match");
  zassert_equal(packet.length, sizeof(fusain_cmd_temp_command_t),
      "Length should match");
  zassert_equal(packet.length, 20, "TEMP_COMMAND should be 20 bytes");

  fusain_cmd_temp_command_t* cmd = (fusain_cmd_temp_command_t*)packet.payload;
  zassert_equal(cmd->thermometer, 0, "Thermometer index should match");
  zassert_equal(cmd->type, FUSAIN_TEMP_CMD_SET_TARGET_TEMP, "Type should match");
  zassert_equal(cmd->motor_index, 0, "Motor index should match");
  zassert_true(cmd->target_temp > 220.0 && cmd->target_temp < 221.0,
      "Target temp should match");
}

/* Test suite setup */
ZTEST_SUITE(fusain_packets, NULL, NULL, NULL, NULL, NULL);
