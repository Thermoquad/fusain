/*
 * Copyright (c) 2025 Kaz Walker, Thermoquad
 * SPDX-License-Identifier: Apache-2.0
 *
 * Fusain Serial Protocol - Net Buffer API
 *
 * Convenience wrapper for decoding Fusain packets directly to Zephyr net_buf
 * for easy routing to Bluetooth, TCP, or other subsystems.
 */

#include <zephyr/net_buf.h>

#include <fusain/fusain.h>

struct net_buf* fusain_decode_byte_to_net_buf(uint8_t byte,
    struct net_buf_pool* pool,
    fusain_decoder_t* decoder)
{
  fusain_packet_t packet;
  fusain_decode_result_t result = fusain_decode_byte(byte, &packet, decoder);

  if (result == FUSAIN_DECODE_OK) {
    struct net_buf* buf = net_buf_alloc(pool, K_NO_WAIT);
    if (buf == NULL) {
      return NULL; /* Pool exhausted */
    }
    net_buf_add_mem(buf, &packet, sizeof(fusain_packet_t));
    return buf;
  }

  return NULL; /* Incomplete or error (decoder auto-resets on error) */
}

fusain_packet_t* fusain_packet_from_buf(struct net_buf* buf)
{
  return (fusain_packet_t*)buf->data;
}
