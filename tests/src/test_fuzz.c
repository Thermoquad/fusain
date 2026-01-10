/*
 * Copyright (c) 2025 Kaz Walker, Thermoquad
 * SPDX-License-Identifier: Apache-2.0
 *
 * Fusain Protocol Library - Fuzz Testing
 */

#include <fusain/fusain.h>
#include <string.h>
#include <zephyr/random/random.h>
#include <zephyr/ztest.h>

/* Simple LCG random number generator for reproducibility */
static uint32_t fuzz_seed = 0x12345678;

static uint32_t fuzz_rand(void)
{
  fuzz_seed = (fuzz_seed * 1103515245 + 12345) & 0x7FFFFFFF;
  return fuzz_seed;
}

static uint8_t fuzz_rand_byte(void)
{
  return fuzz_rand() & 0xFF;
}

/* Fuzz encoding with random payloads */
ZTEST(fusain_fuzz, test_fuzz_encoding)
{
  int success_count = 0;
  int expected_fail_count = 0;

  for (int round = 0; round < CONFIG_FUSAIN_TEST_FUZZ_ROUNDS; round++) {
    fusain_packet_t packet;

    /* Random length (including invalid values to test error handling) */
    int length = fuzz_rand() % (FUSAIN_MAX_PAYLOAD_SIZE + 50);
    packet.length = length;

    /* Random message type */
    packet.msg_type = fuzz_rand_byte();

    /* Fill payload with random data */
    for (int i = 0; i < FUSAIN_MAX_PAYLOAD_SIZE; i++) {
      packet.payload[i] = fuzz_rand_byte();
    }

    uint8_t buffer[FUSAIN_MAX_PACKET_SIZE * 2];
    int encoded_len = fusain_encode_packet(&packet, buffer, sizeof(buffer));

    if (length <= FUSAIN_MAX_PAYLOAD_SIZE) {
      /* Should succeed for valid lengths */
      zassert_true(encoded_len > 0,
          "Round %d: Valid packet should encode (len=%d)",
          round, length);
      success_count++;

      /* Verify framing */
      zassert_equal(buffer[0], FUSAIN_START_BYTE,
          "Round %d: Should start with START byte", round);
      zassert_equal(buffer[encoded_len - 1], FUSAIN_END_BYTE,
          "Round %d: Should end with END byte", round);
    } else {
      /* Should fail for invalid lengths */
      zassert_true(encoded_len < 0,
          "Round %d: Invalid packet should fail (len=%d)",
          round, length);
      expected_fail_count++;
    }
  }

  printk("Fuzz encoding: %d successes, %d expected failures\n", success_count,
      expected_fail_count);
}

/* Fuzz decoding with random byte streams */
ZTEST(fusain_fuzz, test_fuzz_decoding_random)
{
  int detected_errors = 0;

  for (int round = 0; round < CONFIG_FUSAIN_TEST_FUZZ_ROUNDS; round++) {
    fusain_decoder_t decoder;
    fusain_reset_decoder(&decoder);

    fusain_packet_t packet;
    uint8_t random_data[100];

    /* Generate random byte stream */
    for (int i = 0; i < sizeof(random_data); i++) {
      random_data[i] = fuzz_rand_byte();
    }

    /* Feed to decoder */
    fusain_decode_result_t result = FUSAIN_DECODE_INCOMPLETE;
    for (int i = 0; i < sizeof(random_data); i++) {
      result = fusain_decode_byte(random_data[i], &packet, &decoder);

      /* Decoder should never crash, only return valid states */
      zassert_true(result >= FUSAIN_DECODE_OK && result <= FUSAIN_DECODE_BUFFER_OVERFLOW,
          "Round %d: Decoder should return valid result",
          round);

      if (result != FUSAIN_DECODE_INCOMPLETE && result != FUSAIN_DECODE_OK) {
        detected_errors++;
        break;
      }
    }
  }

  printk("Fuzz decoding: Detected %d errors in random data\n", detected_errors);
}

