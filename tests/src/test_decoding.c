/*
 * Copyright (c) 2025 Kaz Walker, Thermoquad
 * SPDX-License-Identifier: Apache-2.0
 *
 * Fusain Protocol Library - Packet Decoding Tests
 */

#include <fusain/fusain.h>
#include <string.h>
#include <zephyr/ztest.h>

/* Test basic round-trip encoding/decoding */
ZTEST(fusain_decoding, test_roundtrip_basic)
{
  helios_packet_t tx_packet = {
    .length = 5,
    .msg_type = HELIOS_MSG_STATE_DATA,
  };
  memcpy(tx_packet.payload, "HELLO", 5);

  /* Encode */
  uint8_t buffer[HELIOS_MAX_PACKET_SIZE * 2];
  int encoded_len = helios_encode_packet(&tx_packet, buffer, sizeof(buffer));
  zassert_true(encoded_len > 0, "Encoding should succeed");

  /* Decode */
  helios_decoder_t decoder;
  helios_reset_decoder(&decoder);

  helios_packet_t rx_packet;
  helios_decode_result_t result = HELIOS_DECODE_INCOMPLETE;

  for (int i = 0; i < encoded_len; i++) {
    result = helios_decode_byte(buffer[i], &rx_packet, &decoder);
    if (result == HELIOS_DECODE_OK) {
      break;
    }
  }

  zassert_equal(result, HELIOS_DECODE_OK, "Decoding should succeed");
  zassert_equal(rx_packet.length, tx_packet.length, "Length should match");
  zassert_equal(rx_packet.msg_type, tx_packet.msg_type, "Type should match");
  zassert_mem_equal(rx_packet.payload, tx_packet.payload, tx_packet.length,
      "Payload should match");
}

/* Test decoding with empty payload */
ZTEST(fusain_decoding, test_roundtrip_empty_payload)
{
  helios_packet_t tx_packet = {
    .length = 0,
    .msg_type = HELIOS_MSG_PING_REQUEST,
  };

  uint8_t buffer[HELIOS_MAX_PACKET_SIZE * 2];
  int encoded_len = helios_encode_packet(&tx_packet, buffer, sizeof(buffer));
  zassert_true(encoded_len > 0, "Encoding should succeed");

  helios_decoder_t decoder;
  helios_reset_decoder(&decoder);

  helios_packet_t rx_packet;
  helios_decode_result_t result = HELIOS_DECODE_INCOMPLETE;

  for (int i = 0; i < encoded_len; i++) {
    result = helios_decode_byte(buffer[i], &rx_packet, &decoder);
  }

  zassert_equal(result, HELIOS_DECODE_OK, "Should decode empty payload");
  zassert_equal(rx_packet.length, 0, "Length should be 0");
}

/* Test decoding with byte stuffing */
ZTEST(fusain_decoding, test_roundtrip_byte_stuffing)
{
  helios_packet_t tx_packet = {
    .length = 3,
    .msg_type = HELIOS_MSG_STATE_COMMAND,
  };
  tx_packet.payload[0] = HELIOS_START_BYTE;
  tx_packet.payload[1] = HELIOS_END_BYTE;
  tx_packet.payload[2] = HELIOS_ESC_BYTE;

  uint8_t buffer[HELIOS_MAX_PACKET_SIZE * 2];
  int encoded_len = helios_encode_packet(&tx_packet, buffer, sizeof(buffer));

  helios_decoder_t decoder;
  helios_reset_decoder(&decoder);

  helios_packet_t rx_packet;
  helios_decode_result_t result = HELIOS_DECODE_INCOMPLETE;

  for (int i = 0; i < encoded_len; i++) {
    result = helios_decode_byte(buffer[i], &rx_packet, &decoder);
  }

  zassert_equal(result, HELIOS_DECODE_OK, "Should decode stuffed bytes");
  zassert_equal(rx_packet.payload[0], HELIOS_START_BYTE, "START should unstuff");
  zassert_equal(rx_packet.payload[1], HELIOS_END_BYTE, "END should unstuff");
  zassert_equal(rx_packet.payload[2], HELIOS_ESC_BYTE, "ESC should unstuff");
}

