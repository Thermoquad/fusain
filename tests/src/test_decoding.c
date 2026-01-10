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
  fusain_packet_t tx_packet = {
    .length = 5,
    .msg_type = FUSAIN_MSG_STATE_DATA,
  };
  memcpy(tx_packet.payload, "HELLO", 5);

  /* Encode */
  uint8_t buffer[FUSAIN_MAX_PACKET_SIZE * 2];
  int encoded_len = fusain_encode_packet(&tx_packet, buffer, sizeof(buffer));
  zassert_true(encoded_len > 0, "Encoding should succeed");

  /* Decode */
  fusain_decoder_t decoder;
  fusain_reset_decoder(&decoder);

  fusain_packet_t rx_packet;
  fusain_decode_result_t result = FUSAIN_DECODE_INCOMPLETE;

  for (int i = 0; i < encoded_len; i++) {
    result = fusain_decode_byte(buffer[i], &rx_packet, &decoder);
    if (result == FUSAIN_DECODE_OK) {
      break;
    }
  }

  zassert_equal(result, FUSAIN_DECODE_OK, "Decoding should succeed");
  zassert_equal(rx_packet.length, tx_packet.length, "Length should match");
  zassert_equal(rx_packet.msg_type, tx_packet.msg_type, "Type should match");
  zassert_mem_equal(rx_packet.payload, tx_packet.payload, tx_packet.length,
      "Payload should match");
}

/* Test decoding with empty payload */
ZTEST(fusain_decoding, test_roundtrip_empty_payload)
{
  fusain_packet_t tx_packet = {
    .length = 0,
    .msg_type = FUSAIN_MSG_PING_REQUEST,
  };

  uint8_t buffer[FUSAIN_MAX_PACKET_SIZE * 2];
  int encoded_len = fusain_encode_packet(&tx_packet, buffer, sizeof(buffer));
  zassert_true(encoded_len > 0, "Encoding should succeed");

  fusain_decoder_t decoder;
  fusain_reset_decoder(&decoder);

  fusain_packet_t rx_packet;
  fusain_decode_result_t result = FUSAIN_DECODE_INCOMPLETE;

  for (int i = 0; i < encoded_len; i++) {
    result = fusain_decode_byte(buffer[i], &rx_packet, &decoder);
  }

  zassert_equal(result, FUSAIN_DECODE_OK, "Should decode empty payload");
  zassert_equal(rx_packet.length, 0, "Length should be 0");
}

/* Test decoding with byte stuffing */
ZTEST(fusain_decoding, test_roundtrip_byte_stuffing)
{
  fusain_packet_t tx_packet = {
    .length = 3,
    .msg_type = FUSAIN_MSG_STATE_COMMAND,
  };
  tx_packet.payload[0] = FUSAIN_START_BYTE;
  tx_packet.payload[1] = FUSAIN_END_BYTE;
  tx_packet.payload[2] = FUSAIN_ESC_BYTE;

  uint8_t buffer[FUSAIN_MAX_PACKET_SIZE * 2];
  int encoded_len = fusain_encode_packet(&tx_packet, buffer, sizeof(buffer));

  fusain_decoder_t decoder;
  fusain_reset_decoder(&decoder);

  fusain_packet_t rx_packet;
  fusain_decode_result_t result = FUSAIN_DECODE_INCOMPLETE;

  for (int i = 0; i < encoded_len; i++) {
    result = fusain_decode_byte(buffer[i], &rx_packet, &decoder);
  }

  zassert_equal(result, FUSAIN_DECODE_OK, "Should decode stuffed bytes");
  zassert_equal(rx_packet.payload[0], FUSAIN_START_BYTE, "START should unstuff");
  zassert_equal(rx_packet.payload[1], FUSAIN_END_BYTE, "END should unstuff");
  zassert_equal(rx_packet.payload[2], FUSAIN_ESC_BYTE, "ESC should unstuff");
}

