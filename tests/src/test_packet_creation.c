/*
 * Copyright (c) 2025 Kaz Walker, Thermoquad
 * SPDX-License-Identifier: Apache-2.0
 *
 * Fusain Protocol Library - Packet Creation Tests
 */

#include <fusain/fusain.h>
#include <string.h>
#include <zephyr/ztest.h>

/* Test v1.x command packet creation */
ZTEST(fusain_packets, test_create_set_mode)
{
  fusain_packet_t packet;
  fusain_create_set_mode(&packet, 0, FUSAIN_MODE_HEAT, 2500);

  zassert_equal(packet.msg_type, FUSAIN_MSG_STATE_COMMAND, "Type should match");
  zassert_equal(packet.length, sizeof(fusain_cmd_set_mode_t),
      "Length should match");

  fusain_cmd_set_mode_t* cmd = (fusain_cmd_set_mode_t*)packet.payload;
  zassert_equal(cmd->mode, FUSAIN_MODE_HEAT, "Mode should match");
  zassert_equal(cmd->parameter, 2500, "Parameter should match");
}

ZTEST(fusain_packets, test_create_set_pump_rate)
{
  fusain_packet_t packet;
  fusain_create_set_pump_rate(&packet, 0, 250);

  zassert_equal(packet.msg_type, FUSAIN_MSG_PUMP_COMMAND, "Type should match");
  zassert_equal(packet.length, sizeof(fusain_cmd_set_pump_rate_t),
      "Length should match");

  fusain_cmd_set_pump_rate_t* cmd = (fusain_cmd_set_pump_rate_t*)packet.payload;
  zassert_equal(cmd->rate_ms, 250, "Rate should match");
}

ZTEST(fusain_packets, test_create_set_target_rpm)
{
  fusain_packet_t packet;
  fusain_create_set_target_rpm(&packet, 0, 3000);

  zassert_equal(packet.msg_type, FUSAIN_MSG_MOTOR_COMMAND, "Type should match");
  zassert_equal(packet.length, sizeof(fusain_cmd_set_target_rpm_t),
      "Length should match");

  fusain_cmd_set_target_rpm_t* cmd = (fusain_cmd_set_target_rpm_t*)packet.payload;
  zassert_equal(cmd->target_rpm, 3000, "RPM should match");
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
  fusain_create_telemetry_config(&packet, 0, true, 100, 0);

  zassert_equal(packet.msg_type, FUSAIN_MSG_TELEMETRY_CONFIG,
      "Type should match");
  zassert_equal(packet.length, sizeof(fusain_cmd_telemetry_config_t),
      "Length should match");

  fusain_cmd_telemetry_config_t* cmd = (fusain_cmd_telemetry_config_t*)packet.payload;
  zassert_equal(cmd->telemetry_enabled, 1, "Enabled should match");
  zassert_equal(cmd->interval_ms, 100, "Interval should match");
  zassert_equal(cmd->telemetry_mode, 0, "Mode should match");
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
    .min_rate_ms = 100,
    .max_rate_ms = 1000,
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
  fusain_create_data_subscription(&packet, 0, 0x123456789ABCDEF0ULL, 0xFFULL);

  zassert_equal(packet.msg_type, FUSAIN_MSG_DATA_SUBSCRIPTION,
      "Type should match");
  zassert_equal(packet.length, sizeof(fusain_cmd_data_subscription_t),
      "Length should match");

  fusain_cmd_data_subscription_t* cmd = (fusain_cmd_data_subscription_t*)packet.payload;
  zassert_equal(cmd->appliance_address, 0x123456789ABCDEF0ULL,
      "Address should match");
  zassert_equal(cmd->message_filter, 0xFFULL, "Filter should match");
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
  fusain_create_state_data(&packet, 0, FUSAIN_STATE_HEATING, FUSAIN_ERROR_NONE);

  zassert_equal(packet.msg_type, FUSAIN_MSG_STATE_DATA, "Type should match");
  zassert_equal(packet.length, sizeof(fusain_data_state_t),
      "Length should match");

  fusain_data_state_t* data = (fusain_data_state_t*)packet.payload;
  zassert_equal(data->state, FUSAIN_STATE_HEATING, "State should match");
  zassert_equal(data->error, FUSAIN_ERROR_NONE, "Error should match");
}

ZTEST(fusain_packets, test_create_ping_response)
{
  fusain_packet_t packet;
  fusain_create_ping_response(&packet, 0, 123456789ULL);

  zassert_equal(packet.msg_type, FUSAIN_MSG_PING_RESPONSE, "Type should match");
  zassert_equal(packet.length, sizeof(fusain_data_ping_response_t),
      "Length should match");

  fusain_data_ping_response_t* data = (fusain_data_ping_response_t*)packet.payload;
  zassert_equal(data->uptime_ms, 123456789ULL, "Uptime should match");
}

ZTEST(fusain_packets, test_create_device_announce)
{
  fusain_packet_t packet;
  fusain_create_device_announce(&packet, 0, 0x01, 0xFF);

  zassert_equal(packet.msg_type, FUSAIN_MSG_DEVICE_ANNOUNCE,
      "Type should match");
  zassert_equal(packet.length, sizeof(fusain_data_device_announce_t),
      "Length should match");

  fusain_data_device_announce_t* data = (fusain_data_device_announce_t*)packet.payload;
  zassert_equal(data->device_type, 0x01, "Device type should match");
  zassert_equal(data->capabilities, 0xFF, "Capabilities should match");
}

ZTEST(fusain_packets, test_create_telemetry_bundle)
{
  fusain_telemetry_motor_t motors[2] = {
    { .rpm = 2500, .target_rpm = 2500, .pwm_duty = 75, .pwm_period = 1000 },
    { .rpm = 1200, .target_rpm = 1200, .pwm_duty = 40, .pwm_period = 1000 },
  };

  fusain_telemetry_temperature_t temps[1] = {
    { .temperature = 230.5 },
  };

  fusain_packet_t packet;
  int ret = fusain_create_telemetry_bundle(&packet, 0, FUSAIN_STATE_HEATING,
      FUSAIN_ERROR_NONE, motors, 2, temps, 1);

  zassert_equal(ret, 0, "Bundle creation should succeed");
  zassert_equal(packet.msg_type, FUSAIN_MSG_TELEMETRY_BUNDLE,
      "Type should match");

  fusain_data_telemetry_bundle_t* bundle = (fusain_data_telemetry_bundle_t*)packet.payload;
  zassert_equal(bundle->motor_count, 2, "Motor count should match");
  zassert_equal(bundle->temp_count, 1, "Temp count should match");
}

/* Test packet creation round-trip for different message types */
ZTEST(fusain_packets, test_packet_roundtrip)
{
  struct test_case {
    const char* name;
    fusain_packet_t (*create)(void);
  };

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
      fusain_create_state_data(&tx_packet, 0, FUSAIN_STATE_HEATING,
          FUSAIN_ERROR_NONE);
      break;
    case 3:
      fusain_create_discovery_request(&tx_packet, 0);
      break;
    case 4:
      fusain_create_device_announce(&tx_packet, 0, 1, 0xFF);
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

/* Test suite setup */
ZTEST_SUITE(fusain_packets, NULL, NULL, NULL, NULL, NULL);
