/*
 * Copyright (c) 2025 Kaz Walker, Thermoquad
 * SPDX-License-Identifier: Apache-2.0
 *
 * Fusain Protocol Library - Packet Encoding Tests
 *
 * Wire format: [START][LENGTH][ADDRESS(8)][CBOR_PAYLOAD][CRC(2)][END]
 * CBOR payload contains [msg_type, payload_map]
 */

#include <fusain/fusain.h>
#include <string.h>
#include <zephyr/ztest.h>

/* Test basic packet encoding */
ZTEST(fusain_encoding, test_encode_basic)
{
  fusain_packet_t packet = {
    .length = 5,
    .msg_type = 0x30, /* For routing purposes only, msg_type is in CBOR */
  };
  memcpy(packet.payload, "HELLO", 5);

  uint8_t buffer[FUSAIN_MAX_PACKET_SIZE * 2];
  int len = fusain_encode_packet(&packet, buffer, sizeof(buffer));

  zassert_true(len > 0, "Encoding should succeed");
  zassert_equal(buffer[0], FUSAIN_START_BYTE, "First byte should be START");
  zassert_equal(buffer[len - 1], FUSAIN_END_BYTE, "Last byte should be END");
}

/* Test encoding with minimal CBOR payload */
ZTEST(fusain_encoding, test_encode_empty_payload)
{
  /* In CBOR mode, even "empty" messages have at least [type, nil] = 3-4 bytes
   * Create a packet using create_ping_request which has nil payload */
  fusain_packet_t packet;
  fusain_create_ping_request(&packet, 0);

  uint8_t buffer[FUSAIN_MAX_PACKET_SIZE * 2];
  int len = fusain_encode_packet(&packet, buffer, sizeof(buffer));

  zassert_true(len > 0, "Minimal CBOR payload should encode successfully");
  /* Minimum: START(1) + LENGTH(1) + ADDR(8) + CBOR(3+) + CRC(2) + END(1) = 16+ */
  zassert_true(len >= 16, "Encoded size should be at least minimum");
}

/* Test encoding with maximum payload */
ZTEST(fusain_encoding, test_encode_max_payload)
{
  fusain_packet_t packet = {
    .length = FUSAIN_MAX_PAYLOAD_SIZE,
    .msg_type = 0x30,
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
    .msg_type = 0x30,
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
    .msg_type = 0x20,
  };

  uint8_t small_buffer[10];
  uint8_t large_buffer[FUSAIN_MAX_PACKET_SIZE * 2];

  /* Test NULL packet */
  int len = fusain_encode_packet(NULL, small_buffer, sizeof(small_buffer));
  zassert_true(len < 0, "NULL packet should fail");

  /* Test NULL buffer */
  len = fusain_encode_packet(&packet, NULL, 100);
  zassert_true(len < 0, "NULL buffer should fail");

  /* Test buffer too small */
  len = fusain_encode_packet(&packet, small_buffer, 2);
  zassert_true(len < 0, "Too small buffer should fail");

  /* Test invalid payload length (use large buffer so we hit length check) */
  packet.length = FUSAIN_MAX_PAYLOAD_SIZE + 1;
  len = fusain_encode_packet(&packet, large_buffer, sizeof(large_buffer));
  zassert_equal(len, -2, "Oversized payload should return -2");
}

/* Test encoding determinism */
ZTEST(fusain_encoding, test_encode_determinism)
{
  fusain_packet_t packet = {
    .length = 8,
    .msg_type = 0x31,
  };
  memcpy(packet.payload, "TESTDATA", 8);

  uint8_t buffer1[FUSAIN_MAX_PACKET_SIZE * 2];
  uint8_t buffer2[FUSAIN_MAX_PACKET_SIZE * 2];

  int len1 = fusain_encode_packet(&packet, buffer1, sizeof(buffer1));
  int len2 = fusain_encode_packet(&packet, buffer2, sizeof(buffer2));

  zassert_equal(len1, len2, "Encoded length should be consistent");
  zassert_mem_equal(buffer1, buffer2, len1, "Encoded data should be identical");
}

/* Test buffer overflow at various encoding stages */
ZTEST(fusain_encoding, test_encode_buffer_overflow_stages)
{
  fusain_packet_t packet = {
    .length = 0,
    .address = 0x0102030405060708,
    .msg_type = 0x30,
  };

  uint8_t buffer[256];
  int len;

  /* Buffer size 0 - fails initial validation */
  len = fusain_encode_packet(&packet, buffer, 0);
  zassert_equal(len, -1, "Size 0 should fail validation");

  /* Buffer too small after START - fails at LENGTH */
  len = fusain_encode_packet(&packet, buffer, 2);
  zassert_true(len < 0, "Size 2 should fail during LENGTH stuffing");

  /* Buffer exhausted during ADDRESS stuffing */
  len = fusain_encode_packet(&packet, buffer, 5);
  zassert_true(len < 0, "Size 5 should fail during ADDRESS stuffing");

  /* Buffer exhausted during CRC stuffing */
  len = fusain_encode_packet(&packet, buffer, 11);
  zassert_true(len < 0, "Size 11 should fail during CRC stuffing");

  /* Buffer exhausted at END byte */
  len = fusain_encode_packet(&packet, buffer, 12);
  zassert_true(len < 0, "Size 12 should fail at END byte");
}

