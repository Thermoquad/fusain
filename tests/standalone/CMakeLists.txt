# Copyright (c) 2025 Kaz Walker, Thermoquad
# SPDX-License-Identifier: Apache-2.0
#
# Fusain Standalone Tests
#
# Build with: cmake -B build -DFUSAIN_BUILD_TESTS=ON && cmake --build build && ctest --test-dir build
#

# Test executable
add_executable(fusain_tests
  main.c
  ../src/test_crc.c
  ../src/test_encoding.c
  ../src/test_decoding.c
  ../src/test_packet_creation.c
)

# Link against fusain library
target_link_libraries(fusain_tests PRIVATE fusain)

# Include standalone test headers (for ztest compatibility)
target_include_directories(fusain_tests PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

# Compiler warnings
target_compile_options(fusain_tests PRIVATE
  $<$<C_COMPILER_ID:GNU>:-Wall -Wextra>
  $<$<C_COMPILER_ID:Clang>:-Wall -Wextra>
  $<$<C_COMPILER_ID:AppleClang>:-Wall -Wextra>
)

# Register as CTest
add_test(NAME fusain_tests COMMAND fusain_tests)
