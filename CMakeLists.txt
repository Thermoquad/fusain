# Copyright (c) 2025 Kaz Walker, Thermoquad
# SPDX-License-Identifier: Apache-2.0

if(CONFIG_HELIOS_SERIAL)

# Make include directory available to all targets
zephyr_include_directories(${ZEPHYR_CURRENT_MODULE_DIR}/include)

# Create library and add sources
zephyr_library()
zephyr_library_sources(${ZEPHYR_CURRENT_MODULE_DIR}/src/helios_serial.c)

endif() # CONFIG_HELIOS_SERIAL
