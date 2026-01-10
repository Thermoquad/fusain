# Fusain Protocol Library - Test Suite

Comprehensive test suite for the Fusain protocol library using Zephyr's Twister framework.

## Test Coverage

### Functional Tests (43 test cases)
Functional tests have fixed test cases and do not use configurable round counts.

- **CRC Calculation** (6 tests)
  - Known value verification
  - Empty data handling
  - Determinism verification
  - Various payload lengths
  - Byte order independence

- **Packet Encoding** (6 tests)
  - Basic encoding
  - Empty payload
  - Maximum payload size
  - Byte stuffing
  - Error handling
  - Deterministic encoding

- **Packet Decoding** (8 tests)
  - Round-trip encoding/decoding
  - Empty payload decoding
  - Byte stuffing/unstuffing
  - CRC error detection
  - Decoder reset
  - Maximum payload
  - Various payload size consistency

- **Packet Creation** (16 tests)
  - v1.x command packets (6 tests)
  - v2.0 config command packets (7 tests)
  - Data packets (3 tests)

- **Message Types Tested**
  - Commands: STATE_COMMAND, PUMP_COMMAND, MOTOR_COMMAND, GLOW_COMMAND, TEMP_COMMAND, PING_REQUEST, SEND_TELEMETRY
  - Config: MOTOR_CONFIG, PUMP_CONFIG, TEMP_CONFIG, GLOW_CONFIG, DATA_SUBSCRIPTION/UNSUBSCRIBE, TELEMETRY_CONFIG, TIMEOUT_CONFIG, DISCOVERY_REQUEST
  - Data: STATE_DATA, MOTOR_DATA, TEMP_DATA, PUMP_DATA, GLOW_DATA, PING_RESPONSE, DEVICE_ANNOUNCE

### Fuzz Tests (43 test cases)
Fuzz tests use configurable round counts via `task test-fuzz -- <rounds>`.

- **Encoding Fuzzing**
  - Random payloads
  - Invalid lengths
  - Edge cases

- **Decoding Fuzzing**
  - Random byte streams
  - Error detection
  - Recovery testing

- **Round-trip Fuzzing**
  - Random valid packets
  - Data integrity verification

- **CRC Fuzzing**
  - Random data
  - Determinism verification
  - Bit flip detection

- **Byte Stuffing Fuzzing**
  - Special byte injection
  - Forced stuffing scenarios

- **Decoder Error Recovery**
  - Corrupted packets
  - State machine recovery

- **v2.0 Packet Fuzzing**
  - All config command types
  - Random parameter values

## Running Tests

### Run All Tests
```bash
task test                     # Default: 100 fuzz rounds
task test -- 420              # Custom fuzz round count
```

### Run Only Functional Tests
```bash
task test-functional          # Fixed test cases, no round count
```

### Run Only Fuzz Tests
```bash
task test-fuzz                # Default: 1000 rounds
task test-fuzz -- 5000        # Custom round count
```

## Test Configuration

**Functional tests** use fixed test cases and do not have configurable round counts.

**Fuzz test** round counts can be configured via:
1. **Command line:** `task test-fuzz -- <rounds>`
2. **prj.conf:** Edit `CONFIG_FUSAIN_TEST_FUZZ_ROUNDS`
3. **Kconfig:** Use menuconfig to adjust the value

## Test Results

Example output with 420 rounds:
```
INFO - 2 of 2 executed test configurations passed (100.00%)
INFO - 86 of 86 executed test cases passed (100.00%)
INFO - Test run completed in ~5 seconds
```

## Test Files

- `src/test_crc.c` - CRC calculation tests
- `src/test_encoding.c` - Packet encoding tests
- `src/test_decoding.c` - Packet decoding tests
- `src/test_packet_creation.c` - Packet creation function tests
- `src/test_fuzz.c` - Fuzzing tests with random values

## Platform Support

Tests run on:
- `native_sim` (primary)
- `qemu_cortex_m3` (functional tests only)

## CI Integration

The test suite is designed for CI/CD integration:
- Fast execution (~5 seconds for default rounds)
- Deterministic results
- JUnit XML report generation
- JSON test report output

## Adding New Tests

1. Create test functions using `ZTEST()` macro
2. Group tests in suites using `ZTEST_SUITE()`
3. Tests are auto-discovered by ztest framework
4. **Functional tests:** Use fixed test cases
5. **Fuzz tests:** Use `CONFIG_FUSAIN_TEST_FUZZ_ROUNDS` for iteration counts

Example (functional test):
```c
ZTEST(fusain_protocol, test_my_feature)
{
    // Fixed test case
    fusain_packet_t packet = {...};
    int result = fusain_encode_packet(&packet, ...);
    zassert_true(result > 0, "Should succeed");
}

ZTEST_SUITE(fusain_protocol, NULL, NULL, NULL, NULL, NULL);
```

Example (fuzz test):
```c
ZTEST(fusain_fuzz, test_random_payloads)
{
    for (int i = 0; i < CONFIG_FUSAIN_TEST_FUZZ_ROUNDS; i++) {
        // Generate random data and test
    }
}

ZTEST_SUITE(fusain_fuzz, NULL, NULL, NULL, NULL, NULL);
```