/* Test decoding with CRC error */
ZTEST(fusain_decoding, test_decode_crc_error)
{
  helios_packet_t tx_packet = {
    .length = 4,
    .msg_type = HELIOS_MSG_MOTOR_COMMAND,
  };
  memcpy(tx_packet.payload, "TEST", 4);

  uint8_t buffer[HELIOS_MAX_PACKET_SIZE * 2];
  int encoded_len = helios_encode_packet(&tx_packet, buffer, sizeof(buffer));

  /* Corrupt a data byte (not START or END) */
  buffer[5] ^= 0xFF;

  helios_decoder_t decoder;
  helios_reset_decoder(&decoder);

  helios_packet_t rx_packet;
  helios_decode_result_t result = HELIOS_DECODE_INCOMPLETE;

  for (int i = 0; i < encoded_len; i++) {
    result = helios_decode_byte(buffer[i], &rx_packet, &decoder);
    if (result != HELIOS_DECODE_INCOMPLETE) {
      break;
    }
  }

  zassert_equal(result, HELIOS_DECODE_INVALID_CRC, "Should detect CRC error");
}

/* Test decoder reset */
ZTEST(fusain_decoding, test_decoder_reset)
{
  helios_decoder_t decoder;

  /* Put decoder in some state */
  decoder.state = 5;
  decoder.buffer_index = 10;
  decoder.escape_next = true;

  /* Reset */
  helios_reset_decoder(&decoder);

  zassert_equal(decoder.state, 0, "State should be reset");
  zassert_equal(decoder.buffer_index, 0, "Index should be reset");
  zassert_false(decoder.escape_next, "Escape flag should be reset");
}

/* Test decoding with maximum payload */
ZTEST(fusain_decoding, test_roundtrip_max_payload)
{
  helios_packet_t tx_packet = {
    .length = HELIOS_MAX_PAYLOAD_SIZE,
    .msg_type = HELIOS_MSG_TELEMETRY_BUNDLE,
  };

  /* Fill with pattern */
  for (int i = 0; i < tx_packet.length; i++) {
    tx_packet.payload[i] = i & 0xFF;
  }

  uint8_t buffer[HELIOS_MAX_PACKET_SIZE * 2];
  int encoded_len = helios_encode_packet(&tx_packet, buffer, sizeof(buffer));

  helios_decoder_t decoder;
  helios_reset_decoder(&decoder);

  helios_packet_t rx_packet;
  helios_decode_result_t result = HELIOS_DECODE_INCOMPLETE;

  for (int i = 0; i < encoded_len; i++) {
    result = helios_decode_byte(buffer[i], &rx_packet, &decoder);
  }

  zassert_equal(result, HELIOS_DECODE_OK, "Should decode max payload");
  zassert_equal(rx_packet.length, HELIOS_MAX_PAYLOAD_SIZE, "Length should match");
  zassert_mem_equal(rx_packet.payload, tx_packet.payload, tx_packet.length,
      "Payload should match");
}

/* Test round-trip consistency with various payload sizes */
ZTEST(fusain_decoding, test_roundtrip_consistency)
{
  /* Test several specific payload sizes: 1, 8, 32, 57, 114 (max) */
  uint8_t test_sizes[] = { 1, 8, 32, 57, HELIOS_MAX_PAYLOAD_SIZE };

  for (int i = 0; i < sizeof(test_sizes); i++) {
    helios_packet_t tx_packet = {
      .length = test_sizes[i],
      .msg_type = HELIOS_MSG_STATE_DATA,
    };

    /* Fill with pattern */
    for (int j = 0; j < tx_packet.length; j++) {
      tx_packet.payload[j] = (i + j) & 0xFF;
    }

    uint8_t buffer[HELIOS_MAX_PACKET_SIZE * 2];
    int encoded_len = helios_encode_packet(&tx_packet, buffer, sizeof(buffer));

    helios_decoder_t decoder;
    helios_reset_decoder(&decoder);

    helios_packet_t rx_packet;
    helios_decode_result_t result = HELIOS_DECODE_INCOMPLETE;

    for (int j = 0; j < encoded_len; j++) {
      result = helios_decode_byte(buffer[j], &rx_packet, &decoder);
    }

    zassert_equal(result, HELIOS_DECODE_OK,
        "Size %d: Decoding should succeed", test_sizes[i]);
    zassert_equal(rx_packet.length, tx_packet.length,
        "Size %d: Length should match", test_sizes[i]);
    zassert_equal(rx_packet.msg_type, tx_packet.msg_type,
        "Size %d: Type should match", test_sizes[i]);
    zassert_mem_equal(rx_packet.payload, tx_packet.payload, tx_packet.length,
        "Size %d: Payload should match", test_sizes[i]);
  }
}

/* Test suite setup */
ZTEST_SUITE(fusain_decoding, NULL, NULL, NULL, NULL, NULL);