/* Test decoding with CRC error */
ZTEST(fusain_decoding, test_decode_crc_error)
{
  fusain_packet_t tx_packet = {
    .length = 4,
    .msg_type = FUSAIN_MSG_MOTOR_COMMAND,
  };
  memcpy(tx_packet.payload, "TEST", 4);

  uint8_t buffer[FUSAIN_MAX_PACKET_SIZE * 2];
  int encoded_len = fusain_encode_packet(&tx_packet, buffer, sizeof(buffer));

  /* Corrupt a data byte (not START or END) */
  buffer[5] ^= 0xFF;

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

  zassert_equal(result, FUSAIN_DECODE_INVALID_CRC, "Should detect CRC error");
}

/* Test decoder reset */
ZTEST(fusain_decoding, test_decoder_reset)
{
  fusain_decoder_t decoder;

  /* Put decoder in some state */
  decoder.state = 5;
  decoder.buffer_index = 10;
  decoder.escape_next = true;

  /* Reset */
  fusain_reset_decoder(&decoder);

  zassert_equal(decoder.state, 0, "State should be reset");
  zassert_equal(decoder.buffer_index, 0, "Index should be reset");
  zassert_false(decoder.escape_next, "Escape flag should be reset");
}

/* Test decoding with maximum payload */
ZTEST(fusain_decoding, test_roundtrip_max_payload)
{
  fusain_packet_t tx_packet = {
    .length = FUSAIN_MAX_PAYLOAD_SIZE,
    .msg_type = FUSAIN_MSG_STATE_DATA,
  };

  /* Fill with pattern */
  for (int i = 0; i < tx_packet.length; i++) {
    tx_packet.payload[i] = i & 0xFF;
  }

  uint8_t buffer[FUSAIN_MAX_PACKET_SIZE * 2];
  int encoded_len = fusain_encode_packet(&tx_packet, buffer, sizeof(buffer));

  fusain_decoder_t decoder;
  fusain_reset_decoder(&decoder);

  fusain_packet_t rx_packet;
  fusain_decode_result_t result = FUSAIN_DECODE_INCOMPLETE;

  for (int i = 0; i < encoded_len; i++) {
    result = fusain_decode_byte(buffer[i], &rx_packet, &decoder);
  }

  zassert_equal(result, FUSAIN_DECODE_OK, "Should decode max payload");
  zassert_equal(rx_packet.length, FUSAIN_MAX_PAYLOAD_SIZE, "Length should match");
  zassert_mem_equal(rx_packet.payload, tx_packet.payload, tx_packet.length,
      "Payload should match");
}

/* Test round-trip consistency with various payload sizes */
ZTEST(fusain_decoding, test_roundtrip_consistency)
{
  /* Test several specific payload sizes: 1, 8, 32, 57, 114 (max) */
  uint8_t test_sizes[] = { 1, 8, 32, 57, FUSAIN_MAX_PAYLOAD_SIZE };

  for (int i = 0; i < sizeof(test_sizes); i++) {
    fusain_packet_t tx_packet = {
      .length = test_sizes[i],
      .msg_type = FUSAIN_MSG_STATE_DATA,
    };

    /* Fill with pattern */
    for (int j = 0; j < tx_packet.length; j++) {
      tx_packet.payload[j] = (i + j) & 0xFF;
    }

    uint8_t buffer[FUSAIN_MAX_PACKET_SIZE * 2];
    int encoded_len = fusain_encode_packet(&tx_packet, buffer, sizeof(buffer));

    fusain_decoder_t decoder;
    fusain_reset_decoder(&decoder);

    fusain_packet_t rx_packet;
    fusain_decode_result_t result = FUSAIN_DECODE_INCOMPLETE;

    for (int j = 0; j < encoded_len; j++) {
      result = fusain_decode_byte(buffer[j], &rx_packet, &decoder);
    }

    zassert_equal(result, FUSAIN_DECODE_OK,
        "Size %d: Decoding should succeed", test_sizes[i]);
    zassert_equal(rx_packet.length, tx_packet.length,
        "Size %d: Length should match", test_sizes[i]);
    zassert_equal(rx_packet.msg_type, tx_packet.msg_type,
        "Size %d: Type should match", test_sizes[i]);
    zassert_mem_equal(rx_packet.payload, tx_packet.payload, tx_packet.length,
        "Size %d: Payload should match", test_sizes[i]);
  }
}

