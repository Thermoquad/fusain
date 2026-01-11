/*
 * Copyright (c) 2025 Kaz Walker, Thermoquad
 * SPDX-License-Identifier: Apache-2.0
 *
 * Fusain Protocol Library - Packet Decoding Tests
 *
 * Wire format: [START][LENGTH][ADDRESS(8)][CBOR_PAYLOAD][CRC(2)][END]
 * CBOR payload contains [msg_type, payload_map]
 */

#include <fusain/fusain.h>
#include <string.h>
#include <zephyr/ztest.h>

/* Test basic round-trip encoding/decoding using helper functions */
ZTEST(fusain_decoding, test_roundtrip_basic)
{
  /* Use a helper function to create a valid CBOR packet */
  fusain_packet_t tx_packet;
  fusain_create_motor_command(&tx_packet, 0x1234, 0, 3000);

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
  zassert_equal(rx_packet.address, tx_packet.address, "Address should match");
  zassert_mem_equal(rx_packet.payload, tx_packet.payload, tx_packet.length,
      "Payload should match");
}

/* Test decoding with nil payload (PING_REQUEST, DISCOVERY_REQUEST) */
ZTEST(fusain_decoding, test_roundtrip_empty_payload)
{
  /* In CBOR mode, PING_REQUEST has [type, nil] */
  fusain_packet_t tx_packet;
  fusain_create_ping_request(&tx_packet, 0x5678);

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

  zassert_equal(result, FUSAIN_DECODE_OK, "Should decode nil payload");
  zassert_equal(rx_packet.msg_type, FUSAIN_MSG_PING_REQUEST, "Type should match");
}

/* Test decoding with byte stuffing */
ZTEST(fusain_decoding, test_roundtrip_byte_stuffing)
{
  /* Create a packet where address or data contains escapable bytes */
  fusain_packet_t tx_packet;
  fusain_create_device_announce(&tx_packet, 0x7E7D7F01, 1, 1, 1, 1);
  /* Address contains START_BYTE, ESC_BYTE, END_BYTE which need escaping */

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
  zassert_equal(rx_packet.address, tx_packet.address, "Address should match after unstuffing");
}