/* Fuzz round-trip with valid packets but random data */
ZTEST(fusain_fuzz, test_fuzz_roundtrip)
{
  int success_count = 0;

  for (int round = 0; round < CONFIG_FUSAIN_TEST_FUZZ_ROUNDS; round++) {
    fusain_packet_t tx_packet;

    /* Random valid length */
    tx_packet.length = fuzz_rand() % (FUSAIN_MAX_PAYLOAD_SIZE + 1);

    /* Random message type */
    tx_packet.msg_type = fuzz_rand_byte();

    /* Random payload */
    for (int i = 0; i < tx_packet.length; i++) {
      tx_packet.payload[i] = fuzz_rand_byte();
    }

    /* Encode */
    uint8_t buffer[FUSAIN_MAX_PACKET_SIZE * 2];
    int encoded_len = fusain_encode_packet(&tx_packet, buffer, sizeof(buffer));

    if (encoded_len < 0) {
      continue; /* Skip invalid packets */
    }

    /* Decode */
    fusain_decoder_t decoder;
    fusain_reset_decoder(&decoder);

    fusain_packet_t rx_packet;
    fusain_decode_result_t result = FUSAIN_DECODE_INCOMPLETE;

    for (int i = 0; i < encoded_len; i++) {
      result = fusain_decode_byte(buffer[i], &rx_packet, &decoder);
    }

    /* Should decode successfully */
    zassert_equal(result, FUSAIN_DECODE_OK,
        "Round %d: Decoding should succeed", round);

    /* Verify data integrity */
    zassert_equal(rx_packet.length, tx_packet.length,
        "Round %d: Length should match", round);
    zassert_equal(rx_packet.msg_type, tx_packet.msg_type,
        "Round %d: Type should match", round);
    zassert_mem_equal(rx_packet.payload, tx_packet.payload, tx_packet.length,
        "Round %d: Payload should match", round);

    success_count++;
  }

  printk("Fuzz round-trip: %d successful round-trips\n", success_count);
}

/* Fuzz CRC with edge cases */
ZTEST(fusain_fuzz, test_fuzz_crc)
{
  for (int round = 0; round < CONFIG_FUSAIN_TEST_FUZZ_ROUNDS; round++) {
    uint8_t data[FUSAIN_MAX_PAYLOAD_SIZE];
    int length = fuzz_rand() % (FUSAIN_MAX_PAYLOAD_SIZE + 1);

    /* Fill with random data */
    for (int i = 0; i < length; i++) {
      data[i] = fuzz_rand_byte();
    }

    /* Calculate CRC twice - should be deterministic */
    uint16_t crc1 = fusain_crc16(data, length);
    uint16_t crc2 = fusain_crc16(data, length);

    zassert_equal(crc1, crc2, "Round %d: CRC should be deterministic",
        round);

    /* Flip a bit and verify CRC changes (if length > 0) */
    if (length > 0) {
      data[0] ^= 0x01;
      uint16_t crc3 = fusain_crc16(data, length);
      zassert_not_equal(crc1, crc3,
          "Round %d: CRC should change with data", round);
    }
  }
}

/* Fuzz byte stuffing edge cases */
ZTEST(fusain_fuzz, test_fuzz_byte_stuffing)
{
  int stuffed_count = 0;

  for (int round = 0; round < CONFIG_FUSAIN_TEST_FUZZ_ROUNDS; round++) {
    fusain_packet_t packet;
    packet.length = (fuzz_rand() % FUSAIN_MAX_PAYLOAD_SIZE) + 1;
    packet.msg_type = fuzz_rand_byte();

    /* Occasionally inject special bytes to force stuffing */
    bool force_stuffing = (round % 10) == 0;
    for (int i = 0; i < packet.length; i++) {
      if (force_stuffing && (i % 3) == 0) {
        /* Force special bytes */
        uint8_t special[] = { FUSAIN_START_BYTE,
          FUSAIN_END_BYTE, FUSAIN_ESC_BYTE };
        packet.payload[i] = special[fuzz_rand() % 3];
      } else {
        packet.payload[i] = fuzz_rand_byte();
      }
    }

    /* Encode and decode */
    uint8_t buffer[FUSAIN_MAX_PACKET_SIZE * 2];
    int encoded_len = fusain_encode_packet(&packet, buffer, sizeof(buffer));

    zassert_true(encoded_len > 0, "Round %d: Encoding should succeed", round);

    fusain_decoder_t decoder;
    fusain_reset_decoder(&decoder);

    fusain_packet_t rx_packet;
    fusain_decode_result_t result = FUSAIN_DECODE_INCOMPLETE;

    for (int i = 0; i < encoded_len; i++) {
      result = fusain_decode_byte(buffer[i], &rx_packet, &decoder);
    }

    zassert_equal(result, FUSAIN_DECODE_OK,
        "Round %d: Decoding should succeed", round);
    zassert_mem_equal(rx_packet.payload, packet.payload, packet.length,
        "Round %d: Payload should survive stuffing", round);

    if (force_stuffing) {
      stuffed_count++;
    }
  }

  printk("Fuzz byte stuffing: Tested %d packets with forced stuffing\n",
      stuffed_count);
}

