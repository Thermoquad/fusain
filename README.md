# Fusain Protocol Library

Reusable C library for encoding and decoding Fusain serial protocol packets.

**Fusain** (fossilized charcoal) provides a platform-independent implementation of the Fusain communication protocol.

## Features

- **CBOR payload encoding** (RFC 8949) using zcbor-generated code
- **CRC-16-CCITT** error detection (polynomial 0x1021, initial 0xFFFF)
- **Byte stuffing** for packet framing
- **Stateful decoder** for streaming byte-by-byte decoding
- **64-bit addressing** for multi-device networks
- **Pure C implementation** - zcbor runtime (auto-fetched in standalone mode)
- **Portable** - works on embedded systems and host applications

## Protocol Overview

- **Framing:** START (0x7E) ... END (0x7F) with escape byte (0x7D) for byte stuffing
- **Structure:** START + LENGTH + ADDRESS(8) + CBOR_PAYLOAD + CRC(2) + END
- **CBOR payload:** `[msg_type, payload_map]` or `[msg_type, nil]` for empty payloads
- **Max packet size:** 128 bytes (13 overhead + CBOR payload)
- **Max CBOR payload size:** 114 bytes
- **Message types:** Configuration, Control, Telemetry, Errors
- **Encoding:** CBOR (RFC 8949) using zcbor-generated code

## Build Modes

This library supports two build modes:

### 1. Zephyr Mode (Embedded)

When built within a Zephyr environment, the library is automatically detected as a Zephyr module.

**Enable in prj.conf:**
```kconfig
CONFIG_FUSAIN=y
```

**Add module to application CMakeLists.txt:**
```cmake
list(APPEND EXTRA_ZEPHYR_MODULES ${CMAKE_CURRENT_SOURCE_DIR}/../../modules/lib/fusain)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
```

### 2. Standalone Mode (Desktop/Other Platforms)

For non-Zephyr applications, build as a standard CMake library:

**Build the library:**
```bash
cmake -B build
cmake --build build
```

**Install (optional):**
```bash
cmake --install build --prefix /usr/local
```

**Use with find_package:**
```cmake
find_package(fusain REQUIRED)
target_link_libraries(myapp PRIVATE Fusain::fusain)
```

**Or add as subdirectory:**
```cmake
add_subdirectory(path/to/fusain)
target_link_libraries(myapp PRIVATE Fusain::fusain)
```

## Usage

### Include Header

```c
#include <fusain/fusain.h>
```

### Encoding Example

```c
fusain_packet_t packet;
uint8_t tx_buffer[FUSAIN_MAX_PACKET_SIZE * 2];
uint64_t device_address = 0x0123456789ABCDEF;

// Create a ping request
fusain_create_ping_request(&packet, device_address);

// Encode to byte buffer
int len = fusain_encode_packet(&packet, tx_buffer, sizeof(tx_buffer));
if (len < 0) {
    // Handle encoding error
}

// Send via UART (application-specific)
uart_send(tx_buffer, len);
```

### Decoding Example

```c
// Decoder state (persistent)
fusain_decoder_t decoder;

// Initialize decoder
fusain_reset_decoder(&decoder);

// Process incoming bytes
while (uart_has_data()) {
    uint8_t byte = uart_read_byte();
    fusain_packet_t packet;

    fusain_decode_result_t result = fusain_decode_byte(byte, &packet, &decoder);

    if (result == FUSAIN_DECODE_OK) {
        // Packet complete and valid
        process_packet(&packet);
    } else if (result != FUSAIN_DECODE_INCOMPLETE) {
        // Decode error - reset decoder
        fusain_reset_decoder(&decoder);
    }
}
```

## API Reference

### Core Functions

**CRC Calculation:**
```c
uint16_t fusain_crc16(const uint8_t* data, size_t length);
```

**Packet Encoding:**
```c
int fusain_encode_packet(const fusain_packet_t* packet, uint8_t* buffer, size_t buffer_size);
```
Returns number of bytes written, or negative error code.

**Packet Decoding:**
```c
fusain_decode_result_t fusain_decode_byte(
    uint8_t rx_byte,
    fusain_packet_t* packet,
    fusain_decoder_t* decoder
);
```
Returns: `FUSAIN_DECODE_OK`, `FUSAIN_DECODE_INCOMPLETE`, or error code.

**Decoder Reset:**
```c
void fusain_reset_decoder(fusain_decoder_t* decoder);
```

### Helper Functions

The library provides helper functions for creating common message types:

**Control Commands:**
- `fusain_create_state_command()` - Set operating mode command
- `fusain_create_pump_command()` - Set fuel pump rate command
- `fusain_create_motor_command()` - Set target RPM command
- `fusain_create_glow_command()` - Glow plug control command
- `fusain_create_temp_command()` - Temperature control command
- `fusain_create_ping_request()` - Ping request (heartbeat)
- `fusain_create_send_telemetry()` - Request telemetry (polling mode)

**Configuration Commands:**
- `fusain_create_motor_config()` - Configure motor controller
- `fusain_create_pump_config()` - Configure pump controller
- `fusain_create_temp_config()` - Configure temperature controller
- `fusain_create_glow_config()` - Configure glow plug
- `fusain_create_telemetry_config()` - Configure telemetry broadcasts
- `fusain_create_timeout_config()` - Configure communication timeout
- `fusain_create_data_subscription()` - Subscribe to appliance data
- `fusain_create_data_unsubscribe()` - Unsubscribe from appliance data
- `fusain_create_discovery_request()` - Request device capabilities