/* Test decoding with CRC error */
ZTEST(fusain_decoding, test_decode_crc_error)
{
  fusain_packet_t tx_packet;
  fusain_create_motor_command(&tx_packet, 0x1234, 0, 2000);

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

/* Test decoding with large CBOR payload (motor_config has many fields) */
ZTEST(fusain_decoding, test_roundtrip_max_payload)
{
  /* Create a motor_config packet which has many CBOR fields
   * This tests the decoder's ability to handle larger payloads */
  fusain_cmd_motor_config_t config = {
    .motor = 0,
    .pwm_period = 1000000, /* Large value to increase CBOR size */
    .pid_kp = 4.0,
    .pid_ki = 12.0,
    .pid_kd = 0.1,
    .max_rpm = 3400,
    .min_rpm = 800,
    .min_pwm_duty = 500000, /* Large value to increase CBOR size */
  };

  fusain_packet_t tx_packet;
  fusain_create_motor_config(&tx_packet, 0xABCDEF0123456789, &config);

  zassert_true(tx_packet.length > 0, "CBOR payload should not be empty");

  uint8_t buffer[FUSAIN_MAX_PACKET_SIZE * 2];
  int encoded_len = fusain_encode_packet(&tx_packet, buffer, sizeof(buffer));

  fusain_decoder_t decoder;
  fusain_reset_decoder(&decoder);

  fusain_packet_t rx_packet;
  fusain_decode_result_t result = FUSAIN_DECODE_INCOMPLETE;

  for (int i = 0; i < encoded_len; i++) {
    result = fusain_decode_byte(buffer[i], &rx_packet, &decoder);
  }

  zassert_equal(result, FUSAIN_DECODE_OK, "Should decode large CBOR payload");
  zassert_equal(rx_packet.length, tx_packet.length, "Length should match");
  zassert_equal(rx_packet.msg_type, tx_packet.msg_type, "Type should match");
  zassert_mem_equal(rx_packet.payload, tx_packet.payload, tx_packet.length,
      "Payload should match");
}

/* Test round-trip consistency with various message types */
ZTEST(fusain_decoding, test_roundtrip_consistency)
{
  fusain_packet_t tx_packet;
  const char* test_names[] = { "ping_request", "set_mode", "state_data",
    "motor_command", "device_announce" };

  for (int i = 0; i < 5; i++) {
    switch (i) {
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
      fusain_create_motor_command(&tx_packet, 0x4444, 0, 3000);
      break;
    case 4:
      fusain_create_device_announce(&tx_packet, 0x5555, 1, 1, 1, 1);
      break;
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
        "%s: Decoding should succeed", test_names[i]);
    zassert_equal(rx_packet.length, tx_packet.length,
        "%s: Length should match", test_names[i]);
    zassert_equal(rx_packet.msg_type, tx_packet.msg_type,
        "%s: Type should match", test_names[i]);
    zassert_mem_equal(rx_packet.payload, tx_packet.payload, tx_packet.length,
        "%s: Payload should match", test_names[i]);
  }
}

/* Test decoder in IDLE state receiving non-START bytes */
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

/* Test decoder receiving invalid length > 114 */
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

/* Test decoder expecting END but receiving wrong byte */
ZTEST(fusain_decoding, test_decode_missing_end_byte)
{
  fusain_packet_t tx_packet;
  fusain_create_ping_request(&tx_packet, 0x0102030405060708);

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

/* Test decoder with corrupted state (defensive code) */
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

/* Test CBOR header decoding edge cases */
ZTEST(fusain_decoding, test_decode_cbor_header_errors)
{
  fusain_decoder_t decoder;
  fusain_reset_decoder(&decoder);

  fusain_packet_t rx_packet;
  fusain_decode_result_t result;

  /* Build a packet with invalid CBOR header (not 0x82 array)
   * We need a valid CRC so the CBOR decode path is actually tested */
  uint8_t crc_data[] = {
    3, /* length */
    0x01,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, /* address (LE) */
    0x83,
    0x30,
    0xF6, /* Invalid: 3-element array instead of 2 */
  };
  uint16_t crc = fusain_crc16(crc_data, sizeof(crc_data));

  uint8_t invalid_cbor[] = {
    FUSAIN_START_BYTE,
    3, /* length = 3 bytes of CBOR */
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* address */
    0x83, 0x30, 0xF6, /* Invalid: 3-element array instead of 2 */
    (uint8_t)(crc >> 8), (uint8_t)(crc & 0xFF), /* Valid CRC */
    FUSAIN_END_BYTE
  };

  for (size_t i = 0; i < sizeof(invalid_cbor); i++) {
    result = fusain_decode_byte(invalid_cbor[i], &rx_packet, &decoder);
    if (result != FUSAIN_DECODE_INCOMPLETE) {
      break;
    }
  }

  /* Should fail due to invalid CBOR header (0x83 != 0x82) */
  zassert_equal(result, FUSAIN_DECODE_INVALID_START,
      "Should reject packet with invalid CBOR header");
}

/* Test decoding with truncated CBOR payload (buffer_size < 2) */
ZTEST(fusain_decoding, test_decode_truncated_cbor)
{
  fusain_decoder_t decoder;
  fusain_reset_decoder(&decoder);

  fusain_packet_t rx_packet;
  fusain_decode_result_t result;

  /* Build a packet with truncated CBOR (only 1 byte, need at least 2)
   * This triggers line 112: if (buffer_size < 2) return -1 */
  uint8_t crc_data[] = {
    1, /* length */
    0x01,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, /* address (LE) */
    0x82, /* Just the array header, no type */
  };
  uint16_t crc = fusain_crc16(crc_data, sizeof(crc_data));

  uint8_t truncated[] = {
    FUSAIN_START_BYTE,
    1, /* length = 1 byte (too short for valid CBOR array) */
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* address */
    0x82, /* Just the array header, no type or payload */
    (uint8_t)(crc >> 8), (uint8_t)(crc & 0xFF), /* Valid CRC */
    FUSAIN_END_BYTE
  };

  for (size_t i = 0; i < sizeof(truncated); i++) {
    result = fusain_decode_byte(truncated[i], &rx_packet, &decoder);
    if (result != FUSAIN_DECODE_INCOMPLETE) {
      break;
    }
  }

  /* Should fail due to truncated CBOR (buffer_size < 2) */
  zassert_equal(result, FUSAIN_DECODE_INVALID_START,
      "Should reject packet with truncated CBOR");
}

/* Test decoding with unsupported CBOR type encoding (line 133) */
ZTEST(fusain_decoding, test_decode_unsupported_cbor_type)
{
  fusain_decoder_t decoder;
  fusain_reset_decoder(&decoder);

  fusain_packet_t rx_packet;
  fusain_decode_result_t result;

  /* Build a packet with unsupported type encoding (0x19 = uint16 follows)
   * This triggers line 133: return -1 for unsupported encoding */
  uint8_t crc_data[] = {
    5, /* length */
    0x01,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, /* address (LE) */
    0x82,
    0x19,
    0x00,
    0x30,
    0xF6, /* Array with uint16 type - unsupported */
  };
  uint16_t crc = fusain_crc16(crc_data, sizeof(crc_data));

  uint8_t unsupported[] = {
    FUSAIN_START_BYTE,
    5, /* length */
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* address */
    0x82, 0x19, 0x00, 0x30, 0xF6, /* Array with uint16 type - unsupported */
    (uint8_t)(crc >> 8), (uint8_t)(crc & 0xFF), /* Valid CRC */
    FUSAIN_END_BYTE
  };

  for (size_t i = 0; i < sizeof(unsupported); i++) {
    result = fusain_decode_byte(unsupported[i], &rx_packet, &decoder);
    if (result != FUSAIN_DECODE_INCOMPLETE) {
      break;
    }
  }

  /* Should fail due to unsupported CBOR encoding */
  zassert_equal(result, FUSAIN_DECODE_INVALID_START,
      "Should reject packet with unsupported CBOR type encoding");
}

/* Test decoding with zero-length CBOR payload (line 285)
 * This happens when packet->length == 0 after address bytes */
ZTEST(fusain_decoding, test_decode_zero_length_payload)
{
  fusain_decoder_t decoder;
  fusain_reset_decoder(&decoder);

  fusain_packet_t rx_packet;
  fusain_decode_result_t result;

  /* Build a packet with zero-length CBOR payload
   * This should trigger line 285: if (packet->length == 0) state = CRC1 */
  uint8_t crc_data[] = {
    0, /* length = 0 */
    0x01,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, /* address (LE) */
    /* No CBOR payload */
  };
  uint16_t crc = fusain_crc16(crc_data, sizeof(crc_data));

  uint8_t zero_len_packet[] = {
    FUSAIN_START_BYTE,
    0, /* length = 0 */
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* address */
    /* No CBOR payload bytes */
    (uint8_t)(crc >> 8), (uint8_t)(crc & 0xFF), /* Valid CRC */
    FUSAIN_END_BYTE
  };

  for (size_t i = 0; i < sizeof(zero_len_packet); i++) {
    result = fusain_decode_byte(zero_len_packet[i], &rx_packet, &decoder);
    if (result != FUSAIN_DECODE_INCOMPLETE) {
      break;
    }
  }

  /* Should fail because zero-length CBOR is invalid (can't extract msg_type) */
  zassert_equal(result, FUSAIN_DECODE_INVALID_START,
      "Should reject packet with zero-length CBOR payload");
}

/* Test decoding with uint8 type prefix but truncated (line 127) */
ZTEST(fusain_decoding, test_decode_truncated_uint8_type)
{
  fusain_decoder_t decoder;
  fusain_reset_decoder(&decoder);

  fusain_packet_t rx_packet;
  fusain_decode_result_t result;

  /* Build a packet with uint8 type prefix (0x18) but no value follows
   * This triggers line 127: if (buffer_size < 3) return -1 */
  uint8_t crc_data[] = {
    2, /* length = 2 bytes (array header + 0x18, but no value) */
    0x01,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, /* address (LE) */
    0x82,
    0x18, /* Array header + uint8 prefix, missing value */
  };
  uint16_t crc = fusain_crc16(crc_data, sizeof(crc_data));

  uint8_t truncated[] = {
    FUSAIN_START_BYTE,
    2, /* length */
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* address */
    0x82, 0x18, /* Array header + uint8 prefix, missing value */
    (uint8_t)(crc >> 8), (uint8_t)(crc & 0xFF), /* Valid CRC */
    FUSAIN_END_BYTE
  };

  for (size_t i = 0; i < sizeof(truncated); i++) {
    result = fusain_decode_byte(truncated[i], &rx_packet, &decoder);
    if (result != FUSAIN_DECODE_INCOMPLETE) {
      break;
    }
  }

  /* Should fail due to truncated uint8 type encoding */
  zassert_equal(result, FUSAIN_DECODE_INVALID_START,
      "Should reject packet with truncated uint8 type prefix");
}

/* Test suite setup */
ZTEST_SUITE(fusain_decoding, NULL, NULL, NULL, NULL, NULL);
