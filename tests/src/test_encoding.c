/*
 * Copyright (c) 2025 Kaz Walker, Thermoquad
 * SPDX-License-Identifier: Apache-2.0
 *
 * Fusain Protocol Library - Packet Encoding Tests
 */

#include <fusain/fusain.h>
#include <string.h>
#include <zephyr/ztest.h>

/* Test basic packet encoding */
ZTEST(fusain_encoding, test_encode_basic)
{
  fusain_packet_t packet = {
    .length = 5,
    .msg_type = FUSAIN_MSG_PING_REQUEST,
  };
  memcpy(packet.payload, "HELLO", 5);

  uint8_t buffer[FUSAIN_MAX_PACKET_SIZE * 2];
  int len = fusain_encode_packet(&packet, buffer, sizeof(buffer));

  zassert_true(len > 0, "Encoding should succeed");
  zassert_equal(buffer[0], FUSAIN_START_BYTE, "First byte should be START");
  zassert_equal(buffer[len - 1], FUSAIN_END_BYTE, "Last byte should be END");
}

/* Test encoding with empty payload */
ZTEST(fusain_encoding, test_encode_empty_payload)
{
  fusain_packet_t packet = {
    .length = 0,
    .msg_type = FUSAIN_MSG_DISCOVERY_REQUEST,
  };

  uint8_t buffer[FUSAIN_MAX_PACKET_SIZE * 2];
  int len = fusain_encode_packet(&packet, buffer, sizeof(buffer));

  zassert_true(len > 0, "Empty payload should encode successfully");
  zassert_true(len >= FUSAIN_MIN_PACKET_SIZE,
      "Encoded size should be at least minimum packet size");
}

/* Test encoding with maximum payload */
ZTEST(fusain_encoding, test_encode_max_payload)
{
  fusain_packet_t packet = {
    .length = FUSAIN_MAX_PAYLOAD_SIZE,
    .msg_type = FUSAIN_MSG_STATE_DATA,
  };

  /* Fill with pattern */
  for (int i = 0; i < packet.length; i++) {
    packet.payload[i] = i & 0xFF;
  }

  uint8_t buffer[FUSAIN_MAX_PACKET_SIZE * 2];
  int len = fusain_encode_packet(&packet, buffer, sizeof(buffer));

  zassert_true(len > 0, "Max payload should encode successfully");
  zassert_equal(buffer[0], FUSAIN_START_BYTE, "Should start with START byte");
  zassert_equal(buffer[len - 1], FUSAIN_END_BYTE, "Should end with END byte");
}

/* Test encoding with byte stuffing required */
ZTEST(fusain_encoding, test_encode_byte_stuffing)
{
  fusain_packet_t packet = {
    .length = 3,
    .msg_type = FUSAIN_START_BYTE, /* Requires escaping */
  };
  packet.payload[0] = FUSAIN_START_BYTE; /* Requires escaping */
  packet.payload[1] = FUSAIN_END_BYTE; /* Requires escaping */
  packet.payload[2] = FUSAIN_ESC_BYTE; /* Requires escaping */

  uint8_t buffer[FUSAIN_MAX_PACKET_SIZE * 2];
  int len = fusain_encode_packet(&packet, buffer, sizeof(buffer));

  zassert_true(len > 0, "Encoding with stuffing should succeed");

  /* Count escape bytes in the encoded output */
  int esc_count = 0;
  for (int i = 1; i < len - 1; i++) { /* Skip START and END */
    if (buffer[i] == FUSAIN_ESC_BYTE) {
      esc_count++;
    }
  }

  zassert_true(esc_count > 0, "Should have escape bytes");
}

/* Test encoding error handling */
ZTEST(fusain_encoding, test_encode_errors)
{
  fusain_packet_t packet = {
    .length = 10,
    .msg_type = FUSAIN_MSG_STATE_COMMAND,
  };

  uint8_t buffer[10];

  /* Test NULL packet */
  int len = fusain_encode_packet(NULL, buffer, sizeof(buffer));
  zassert_true(len < 0, "NULL packet should fail");

  /* Test NULL buffer */
  len = fusain_encode_packet(&packet, NULL, 100);
  zassert_true(len < 0, "NULL buffer should fail");

  /* Test buffer too small */
  len = fusain_encode_packet(&packet, buffer, 2);
  zassert_true(len < 0, "Too small buffer should fail");

  /* Test invalid payload length */
  packet.length = FUSAIN_MAX_PAYLOAD_SIZE + 1;
  len = fusain_encode_packet(&packet, buffer, sizeof(buffer));
  zassert_true(len < 0, "Oversized payload should fail");
}

/* Test encoding determinism */
ZTEST(fusain_encoding, test_encode_determinism)
{
  fusain_packet_t packet = {
    .length = 8,
    .msg_type = FUSAIN_MSG_MOTOR_DATA,
  };
  memcpy(packet.payload, "TESTDATA", 8);

  uint8_t buffer1[FUSAIN_MAX_PACKET_SIZE * 2];
  uint8_t buffer2[FUSAIN_MAX_PACKET_SIZE * 2];

  int len1 = fusain_encode_packet(&packet, buffer1, sizeof(buffer1));
  int len2 = fusain_encode_packet(&packet, buffer2, sizeof(buffer2));

  zassert_equal(len1, len2, "Encoded length should be consistent");
  zassert_mem_equal(buffer1, buffer2, len1, "Encoded data should be identical");
}

/* Test suite setup */
ZTEST_SUITE(fusain_encoding, NULL, NULL, NULL, NULL, NULL);
