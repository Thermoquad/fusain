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
static uint32_t fuzz_seed;
static uint32_t initial_seed;

__attribute__((unused)) static void* fuzz_suite_setup(void)
{
  /* Use Kconfig seed if set (non-zero), otherwise random */
  if (CONFIG_FUSAIN_TEST_FUZZ_SEED != 0) {
    fuzz_seed = CONFIG_FUSAIN_TEST_FUZZ_SEED;
  } else {
    fuzz_seed = sys_rand32_get();
  }
  initial_seed = fuzz_seed;
  printk("Fuzz seed: 0x%08X (set CONFIG_FUSAIN_TEST_FUZZ_SEED to reproduce)\n",
      initial_seed);
  return NULL;
}

static uint32_t fuzz_rand(void)
{
  fuzz_seed = (fuzz_seed * 1103515245 + 12345) & 0x7FFFFFFF;
  return fuzz_seed;
}

static uint8_t fuzz_rand_byte(void)
{
  return fuzz_rand() & 0xFF;
}

/* Create a random valid packet using the create_* helpers */
static void fuzz_create_random_packet(fusain_packet_t* packet)
{
  uint64_t addr = ((uint64_t)fuzz_rand() << 32) | fuzz_rand();

  switch (fuzz_rand() % 16) {
  case 0: {
    fusain_cmd_motor_config_t cfg = {
      .motor = fuzz_rand_byte(),
      .pwm_period = fuzz_rand(),
      .pid_kp = (double)fuzz_rand() / 1000.0,
      .pid_ki = (double)fuzz_rand() / 1000.0,
      .pid_kd = (double)fuzz_rand() / 1000.0,
      .max_rpm = fuzz_rand(),
      .min_rpm = fuzz_rand(),
      .min_pwm_duty = fuzz_rand(),
    };
    fusain_create_motor_config(packet, addr, &cfg);
    break;
  }
  case 1: {
    fusain_cmd_pump_config_t cfg = {
      .pump = fuzz_rand_byte(),
      .pulse_ms = fuzz_rand(),
      .recovery_ms = fuzz_rand(),
    };
    fusain_create_pump_config(packet, addr, &cfg);
    break;
  }
  case 2: {
    fusain_cmd_temp_config_t cfg = {
      .thermometer = fuzz_rand_byte(),
      .pid_kp = (double)fuzz_rand() / 1000.0,
      .pid_ki = (double)fuzz_rand() / 1000.0,
      .pid_kd = (double)fuzz_rand() / 1000.0,
    };
    fusain_create_temp_config(packet, addr, &cfg);
    break;
  }
  case 3: {
    fusain_cmd_glow_config_t cfg = {
      .glow = fuzz_rand_byte(),
      .max_duration_ms = fuzz_rand(),
    };
    fusain_create_glow_config(packet, addr, &cfg);
    break;
  }
  case 4:
    fusain_create_state_command(packet, addr, fuzz_rand_byte(), (int32_t)fuzz_rand());
    break;
  case 5:
    fusain_create_pump_command(packet, addr, fuzz_rand_byte(), fuzz_rand());
    break;
  case 6:
    fusain_create_motor_command(packet, addr, fuzz_rand_byte(), fuzz_rand());
    break;
  case 7:
    fusain_create_ping_request(packet, addr);
    break;
  case 8:
    fusain_create_ping_response(packet, addr, fuzz_rand());
    break;
  case 9:
    fusain_create_discovery_request(packet, addr);
    break;
  case 10:
    fusain_create_device_announce(packet, addr, fuzz_rand_byte(),
        fuzz_rand_byte(), fuzz_rand_byte(), fuzz_rand_byte());
    break;
  case 11:
    /* state and error_code are constrained to .le 255 in CDDL */
    fusain_create_state_data(packet, addr, fuzz_rand() % 2,
        fuzz_rand_byte(), fuzz_rand_byte(), fuzz_rand());
    break;
  case 12:
    fusain_create_data_subscription(packet, addr, ((uint64_t)fuzz_rand() << 32) | fuzz_rand());
    break;
  case 13:
    fusain_create_data_unsubscribe(packet, addr, ((uint64_t)fuzz_rand() << 32) | fuzz_rand());
    break;
  case 14:
    fusain_create_timeout_config(packet, addr, fuzz_rand() % 2, fuzz_rand());
    break;
  case 15:
    /* Emergency stop via state_command with EMERGENCY mode */
    fusain_create_state_command(packet, addr, FUSAIN_MODE_EMERGENCY, 0);
    break;
  }
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
    for (size_t i = 0; i < sizeof(random_data); i++) {
      random_data[i] = fuzz_rand_byte();
    }

    /* Feed to decoder */
    fusain_decode_result_t result = FUSAIN_DECODE_INCOMPLETE;
    for (size_t i = 0; i < sizeof(random_data); i++) {
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

/* Fuzz round-trip with valid CBOR packets */
ZTEST(fusain_fuzz, test_fuzz_roundtrip)
{
  int success_count = 0;

  for (int round = 0; round < CONFIG_FUSAIN_TEST_FUZZ_ROUNDS; round++) {
    fusain_packet_t tx_packet;
    fuzz_create_random_packet(&tx_packet);

    /* Encode */
    uint8_t buffer[FUSAIN_MAX_PACKET_SIZE * 2];
    int encoded_len = fusain_encode_packet(&tx_packet, buffer, sizeof(buffer));

    zassert_true(encoded_len > 0, "Round %d: Encoding should succeed", round);

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
    zassert_equal(rx_packet.address, tx_packet.address,
        "Round %d: Address should match", round);
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

/* Fuzz byte stuffing edge cases - tests addresses with special bytes */
ZTEST(fusain_fuzz, test_fuzz_byte_stuffing)
{
  int stuffed_count = 0;

  for (int round = 0; round < CONFIG_FUSAIN_TEST_FUZZ_ROUNDS; round++) {
    fusain_packet_t packet;
    fuzz_create_random_packet(&packet);

    /* Force special bytes in the address to trigger byte stuffing */
    bool force_stuffing = (round % 10) == 0;
    if (force_stuffing) {
      uint8_t special[] = { FUSAIN_START_BYTE, FUSAIN_END_BYTE, FUSAIN_ESC_BYTE };
      /* Create an address with special bytes embedded */
      uint64_t addr = 0;
      for (int i = 0; i < 8; i++) {
        addr |= ((uint64_t)special[fuzz_rand() % 3]) << (i * 8);
      }
      packet.address = addr;
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
    zassert_equal(rx_packet.address, packet.address,
        "Round %d: Address should survive stuffing", round);
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
    /* Create valid CBOR packet */
    fusain_packet_t tx_packet;
    fuzz_create_random_packet(&tx_packet);

    uint8_t buffer[FUSAIN_MAX_PACKET_SIZE * 2];
    int encoded_len = fusain_encode_packet(&tx_packet, buffer, sizeof(buffer));

    zassert_true(encoded_len > 0, "Round %d: Encoding should succeed", round);

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
      encoded_len = fusain_encode_packet(&tx_packet, buffer, sizeof(buffer));

      result = FUSAIN_DECODE_INCOMPLETE;
      for (int i = 0; i < encoded_len; i++) {
        result = fusain_decode_byte(buffer[i], &rx_packet, &decoder);
      }

      zassert_equal(result, FUSAIN_DECODE_OK,
          "Round %d: Should recover after error", round);
    }
  }

  printk("Fuzz decoder errors: %d error recoveries\n", error_recovery_count);
}

/* Test suite setup - prints seed at start for reproducibility */
ZTEST_SUITE(fusain_fuzz, NULL, fuzz_suite_setup, NULL, NULL, NULL);