/* Test decoder in IDLE state receiving non-START bytes (lines 168-170) */
ZTEST(fusain_decoding, test_decode_idle_garbage)
{
  fusain_decoder_t decoder;
  fusain_reset_decoder(&decoder);

  fusain_packet_t rx_packet;
  fusain_decode_result_t result;

  /* Send various non-START bytes while in IDLE state */
  result = fusain_decode_byte(0x00, &rx_packet, &decoder);
  zassert_equal(result, FUSAIN_DECODE_INCOMPLETE,
      "IDLE state should ignore non-START byte");

  result = fusain_decode_byte(0xFF, &rx_packet, &decoder);
  zassert_equal(result, FUSAIN_DECODE_INCOMPLETE,
      "IDLE state should ignore non-START byte");

  result = fusain_decode_byte(0x42, &rx_packet, &decoder);
  zassert_equal(result, FUSAIN_DECODE_INCOMPLETE,
      "IDLE state should ignore non-START byte");

  /* Decoder should still be in IDLE and ready to accept START */
  result = fusain_decode_byte(FUSAIN_START_BYTE, &rx_packet, &decoder);
  zassert_equal(result, FUSAIN_DECODE_INCOMPLETE,
      "Should accept START after garbage");
}

/* Test decoder receiving invalid length > 114 (lines 174-175) */
ZTEST(fusain_decoding, test_decode_invalid_length)
{
  fusain_decoder_t decoder;
  fusain_reset_decoder(&decoder);

  fusain_packet_t rx_packet;
  fusain_decode_result_t result;

  /* Send START byte */
  result = fusain_decode_byte(FUSAIN_START_BYTE, &rx_packet, &decoder);
  zassert_equal(result, FUSAIN_DECODE_INCOMPLETE, "Should accept START");

  /* Send invalid length (> 114) */
  result = fusain_decode_byte(FUSAIN_MAX_PAYLOAD_SIZE + 1, &rx_packet, &decoder);
  zassert_equal(result, FUSAIN_DECODE_INVALID_LENGTH,
      "Should reject length > 114");

  /* Decoder should be back in IDLE */
  result = fusain_decode_byte(0x42, &rx_packet, &decoder);
  zassert_equal(result, FUSAIN_DECODE_INCOMPLETE,
      "Should be in IDLE after invalid length");
}

/* Test decoder expecting END but receiving wrong byte (lines 226-227) */
ZTEST(fusain_decoding, test_decode_missing_end_byte)
{
  fusain_packet_t tx_packet = {
    .length = 0,
    .address = 0x0102030405060708,
    .msg_type = FUSAIN_MSG_PING_REQUEST,
  };

  uint8_t buffer[FUSAIN_MAX_PACKET_SIZE * 2];
  int encoded_len = fusain_encode_packet(&tx_packet, buffer, sizeof(buffer));
  zassert_true(encoded_len > 0, "Encoding should succeed");

  /* Replace END byte with something else */
  buffer[encoded_len - 1] = 0x42;

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

  zassert_equal(result, FUSAIN_DECODE_INVALID_START,
      "Should fail when END byte is missing");
}

/* Test decoder with corrupted state (defensive code, lines 241-243) */
ZTEST(fusain_decoding, test_decode_invalid_state)
{
  fusain_decoder_t decoder;
  fusain_reset_decoder(&decoder);

  /* Manually corrupt the decoder state to an invalid value */
  decoder.state = 255; /* Invalid state value */

  fusain_packet_t rx_packet;
  fusain_decode_result_t result;

  /* Send any byte - should trigger default case */
  result = fusain_decode_byte(0x42, &rx_packet, &decoder);
  zassert_equal(result, FUSAIN_DECODE_INVALID_START,
      "Invalid state should return error");

  /* Decoder should be reset to IDLE */
  zassert_equal(decoder.state, 0, "Should reset to IDLE after invalid state");
}

/* Test suite setup */
ZTEST_SUITE(fusain_decoding, NULL, NULL, NULL, NULL, NULL);
