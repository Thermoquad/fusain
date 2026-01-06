# Fusain Utility Library

Reusable C library for encoding and decoding Fusain serial protocol packets.

**Fusain** (fossilized charcoal) provides a platform-independent implementation of the Fusain communication protocol.

## Features

- **CRC-16-CCITT** error detection (polynomial 0x1021, initial 0xFFFF)
- **Byte stuffing** for packet framing
- **Stateful decoder** for streaming byte-by-byte decoding
- **Pure C implementation** - zero dependencies beyond standard C library
- **Portable** - works on embedded systems and host applications
- **No Zephyr dependencies** - can be used outside Zephyr RTOS

## Protocol Overview

- **Framing:** START (0x7E) ... END (0x7F) with escape byte (0x7D) for byte stuffing
- **Structure:** START + LENGTH + TYPE + PAYLOAD + CRC(2 bytes) + END
- **Max packet size:** 64 bytes (including overhead)
- **Max payload size:** 58 bytes
- **Message types:** Commands (master → ICU), Data (ICU → master), Errors (bidirectional)

## Usage

### Enable in Kconfig

```kconfig
CONFIG_FUSAIN=y
```

### Include Header

```c
#include <fusain/fusain.h>
```

### Encoding Example

```c
fusain_packet_t packet;
uint8_t tx_buffer[FUSAIN_MAX_PACKET_SIZE * 2];

// Create a ping request
fusain_create_ping_request(&packet);

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

- `fusain_create_set_mode()` - Set operating mode command
- `fusain_create_set_pump_rate()` - Set fuel pump rate command
- `fusain_create_set_target_rpm()` - Set target RPM command
- `fusain_create_ping_request()` - Ping request (heartbeat)
- `fusain_create_set_timeout_config()` - Configure timeout mode
- `fusain_create_emergency_stop()` - Emergency stop command
- `fusain_create_state_data()` - State and error data
- `fusain_create_ping_response()` - Ping response (heartbeat)
- `fusain_create_telemetry_bundle()` - Aggregated telemetry data

See `include/fusain/fusain.h` for complete API documentation.

## Message Types

### Commands (Master → ICU)
- `FUSAIN_MSG_SET_MODE` - Set operating mode (idle, fan, heat, emergency)
- `FUSAIN_MSG_SET_PUMP_RATE` - Set fuel pump rate
- `FUSAIN_MSG_SET_TARGET_RPM` - Set target motor RPM
- `FUSAIN_MSG_PING_REQUEST` - Keepalive ping
- `FUSAIN_MSG_SET_TIMEOUT_CONFIG` - Configure timeout behavior
- `FUSAIN_MSG_EMERGENCY_STOP` - Immediate emergency stop

### Data (ICU → Master)
- `FUSAIN_MSG_STATE_DATA` - Current state and error code
- `FUSAIN_MSG_MOTOR_DATA` - Motor telemetry (RPM, PWM, etc.)
- `FUSAIN_MSG_TEMPERATURE_DATA` - Temperature and PID status
- `FUSAIN_MSG_PUMP_DATA` - Pump status and pulse count
- `FUSAIN_MSG_GLOW_DATA` - Glow plug status
- `FUSAIN_MSG_TELEMETRY_BUNDLE` - Aggregated telemetry (multiple motors/temps)
- `FUSAIN_MSG_PING_RESPONSE` - Ping response with uptime

### Errors (Bidirectional)
- `FUSAIN_MSG_ERROR_INVALID_COMMAND` - Unrecognized command
- `FUSAIN_MSG_ERROR_INVALID_CRC` - CRC mismatch
- `FUSAIN_MSG_ERROR_INVALID_LENGTH` - Payload length error
- `FUSAIN_MSG_ERROR_TIMEOUT` - Communication timeout

## Architecture

The library is designed for master/slave communication:
- **Master** (controller) - Sends commands, receives data
- **Slave** (ICU firmware) - Receives commands, sends data

### Thread Safety

The encoder (`fusain_encode_packet()`) is stateless and thread-safe.

The decoder (`fusain_decode_byte()`) requires per-connection state and is NOT thread-safe. Use separate decoder instances for each connection or protect with mutexes.

### Platform Independence

The library uses only standard C types and functions:
- `<stdbool.h>`, `<stddef.h>`, `<stdint.h>`, `<string.h>`
- No dynamic allocation
- No platform-specific code
- No Zephyr dependencies

Can be compiled for:
- Embedded systems (ARM, RISC-V, etc.)
- Desktop applications (Linux, macOS, Windows)
- Bare metal or RTOS environments

## Testing

The library has been tested with:
- Raspberry Pi Pico 2 (RP2350A, Cortex-M33)
- Zephyr RTOS v4.3.0+
- 115200 baud UART communication
- Extended burn testing (Helios ICU firmware)

## Integration with Applications

### ICU Firmware (Zephyr)
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
- Handle TELEMETRY_BUNDLE messages for monitoring

## License

Apache-2.0

Copyright (c) 2025 Kaz Walker, Thermoquad

## References

- **Helios ICU Firmware:** `apps/helios/` - Reference implementation
- **Protocol Documentation:** `apps/helios/docs/serial_protocol.md`
- **CRC-16-CCITT:** Polynomial 0x1021, initial value 0xFFFF
