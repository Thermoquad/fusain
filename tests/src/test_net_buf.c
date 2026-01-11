/*
 * Copyright (c) 2025 Kaz Walker, Thermoquad
 * SPDX-License-Identifier: Apache-2.0
 *
 * Fusain Protocol Library - Net Buffer API Tests
 */

#include <string.h>
#include <zephyr/net_buf.h>
#include <zephyr/ztest.h>

#include <fusain/fusain.h>

/* Define a net_buf pool for testing */
NET_BUF_POOL_DEFINE(test_pool, 4, sizeof(fusain_packet_t), 0, NULL);

/* Test basic decode to net_buf */
ZTEST(fusain_net_buf, test_decode_to_net_buf_basic)
{
  /* Create and encode a ping request packet */
  fusain_packet_t tx_packet;
  fusain_create_ping_request(&tx_packet, 0x0102030405060708ULL);

  uint8_t encoded[FUSAIN_MAX_PACKET_SIZE * 2];
  int encoded_len = fusain_encode_packet(&tx_packet, encoded, sizeof(encoded));
  zassert_true(encoded_len > 0, "Encoding should succeed");

  /* Decode byte-by-byte to net_buf */
  fusain_decoder_t decoder;
  fusain_reset_decoder(&decoder);

  struct net_buf* result = NULL;
  for (int i = 0; i < encoded_len; i++) {
    result = fusain_decode_byte_to_net_buf(encoded[i], &test_pool, &decoder);
  }

  zassert_not_null(result, "Should return net_buf on completion");

  /* Verify packet contents */
  fusain_packet_t* rx_packet = fusain_packet_from_buf(result);
  zassert_equal(rx_packet->msg_type, FUSAIN_MSG_PING_REQUEST,
      "Message type should match");
  zassert_equal(rx_packet->address, 0x0102030405060708ULL,
      "Address should match");

  net_buf_unref(result);
}

/* Test decode returns NULL on incomplete packet */
ZTEST(fusain_net_buf, test_decode_incomplete_returns_null)
{
  fusain_decoder_t decoder;
  fusain_reset_decoder(&decoder);

  /* Feed partial packet (just START byte) */
  struct net_buf* result = fusain_decode_byte_to_net_buf(FUSAIN_START_BYTE,
      &test_pool, &decoder);
  zassert_is_null(result, "Incomplete packet should return NULL");
}

/* Test decode returns NULL on error (bad CRC) */
ZTEST(fusain_net_buf, test_decode_error_returns_null)
{
  /* Create a packet with invalid CRC */
  uint8_t bad_packet[] = {
    FUSAIN_START_BYTE,
    3, /* length */
    0x01, /* address bytes... */
    0x02,
    0x03,
    0x04,
    0x05,
    0x06,
    0x07,
    0x08,
    0x82, /* CBOR array header */
    0x30, /* msg_type */
    0xF6, /* nil payload */
    0xFF, /* bad CRC high */
    0xFF, /* bad CRC low */
    FUSAIN_END_BYTE,
  };

  fusain_decoder_t decoder;
  fusain_reset_decoder(&decoder);

  struct net_buf* result = NULL;
  for (size_t i = 0; i < sizeof(bad_packet); i++) {
    result = fusain_decode_byte_to_net_buf(bad_packet[i], &test_pool, &decoder);
  }

  zassert_is_null(result, "Bad CRC should return NULL");
}

/* Test round-trip encode/decode with net_buf */
ZTEST(fusain_net_buf, test_round_trip_state_data)
{
  /* Create state data packet */
  fusain_packet_t tx_packet;
  fusain_create_state_data(&tx_packet, 0xDEADBEEFCAFEBABEULL,
      0, FUSAIN_ERROR_NONE, FUSAIN_STATE_HEATING, 12345);

  uint8_t encoded[FUSAIN_MAX_PACKET_SIZE * 2];
  int encoded_len = fusain_encode_packet(&tx_packet, encoded, sizeof(encoded));
  zassert_true(encoded_len > 0, "Encoding should succeed");

  /* Decode to net_buf */
  fusain_decoder_t decoder;
  fusain_reset_decoder(&decoder);

  struct net_buf* result = NULL;
  for (int i = 0; i < encoded_len; i++) {
    result = fusain_decode_byte_to_net_buf(encoded[i], &test_pool, &decoder);
  }

  zassert_not_null(result, "Should decode successfully");

  fusain_packet_t* rx_packet = fusain_packet_from_buf(result);
  zassert_equal(rx_packet->msg_type, FUSAIN_MSG_STATE_DATA,
      "Message type should match");
  zassert_equal(rx_packet->address, 0xDEADBEEFCAFEBABEULL,
      "Address should match");

  net_buf_unref(result);
}

/* Test multiple packets decoded sequentially */
ZTEST(fusain_net_buf, test_multiple_packets)
{
  fusain_decoder_t decoder;
  fusain_reset_decoder(&decoder);

  /* Encode two different packets */
  fusain_packet_t tx1, tx2;
  fusain_create_ping_request(&tx1, 0x1111111111111111ULL);
  fusain_create_ping_request(&tx2, 0x2222222222222222ULL);

  uint8_t encoded1[FUSAIN_MAX_PACKET_SIZE * 2];
  uint8_t encoded2[FUSAIN_MAX_PACKET_SIZE * 2];
  int len1 = fusain_encode_packet(&tx1, encoded1, sizeof(encoded1));
  int len2 = fusain_encode_packet(&tx2, encoded2, sizeof(encoded2));

  /* Decode first packet */
  struct net_buf* result = NULL;
  for (int i = 0; i < len1; i++) {
    result = fusain_decode_byte_to_net_buf(encoded1[i], &test_pool, &decoder);
  }
  zassert_not_null(result, "First packet should decode");
  fusain_packet_t* rx1 = fusain_packet_from_buf(result);
  zassert_equal(rx1->address, 0x1111111111111111ULL, "First address should match");
  net_buf_unref(result);

  /* Decode second packet (decoder should be ready for next) */
  result = NULL;
  for (int i = 0; i < len2; i++) {
    result = fusain_decode_byte_to_net_buf(encoded2[i], &test_pool, &decoder);
  }
  zassert_not_null(result, "Second packet should decode");
  fusain_packet_t* rx2 = fusain_packet_from_buf(result);
  zassert_equal(rx2->address, 0x2222222222222222ULL, "Second address should match");
  net_buf_unref(result);
}

/* Test suite setup */
ZTEST_SUITE(fusain_net_buf, NULL, NULL, NULL, NULL, NULL);