/* Fuzz decoder state machine with injected errors */
ZTEST(fusain_fuzz, test_fuzz_decoder_errors)
{
  int error_recovery_count = 0;

  for (int round = 0; round < CONFIG_FUSAIN_TEST_FUZZ_ROUNDS; round++) {
    /* Create valid packet */
    fusain_packet_t tx_packet;
    tx_packet.length = (fuzz_rand() % 20) + 1; /* Smaller for faster test */
    tx_packet.msg_type = fuzz_rand_byte();

    for (int i = 0; i < tx_packet.length; i++) {
      tx_packet.payload[i] = fuzz_rand_byte();
    }

    uint8_t buffer[FUSAIN_MAX_PACKET_SIZE * 2];
    int encoded_len = fusain_encode_packet(&tx_packet, buffer, sizeof(buffer));

    if (encoded_len < 0) {
      continue;
    }

    /* Inject random corruption */
    if (encoded_len > 5) {
      int corrupt_idx = 1 + (fuzz_rand() % (encoded_len - 2));
      buffer[corrupt_idx] ^= 0xFF;
    }

    /* Decode corrupted packet */
    fusain_decoder_t decoder;
    fusain_reset_decoder(&decoder);

    fusain_packet_t rx_packet;
    fusain_decode_result_t result = FUSAIN_DECODE_INCOMPLETE;

    for (int i = 0; i < encoded_len; i++) {
      result = fusain_decode_byte(buffer[i], &rx_packet, &decoder);
      if (result != FUSAIN_DECODE_INCOMPLETE) {
        break;
      }
    }

    /* Should detect error (not crash) */
    if (result != FUSAIN_DECODE_OK) {
      error_recovery_count++;

      /* Reset and try valid packet */
      fusain_reset_decoder(&decoder);
      encoded_len = fusain_encode_packet(&tx_packet, buffer,
          sizeof(buffer));

      result = FUSAIN_DECODE_INCOMPLETE;
      for (int i = 0; i < encoded_len; i++) {
        result = fusain_decode_byte(buffer[i], &rx_packet,
            &decoder);
      }

      zassert_equal(result, FUSAIN_DECODE_OK,
          "Round %d: Should recover after error", round);
    }
  }

  printk("Fuzz decoder errors: %d error recoveries\n", error_recovery_count);
}

/* Fuzz v2.0 packet creation functions */
ZTEST(fusain_fuzz, test_fuzz_v2_packets)
{
  for (int round = 0; round < CONFIG_FUSAIN_TEST_FUZZ_ROUNDS; round++) {
    fusain_packet_t packet;

    /* Test different v2.0 packet types */
    switch (round % 8) {
    case 0: {
      fusain_cmd_motor_config_t config = {
        .motor = fuzz_rand(),
        .pwm_period = fuzz_rand(),
        .pid_kp = (double)fuzz_rand() / 1000.0,
        .pid_ki = (double)fuzz_rand() / 1000.0,
        .pid_kd = (double)fuzz_rand() / 1000.0,
        .max_rpm = fuzz_rand(),
        .min_rpm = fuzz_rand(),
        .min_pwm_duty = fuzz_rand(),
      };
      fusain_create_motor_config(&packet, 0, &config);
      break;
    }
    case 1: {
      fusain_cmd_pump_config_t config = {
        .pump = fuzz_rand(),
        .pulse_ms = fuzz_rand(),
        .recovery_ms = fuzz_rand(),
      };
      fusain_create_pump_config(&packet, 0, &config);
      break;
    }
    case 2: {
      fusain_cmd_temp_config_t config = {
        .thermometer = fuzz_rand(),
        .pid_kp = (double)fuzz_rand() / 1000.0,
        .pid_ki = (double)fuzz_rand() / 1000.0,
        .pid_kd = (double)fuzz_rand() / 1000.0,
        .sample_count = fuzz_rand(),
        .read_rate = fuzz_rand(),
      };
      fusain_create_temp_config(&packet, 0, &config);
      break;
    }
    case 3: {
      fusain_cmd_glow_config_t config = {
        .glow = fuzz_rand(),
        .max_duration_ms = fuzz_rand(),
      };
      fusain_create_glow_config(&packet, 0, &config);
      break;
    }
    case 4:
      fusain_create_data_subscription(
          &packet, 0, (uint64_t)fuzz_rand() << 32 | fuzz_rand());
      break;
    case 5:
      fusain_create_data_unsubscribe(
          &packet, 0, (uint64_t)fuzz_rand() << 32 | fuzz_rand());
      break;
    case 6:
      fusain_create_discovery_request(&packet, 0);
      break;
    case 7:
      fusain_create_device_announce(&packet, 0, fuzz_rand_byte(),
          fuzz_rand_byte(), fuzz_rand_byte(), fuzz_rand_byte());
      break;
    }

    /* Encode and decode */
    uint8_t buffer[FUSAIN_MAX_PACKET_SIZE * 2];
    int len = fusain_encode_packet(&packet, buffer, sizeof(buffer));

    zassert_true(len > 0, "Round %d: v2.0 packet should encode", round);

    fusain_decoder_t decoder;
    fusain_reset_decoder(&decoder);

    fusain_packet_t rx_packet;
    fusain_decode_result_t result = FUSAIN_DECODE_INCOMPLETE;

    for (int i = 0; i < len; i++) {
      result = fusain_decode_byte(buffer[i], &rx_packet, &decoder);
    }

    zassert_equal(result, FUSAIN_DECODE_OK,
        "Round %d: v2.0 packet should decode", round);
  }
}

/* Test suite setup */
ZTEST_SUITE(fusain_fuzz, NULL, NULL, NULL, NULL, NULL);
