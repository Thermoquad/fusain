/*
 * Copyright (c) 2025 Kaz Walker, Thermoquad
 * SPDX-License-Identifier: Apache-2.0
 *
 * Zephyr random API compatibility for standalone testing
 */

#ifndef ZEPHYR_RANDOM_COMPAT_H_
#define ZEPHYR_RANDOM_COMPAT_H_

#include <stdint.h>
#include <stdlib.h>
#include <time.h>

static inline uint32_t sys_rand32_get(void)
{
  static int initialized = 0;
  if (!initialized) {
    srand((unsigned int)time(NULL));
    initialized = 1;
  }
  /* Combine two rand() calls for better distribution on systems with 15-bit
   * RAND_MAX */
  return ((uint32_t)rand() << 16) ^ (uint32_t)rand();
}

#endif /* ZEPHYR_RANDOM_COMPAT_H_ */
