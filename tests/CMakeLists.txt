# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

# Add fusain module to the build
list(APPEND EXTRA_ZEPHYR_MODULES ${CMAKE_CURRENT_SOURCE_DIR}/..)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(fusain_tests)

# Add test source files
target_sources(app PRIVATE
  src/main.c
  src/test_crc.c
  src/test_encoding.c
  src/test_decoding.c
  src/test_packet_creation.c
  src/test_fuzz.c
)

# Add test include directory
target_include_directories(app PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)
