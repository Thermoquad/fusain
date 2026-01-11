# Copyright (c) 2025 Kaz Walker, Thermoquad
# SPDX-License-Identifier: Apache-2.0
#
# Fusain Serial Protocol Library - Dual-Mode CMake Build
#
# This library can be built in two modes:
# 1. ZEPHYR MODE: As a Zephyr module (detected via ZEPHYR_BASE)
# 2. STANDALONE MODE: As a portable C library for any application
#
# CBOR encoding/decoding is provided by zcbor-generated code from fusain.cddl.
# Regenerate with: task zcbor-generate
#
cmake_minimum_required(VERSION 3.20.0)

# Detect Zephyr context BEFORE project() declaration
if(ZEPHYR_BASE)
  # ============================================================
  # ZEPHYR MODE - Build as Zephyr module
  # Uses Zephyr's native CRC (crc16_itu_t) and zcbor runtime.
  # ============================================================
  if(CONFIG_FUSAIN)
    set(ZEPHYR_CURRENT_LIBRARY fusain)
    zephyr_include_directories(${ZEPHYR_CURRENT_MODULE_DIR}/include)
    zephyr_include_directories(${ZEPHYR_CURRENT_MODULE_DIR}/src/generated)
    zephyr_library()
    zephyr_library_sources(
      ${ZEPHYR_CURRENT_MODULE_DIR}/src/fusain.c
      ${ZEPHYR_CURRENT_MODULE_DIR}/src/generated/fusain_cbor_encode.c
      ${ZEPHYR_CURRENT_MODULE_DIR}/src/generated/fusain_cbor_decode.c
    )
    if(CONFIG_FUSAIN_NET_BUF)
      zephyr_library_sources(
        ${ZEPHYR_CURRENT_MODULE_DIR}/src/fusain_net_buf.c
      )
    endif()
  endif()
else()
  # ============================================================
  # STANDALONE MODE - Build as portable C library
  # Uses built-in CRC and zcbor-generated CBOR code.
  # ============================================================
  project(fusain
    VERSION 1.0.0
    DESCRIPTION "Fusain Serial Protocol Library"
    LANGUAGES C
  )

  # Fetch zcbor for standalone builds (provides zcbor runtime)
  include(FetchContent)
  FetchContent_Declare(
    zcbor
    GIT_REPOSITORY https://github.com/NordicSemiconductor/zcbor.git
    GIT_TAG        0.9.1
    GIT_SHALLOW    TRUE
  )
  FetchContent_MakeAvailable(zcbor)

  # Library target
  add_library(fusain STATIC
    src/fusain.c
    src/generated/fusain_cbor_encode.c
    src/generated/fusain_cbor_decode.c
    ${zcbor_SOURCE_DIR}/src/zcbor_common.c
    ${zcbor_SOURCE_DIR}/src/zcbor_encode.c
    ${zcbor_SOURCE_DIR}/src/zcbor_decode.c
  )
  add_library(Fusain::fusain ALIAS fusain)

  # Include directories with generator expressions for build/install
  target_include_directories(fusain PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/generated>
    $<BUILD_INTERFACE:${zcbor_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
  )

  # Compiler warnings (GCC/Clang)
  target_compile_options(fusain PRIVATE
    $<$<C_COMPILER_ID:GNU>:-Wall -Wextra -pedantic>
    $<$<C_COMPILER_ID:Clang>:-Wall -Wextra -pedantic>
    $<$<C_COMPILER_ID:AppleClang>:-Wall -Wextra -pedantic>
  )

  # C standard
  target_compile_features(fusain PUBLIC c_std_99)

  # ============================================================
  # Installation and Export (for find_package support)
  # ============================================================
  include(GNUInstallDirs)
  include(CMakePackageConfigHelpers)

  # Install library
  install(TARGETS fusain
    EXPORT fusainTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  )

  # Install headers
  install(DIRECTORY include/fusain
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )

  # Export targets for find_package
  install(EXPORT fusainTargets
    FILE fusainTargets.cmake
    NAMESPACE Fusain::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/fusain
  )

  # Generate package config file
  configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/fusainConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/fusainConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/fusain
  )

  # Generate version file
  write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/fusainConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
  )

  # Install config files
  install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/fusainConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/fusainConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/fusain
  )

  # ============================================================
  # Testing (standalone mode only)
  # ============================================================
  option(FUSAIN_BUILD_TESTS "Build fusain tests" OFF)
  if(FUSAIN_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests/standalone)
  endif()
endif()
