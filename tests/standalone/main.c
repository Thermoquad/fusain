/*
 * Copyright (c) 2025 Kaz Walker, Thermoquad
 * SPDX-License-Identifier: Apache-2.0
 *
 * Fusain Standalone Test Runner
 *
 * This file provides the main() entry point for running Fusain tests
 * outside of Zephyr. Tests are automatically registered via constructor
 * attributes and run sequentially.
 */

#include <stdio.h>
#include <string.h>
#include <zephyr/ztest.h>

/* Global test registry */
struct ztest_entry __ztest_registry[ZTEST_MAX_TESTS];
int __ztest_count = 0;

/* Test result tracking */
int __ztest_passed = 0;
int __ztest_failed = 0;
const char* __ztest_current_suite = NULL;
const char* __ztest_current_test = NULL;

/* Run all registered tests */
int ztest_run_all(void)
{
  const char* current_suite = NULL;

  printf("Running %d tests...\n\n", __ztest_count);

  for (int i = 0; i < __ztest_count; i++) {
    struct ztest_entry* entry = &__ztest_registry[i];

    /* Print suite header on suite change */
    if (current_suite == NULL || strcmp(current_suite, entry->suite) != 0) {
      if (current_suite != NULL) {
        printf("\n");
      }
      printf("Suite: %s\n", entry->suite);
      current_suite = entry->suite;
    }

    /* Run test */
    __ztest_current_suite = entry->suite;
    __ztest_current_test = entry->name;

    int failed_before = __ztest_failed;
    printf("  %s... ", entry->name);
    fflush(stdout);

    entry->func();

    if (__ztest_failed == failed_before) {
      printf("PASS\n");
      __ztest_passed++;
    } else {
      /* Failure message already printed by assertion */
    }
  }

  /* Print summary */
  printf("\n========================================\n");
  printf("Results: %d passed, %d failed, %d total\n", __ztest_passed,
      __ztest_failed, __ztest_passed + __ztest_failed);
  printf("========================================\n");

  return __ztest_failed > 0 ? 1 : 0;
}

int main(int argc, char* argv[])
{
  (void)argc;
  (void)argv;

  printf("Fusain Protocol Library - Standalone Tests\n");
  printf("==========================================\n\n");

  return ztest_run_all();
}
