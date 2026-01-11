/*
 * Copyright (c) 2025 Kaz Walker, Thermoquad
 * SPDX-License-Identifier: Apache-2.0
 *
 * Ztest Compatibility Layer for Standalone Testing
 *
 * This header provides minimal ztest-compatible macros for running
 * Fusain tests outside of Zephyr. It allows the same test code to
 * work in both Zephyr and standalone environments.
 */

#ifndef ZTEST_COMPAT_H_
#define ZTEST_COMPAT_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Test registration structures
 */

typedef void (*ztest_func_t)(void);

struct ztest_entry {
  const char* suite;
  const char* name;
  ztest_func_t func;
};

/* Maximum number of tests */
#define ZTEST_MAX_TESTS 256

/* Global test registry */
extern struct ztest_entry __ztest_registry[];
extern int __ztest_count;

/* Test result tracking */
extern int __ztest_passed;
extern int __ztest_failed;
extern const char* __ztest_current_suite;
extern const char* __ztest_current_test;

/*
 * Test registration macros
 */

/* Helper to set entry fields (avoids macro parameter name collision) */
static inline void __ztest_set_entry(
    struct ztest_entry* entry, const char* s, const char* n, ztest_func_t f)
{
  entry->suite = s;
  entry->name = n;
  entry->func = f;
}

/* Register a test function */
#define ZTEST(suite, name)                                                   \
  static void suite##_##name(void);                                          \
  __attribute__((constructor)) static void __register_##suite##_##name(void) \
  {                                                                          \
    if (__ztest_count < ZTEST_MAX_TESTS) {                                   \
      __ztest_set_entry(&__ztest_registry[__ztest_count], #suite, #name,     \
          suite##_##name);                                                   \
      __ztest_count++;                                                       \
    }                                                                        \
  }                                                                          \
  static void suite##_##name(void)

/* Suite registration (no-op in standalone, suites are implicit from ZTEST) */
#define ZTEST_SUITE(name, pred, setup, before, after, teardown) /* no-op */

/*
 * Assertion macros
 */

#define zassert_true(cond, fmt, ...)                                           \
  do {                                                                         \
    if (!(cond)) {                                                             \
      printf("    FAIL: %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
      __ztest_failed++;                                                        \
      return;                                                                  \
    }                                                                          \
  } while (0)

#define zassert_false(cond, fmt, ...) zassert_true(!(cond), fmt, ##__VA_ARGS__)

#define zassert_equal(a, b, fmt, ...)                                       \
  do {                                                                      \
    if ((a) != (b)) {                                                       \
      printf("    FAIL: %s:%d: expected %ld, got %ld: " fmt "\n", __FILE__, \
          __LINE__, (long)(b), (long)(a), ##__VA_ARGS__);                   \
      __ztest_failed++;                                                     \
      return;                                                               \
    }                                                                       \
  } while (0)

#define zassert_not_equal(a, b, fmt, ...)                                    \
  do {                                                                       \
    if ((a) == (b)) {                                                        \
      printf("    FAIL: %s:%d: values should not be equal (%ld): " fmt "\n", \
          __FILE__, __LINE__, (long)(a), ##__VA_ARGS__);                     \
      __ztest_failed++;                                                      \
      return;                                                                \
    }                                                                        \
  } while (0)

#define zassert_is_null(ptr, fmt, ...) \
  zassert_true((ptr) == NULL, fmt, ##__VA_ARGS__)

#define zassert_not_null(ptr, fmt, ...) \
  zassert_true((ptr) != NULL, fmt, ##__VA_ARGS__)

#define zassert_mem_equal(buf1, buf2, len, fmt, ...)                  \
  do {                                                                \
    if (memcmp((buf1), (buf2), (len)) != 0) {                         \
      printf("    FAIL: %s:%d: memory mismatch: " fmt "\n", __FILE__, \
          __LINE__, ##__VA_ARGS__);                                   \
      __ztest_failed++;                                               \
      return;                                                         \
    }                                                                 \
  } while (0)

#define zassert_ok(result, fmt, ...) zassert_equal(result, 0, fmt, ##__VA_ARGS__)

/*
 * Zephyr compatibility - printk
 */
#define printk printf

/*
 * Test runner (implemented in main.c)
 */
int ztest_run_all(void);

#endif /* ZTEST_COMPAT_H_ */
