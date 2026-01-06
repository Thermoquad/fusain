/*
 * Copyright (c) 2025 Kaz Walker, Thermoquad
 * SPDX-License-Identifier: Apache-2.0
 *
 * Fusain Protocol Library - CRC-16-CCITT Tests
 */

#include <fusain/fusain.h>
#include <string.h>
#include <zephyr/ztest.h>

/* Test CRC calculation with known values */
ZTEST(fusain_crc, test_crc_known_values)
{
  uint8_t data1[] = { 0x00 };
  uint16_t crc1 = helios_crc16(data1, sizeof(data1));
  zassert_not_equal(crc1, 0, "CRC should not be zero for non-empty data");

  /* CRC should be different for different data */
  uint8_t data2[] = { 0x01 };
  uint16_t crc2 = helios_crc16(data2, sizeof(data2));
  zassert_not_equal(crc1, crc2, "Different data should produce different CRCs");
}

/* Test CRC with empty data */
ZTEST(fusain_crc, test_crc_empty)
{
  uint8_t data[] = { 0x00 };
  uint16_t crc = helios_crc16(data, 0);
  zassert_equal(crc, 0xFFFF, "CRC of empty data should be initial value 0xFFFF");
}

/* Test CRC determinism */
ZTEST(fusain_crc, test_crc_determinism)
{
  uint8_t data[] = { 0xDE, 0xAD, 0xBE, 0xEF };

  uint16_t crc1 = helios_crc16(data, sizeof(data));
  uint16_t crc2 = helios_crc16(data, sizeof(data));
  zassert_equal(crc1, crc2, "CRC should be deterministic");
}

/* Test CRC with various lengths */
ZTEST(fusain_crc, test_crc_various_lengths)
{
  uint8_t data[HELIOS_MAX_PAYLOAD_SIZE];

  /* Fill with pattern */
  for (int i = 0; i < sizeof(data); i++) {
    data[i] = i & 0xFF;
  }

  /* Test different lengths */
  uint16_t prev_crc = 0xFFFF;
  for (int len = 1; len <= sizeof(data); len++) {
    uint16_t crc = helios_crc16(data, len);
    /* Each length should produce a different CRC */
    zassert_not_equal(crc, prev_crc,
        "Different lengths should produce different CRCs (len=%d)",
        len);
    prev_crc = crc;
  }
}

/* Test CRC byte order independence */
ZTEST(fusain_crc, test_crc_byte_order)
{
  uint8_t data1[] = { 0x12, 0x34 };
  uint8_t data2[] = { 0x34, 0x12 };

  uint16_t crc1 = helios_crc16(data1, sizeof(data1));
  uint16_t crc2 = helios_crc16(data2, sizeof(data2));

  /* Different byte order should produce different CRC */
  zassert_not_equal(crc1, crc2, "Byte order should affect CRC");
}

/* Test suite setup */
ZTEST_SUITE(fusain_crc, NULL, NULL, NULL, NULL, NULL);
