# Copyright (c) 2025 Kaz Walker, Thermoquad
# SPDX-License-Identifier: Apache-2.0
#
# Fusain Standalone Tests
#
# Build with: cmake -B build -DFUSAIN_BUILD_TESTS=ON && cmake --build build && ctest --test-dir build
#

# Fuzz test configuration
option(FUSAIN_FUZZ_ENABLED "Enable fuzz tests" ON)
set(FUSAIN_FUZZ_ROUNDS "1000" CACHE STRING "Number of fuzz test rounds")
set(FUSAIN_FUZZ_SEED "0" CACHE STRING "Fuzz test seed (0 = random)")

# Test executable
add_executable(fusain_tests
  main.c
  ../src/test_crc.c
  ../src/test_encoding.c
  ../src/test_decoding.c
  ../src/test_packet_creation.c
  $<$<BOOL:${FUSAIN_FUZZ_ENABLED}>:../src/test_fuzz.c>
)

# Link against fusain library
target_link_libraries(fusain_tests PRIVATE fusain)

# Include standalone test headers (for ztest compatibility)
target_include_directories(fusain_tests PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

# Fuzz test Kconfig compatibility
target_compile_definitions(fusain_tests PRIVATE
  CONFIG_FUSAIN_TEST_FUZZ_ROUNDS=${FUSAIN_FUZZ_ROUNDS}
  CONFIG_FUSAIN_TEST_FUZZ_SEED=${FUSAIN_FUZZ_SEED}
)

# Compiler warnings as errors
target_compile_options(fusain_tests PRIVATE
  $<$<C_COMPILER_ID:GNU>:-Wall -Wextra -Werror>
  $<$<C_COMPILER_ID:Clang>:-Wall -Wextra -Werror>
  $<$<C_COMPILER_ID:AppleClang>:-Wall -Wextra -Werror>
)

# Register as CTest
add_test(NAME fusain_tests COMMAND fusain_tests)
