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

  /* Buffer size 0 - fails initial validation (line 67) */
  len = fusain_encode_packet(&packet, buffer, 0);
  zassert_equal(len, -1, "Size 0 should fail validation");

  /* Buffer too small after START - fails at LENGTH stuffing (line 96) */
  len = fusain_encode_packet(&packet, buffer, 2);
  zassert_true(len < 0, "Size 2 should fail during LENGTH stuffing");

  /* Buffer exhausted during ADDRESS stuffing (line 103) */
  len = fusain_encode_packet(&packet, buffer, 5);
  zassert_true(len < 0, "Size 5 should fail during ADDRESS stuffing");

  /* Buffer exhausted during TYPE stuffing (line 109) */
  len = fusain_encode_packet(&packet, buffer, 10);
  zassert_true(len < 0, "Size 10 should fail during TYPE stuffing");

  /* Buffer exhausted during CRC stuffing (lines 123, 126) */
  len = fusain_encode_packet(&packet, buffer, 12);
  zassert_true(len < 0, "Size 12 should fail during CRC stuffing");

  /* Buffer exhausted at END byte (line 131) */
  len = fusain_encode_packet(&packet, buffer, 13);
  zassert_true(len < 0, "Size 13 should fail at END byte");
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

  /* Buffer too small for payload (line 115) */
  len = fusain_encode_packet(&packet, buffer, 15);
  zassert_true(len < 0, "Should fail during payload stuffing");
}

/* Test stuff_byte overflow paths with escape sequences */
ZTEST(fusain_encoding, test_encode_escape_overflow)
{
  fusain_packet_t packet = {
    .length = 1,
    .address = 0,
    .msg_type = 0x30,
  };
  /* Payload byte that requires escaping */
  packet.payload[0] = FUSAIN_START_BYTE;

  uint8_t buffer[256];
  int len;

  /*
   * With escaping, the payload byte needs 2 bytes.
   * A minimal packet with 1-byte payload needs:
   * START(1) + LENGTH(1) + ADDR(8) + TYPE(1) + PAYLOAD(2 escaped) + CRC(2) + END(1) = 16
   * Size 15 should fail when trying to stuff the escaped payload byte (line 49)
   */
  len = fusain_encode_packet(&packet, buffer, 15);
  zassert_true(len < 0, "Should fail when escaping payload byte");
}

/* Test stuff_byte normal byte overflow (line 56) */
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

  /*
   * Packet with 5 normal payload bytes needs:
   * START(1) + LENGTH(1) + ADDR(8) + TYPE(1) + PAYLOAD(5) + CRC(2) + END(1) = 19
   * Size 17 should fail during normal payload byte stuffing (line 56)
   */
  len = fusain_encode_packet(&packet, buffer, 17);
  zassert_true(len < 0, "Should fail during normal byte stuffing");
}

/* Test overflow when msg_type needs escaping (line 109) */
ZTEST(fusain_encoding, test_encode_type_escape_overflow)
{
  fusain_packet_t packet = {
    .length = 0,
    /* 3 address bytes need escaping: index after address = 1 + 1 + (3*2 + 5) = 13 */
    .address = 0x007E7E7E, /* Low 3 bytes are START_BYTE */
    .msg_type = FUSAIN_START_BYTE, /* 0x7E needs escaping - 2 bytes */
  };

  uint8_t buffer[256];
  int len;

  /*
   * With 3 escaped address bytes: index = 13 after address
   * TYPE needs escape: check 13 + 2 > 14 => TRUE, fail at line 109
   */
  len = fusain_encode_packet(&packet, buffer, 14);
  zassert_true(len < 0, "Should fail when TYPE needs escaping");
}

/* Test overflow when address byte needs escaping (line 103) */
ZTEST(fusain_encoding, test_encode_address_escape_overflow)
{
  fusain_packet_t packet = {
    .length = 0,
    /* Multiple address bytes need escaping to overflow during address stuffing */
    .address = 0x7E7E7E7E7E7E7E7EULL, /* All 8 bytes are START_BYTE */
    .msg_type = 0x30,
  };

  uint8_t buffer[256];
  int len;

  /*
   * With all 8 address bytes needing escape:
   * START(1) + LENGTH(1) + ADDR(16) + TYPE(1) + CRC(2) + END(1) = 22
   * Buffer size 14 should fail during address byte 5 stuffing (line 103)
   */
  len = fusain_encode_packet(&packet, buffer, 14);
  zassert_true(len < 0, "Should fail when ADDRESS bytes need escaping");
}

/* Test overflow when CRC high byte needs escaping (line 123) */
ZTEST(fusain_encoding, test_encode_crc_escape_overflow)
{
  /*
   * To fail at CRC stuffing, we need index to be high enough that
   * CRC stuffing overflows. Since CRC depends on data, we use
   * address bytes that need escaping to consume buffer space,
   * leaving just enough room to reach CRC but not complete it.
   */
  fusain_packet_t packet = {
    .length = 0,
    /* 2 escaped address bytes + 6 normal = index = 1 + 1 + (2*2 + 6) = 12 after addr */
    .address = 0x00007E7E, /* Low 2 bytes need escaping */
    .msg_type = 0x42, /* Normal, doesn't need escaping, index = 13 after type */
  };

  uint8_t buffer[256];
  int len;

  /*
   * After TYPE: index = 13
   * CRC high: if it needs escape, check 13 + 2 > 14 => TRUE, fail at line 123
   * If CRC high doesn't need escape: index = 14, then CRC low check...
   * Either way, buffer 14 is too small for this packet.
   * Let's test with buffer = 14.
   */
  len = fusain_encode_packet(&packet, buffer, 14);
  zassert_true(len < 0, "Should fail during CRC stuffing or END");
}

/* Test stuff_byte escape overflow path specifically (line 49) */
ZTEST(fusain_encoding, test_stuff_byte_escape_boundary)
{
  /*
   * Create scenario where we have exactly 1 byte left in buffer
   * but need to write an escaped byte (2 bytes).
   * Use a large payload with escape bytes at the end.
   */
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

  /*
   * Normal encoding: START(1) + LENGTH(1) + ADDR(8) + TYPE(1) + PAYLOAD(100) + CRC(2) + END(1) = 114
   * With one escaped byte in payload: 115
   * Try buffer of 114 to trigger escape overflow
   */
  len = fusain_encode_packet(&packet, buffer, 114);
  zassert_true(len < 0, "Should fail when escape needs 2 bytes but only 1 left");
}

/* Test suite setup */
ZTEST_SUITE(fusain_encoding, NULL, NULL, NULL, NULL, NULL);