/* Test buffer overflow during payload stuffing */
ZTEST(fusain_encoding, test_encode_payload_overflow)
{
  fusain_packet_t packet = {
    .length = 10,
    .address = 0,
    .msg_type = 0x30,
  };
  memset(packet.payload, 0x42, 10);

  uint8_t buffer[20];
  int len;

  /* Buffer too small for payload
   * Need: START(1) + LENGTH(1) + ADDR(8) + PAYLOAD(10) + CRC(2) + END(1) = 23 */
  len = fusain_encode_packet(&packet, buffer, 18);
  zassert_true(len < 0, "Should fail during payload stuffing");
}

/* Test stuff_byte overflow paths with escape sequences */
ZTEST(fusain_encoding, test_encode_escape_overflow)
{
  fusain_packet_t packet = {
    .length = 3,
    .address = 0,
    .msg_type = 0x30,
  };
  /* Payload bytes that require escaping */
  packet.payload[0] = FUSAIN_START_BYTE;
  packet.payload[1] = FUSAIN_END_BYTE;
  packet.payload[2] = FUSAIN_ESC_BYTE;

  uint8_t buffer[256];
  int len;

  /* With escaping, the 3 payload bytes need 6 bytes.
   * Minimal packet needs: START(1) + LENGTH(1) + ADDR(8) + PAYLOAD(6) + CRC(2) + END(1) = 19
   * Size 18 should fail */
  len = fusain_encode_packet(&packet, buffer, 18);
  zassert_true(len < 0, "Should fail when escaping payload bytes");
}

/* Test stuff_byte normal byte overflow */
ZTEST(fusain_encoding, test_encode_normal_byte_overflow)
{
  fusain_packet_t packet = {
    .length = 5,
    .address = 0,
    .msg_type = 0x30,
  };
  memset(packet.payload, 0x42, 5); /* Normal bytes, no escaping needed */

  uint8_t buffer[256];
  int len;

  /* Packet with 5 normal payload bytes needs:
   * START(1) + LENGTH(1) + ADDR(8) + PAYLOAD(5) + CRC(2) + END(1) = 18
   * Size 16 should fail during normal payload byte stuffing */
  len = fusain_encode_packet(&packet, buffer, 16);
  zassert_true(len < 0, "Should fail during normal byte stuffing");
}

/* Test overflow when address bytes need escaping */
ZTEST(fusain_encoding, test_encode_type_escape_overflow)
{
  fusain_packet_t packet = {
    .length = 0,
    .address = 0x007E7E7E, /* Low 3 bytes are START_BYTE and need escaping */
    .msg_type = 0x30,
  };

  uint8_t buffer[256];
  int len;

  /* With 3 escaped address bytes (6 bytes total for those 3):
   * START(1) + LENGTH(1) + ADDR(5 normal + 6 escaped = 11) + CRC(2) + END(1) = 16
   * Size 13 should fail during address escaping */
  len = fusain_encode_packet(&packet, buffer, 13);
  zassert_true(len < 0, "Should fail when ADDRESS bytes need escaping");
}

/* Test overflow when address byte needs escaping */
ZTEST(fusain_encoding, test_encode_address_escape_overflow)
{
  fusain_packet_t packet = {
    .length = 0,
    .address = 0x7E7E7E7E7E7E7E7EULL, /* All 8 bytes are START_BYTE */
    .msg_type = 0x30,
  };

  uint8_t buffer[256];
  int len;

  /* With all 8 address bytes needing escape (16 bytes):
   * START(1) + LENGTH(1) + ADDR(16) + CRC(2) + END(1) = 21
   * Buffer size 14 should fail during address byte stuffing */
  len = fusain_encode_packet(&packet, buffer, 14);
  zassert_true(len < 0, "Should fail when ADDRESS bytes need escaping");
}

/* Test overflow during CRC stuffing */
ZTEST(fusain_encoding, test_encode_crc_escape_overflow)
{
  fusain_packet_t packet = {
    .length = 0,
    .address = 0x00007E7E, /* Low 2 bytes need escaping */
    .msg_type = 0x42,
  };

  uint8_t buffer[256];
  int len;

  /* With 2 escaped address bytes:
   * START(1) + LENGTH(1) + ADDR(6 normal + 4 escaped = 10) + CRC(2) + END(1) = 15
   * Buffer 13 should fail at CRC stuffing or END */
  len = fusain_encode_packet(&packet, buffer, 13);
  zassert_true(len < 0, "Should fail during CRC stuffing or END");
}

/* Test stuff_byte escape overflow path specifically */
ZTEST(fusain_encoding, test_stuff_byte_escape_boundary)
{
  /* Create scenario where we have exactly 1 byte left in buffer
   * but need to write an escaped byte (2 bytes). */
  fusain_packet_t packet = {
    .length = 100,
    .address = 0,
    .msg_type = 0x30,
  };
  /* Fill most of payload with normal bytes */
  memset(packet.payload, 0x42, 100);
  /* Put escape-needing byte near the end */
  packet.payload[99] = FUSAIN_START_BYTE;

  uint8_t buffer[256];
  int len;

  /* Normal encoding: START(1) + LENGTH(1) + ADDR(8) + PAYLOAD(100) + CRC(2) + END(1) = 113
   * With one escaped byte in payload: 114
   * Try buffer of 113 to trigger escape overflow */
  len = fusain_encode_packet(&packet, buffer, 113);
  zassert_true(len < 0, "Should fail when escape needs 2 bytes but only 1 left");
}

/* Test suite setup */
ZTEST_SUITE(fusain_encoding, NULL, NULL, NULL, NULL, NULL);