**Data Messages:**
- `fusain_create_state_data()` - State and error data
- `fusain_create_ping_response()` - Ping response (heartbeat)
- `fusain_create_device_announce()` - Device capabilities announcement

See `include/fusain/fusain.h` for complete API documentation.

## Message Types

### Configuration Commands (0x10-0x1F)
- `FUSAIN_MSG_MOTOR_CONFIG` - Configure motor controller parameters
- `FUSAIN_MSG_PUMP_CONFIG` - Configure pump controller parameters
- `FUSAIN_MSG_TEMP_CONFIG` - Configure temperature controller parameters
- `FUSAIN_MSG_GLOW_CONFIG` - Configure glow plug parameters
- `FUSAIN_MSG_DATA_SUBSCRIPTION` - Subscribe to appliance data
- `FUSAIN_MSG_DATA_UNSUBSCRIBE` - Unsubscribe from appliance data
- `FUSAIN_MSG_TELEMETRY_CONFIG` - Enable/disable telemetry broadcasts
- `FUSAIN_MSG_TIMEOUT_CONFIG` - Configure communication timeout
- `FUSAIN_MSG_DISCOVERY_REQUEST` - Request device capabilities

### Control Commands (0x20-0x2F)
- `FUSAIN_MSG_STATE_COMMAND` - Set system mode/state
- `FUSAIN_MSG_MOTOR_COMMAND` - Set motor RPM
- `FUSAIN_MSG_PUMP_COMMAND` - Set pump rate
- `FUSAIN_MSG_GLOW_COMMAND` - Control glow plug
- `FUSAIN_MSG_TEMP_COMMAND` - Temperature controller control
- `FUSAIN_MSG_SEND_TELEMETRY` - Request telemetry (polling mode)
- `FUSAIN_MSG_PING_REQUEST` - Heartbeat/connectivity check

### Telemetry Data (0x30-0x3F)
- `FUSAIN_MSG_STATE_DATA` - System state and status
- `FUSAIN_MSG_MOTOR_DATA` - Motor telemetry
- `FUSAIN_MSG_PUMP_DATA` - Pump status and events
- `FUSAIN_MSG_GLOW_DATA` - Glow plug status
- `FUSAIN_MSG_TEMP_DATA` - Temperature readings
- `FUSAIN_MSG_DEVICE_ANNOUNCE` - Device capabilities announcement
- `FUSAIN_MSG_PING_RESPONSE` - Heartbeat response

### Error Messages (0xE0-0xEF)
- `FUSAIN_MSG_ERROR_INVALID_CMD` - Command validation failed
- `FUSAIN_MSG_ERROR_STATE_REJECT` - Command rejected by state machine

## Architecture

The library is designed for controller/appliance communication:
- **Controller** - Sends commands, receives telemetry
- **Appliance** (ICU firmware) - Receives commands, sends telemetry

### Thread Safety

The encoder (`fusain_encode_packet()`) is stateless and thread-safe.

The decoder (`fusain_decode_byte()`) requires per-connection state and is NOT thread-safe. Use separate decoder instances for each connection or protect with mutexes.

### Platform Independence

The library uses standard C types and functions:
- `<stdbool.h>`, `<stddef.h>`, `<stdint.h>`, `<string.h>`
- No dynamic allocation
- No platform-specific code

**Dependencies:**
- **Standalone mode:** zcbor runtime (auto-fetched via CMake FetchContent)
- **Zephyr mode:** Uses Zephyr's built-in zcbor (`CONFIG_ZCBOR`)

Can be compiled for:
- Embedded systems (ARM, RISC-V, etc.)
- Desktop applications (Linux, macOS, Windows)
- Bare metal or RTOS environments

## Testing

The library includes comprehensive tests:
- Functional tests (encoding, decoding, packet creation)
- Fuzz tests (random data, edge cases)

### Zephyr Tests (with Twister)

```bash
task test              # Run all tests with Twister
task test-functional   # Run functional tests only
task test-fuzz         # Run fuzz tests only
task test -- 5000      # Run with custom fuzz round count
```

### Standalone Tests (no Zephyr required)

```bash
task standalone-test          # Run all standalone tests
task standalone-test-verbose  # Run with detailed output
task standalone-coverage      # Run with coverage report (requires gcovr)
task standalone-ci            # Run format check + tests
```

### Coverage

Standalone tests achieve **100% code coverage**. Generate a report with:

```bash
task standalone-coverage
```

Requires `gcovr` (`pip install gcovr`).

## Integration with Applications

### Appliance Firmware (Zephyr)
The ICU-specific UART handler integrates this library with:
- Zephyr UART drivers (interrupt-driven RX, polling TX)
- Zbus message bus for inter-thread communication
- Timeout mode for safety (auto-idle on communication loss)

See `apps/helios/src/communications/serial_handler.c` for reference implementation.

### Controller Application
Controller implementations should:
- Use this library for protocol encoding/decoding
- Implement UART/serial communication (platform-specific)
- Send periodic PING_REQUEST to maintain connection
- Handle telemetry messages for monitoring

## License

Apache-2.0

Copyright (c) 2025 Kaz Walker, Thermoquad

## References

- **Helios ICU Firmware:** `apps/helios/` - Reference implementation
- **Protocol Specification:** `origin/documentation/source/specifications/fusain/`
- **CRC-16-CCITT:** Polynomial 0x1021, initial value 0xFFFF
