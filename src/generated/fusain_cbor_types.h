/*
 * Generated using zcbor version 0.9.1
 * https://github.com/NordicSemiconductor/zcbor
 * Generated with a --default-max-qty of 3
 */

#ifndef FUSAIN_CBOR_TYPES_H__
#define FUSAIN_CBOR_TYPES_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif

/** Which value for --default-max-qty this file was created with.
 *
 *  The define is used in the other generated file to do a build-time
 *  compatibility check.
 *
 *  See `zcbor --help` for more information about --default-max-qty
 */
#define DEFAULT_MAX_QTY 3

struct telemetry_config_payload {
	bool telemetry_config_payload_uint0bool;
	uint32_t telemetry_config_payload_uint1uint;
};

struct timeout_config_payload {
	bool timeout_config_payload_uint0bool;
	uint32_t timeout_config_payload_uint1uint;
};

struct device_announce_payload {
	uint32_t device_announce_payload_uint0uint;
	uint32_t device_announce_payload_uint1uint;
	uint32_t device_announce_payload_uint2uint;
	uint32_t device_announce_payload_uint3uint;
};

struct error_invalid_cmd_payload {
	int32_t error_invalid_cmd_payload_uint0int;
};

struct motor_config_payload_uint1uint {
	uint32_t motor_config_payload_uint1uint;
};

struct motor_config_payload_uint2float {
	double motor_config_payload_uint2float;
};

struct motor_config_payload_uint3float {
	double motor_config_payload_uint3float;
};

struct motor_config_payload_uint4float {
	double motor_config_payload_uint4float;
};

struct motor_config_payload_uint5int {
	int32_t motor_config_payload_uint5int;
};

struct motor_config_payload_uint6int {
	int32_t motor_config_payload_uint6int;
};

struct motor_config_payload_uint7uint {
	uint32_t motor_config_payload_uint7uint;
};

struct motor_config_payload {
	uint32_t motor_config_payload_motor_index_m;
	struct motor_config_payload_uint1uint motor_config_payload_uint1uint;
	bool motor_config_payload_uint1uint_present;
	struct motor_config_payload_uint2float motor_config_payload_uint2float;
	bool motor_config_payload_uint2float_present;
	struct motor_config_payload_uint3float motor_config_payload_uint3float;
	bool motor_config_payload_uint3float_present;
	struct motor_config_payload_uint4float motor_config_payload_uint4float;
	bool motor_config_payload_uint4float_present;
	struct motor_config_payload_uint5int motor_config_payload_uint5int;
	bool motor_config_payload_uint5int_present;
	struct motor_config_payload_uint6int motor_config_payload_uint6int;
	bool motor_config_payload_uint6int_present;
	struct motor_config_payload_uint7uint motor_config_payload_uint7uint;
	bool motor_config_payload_uint7uint_present;
};

struct pump_config_payload_uint1uint {
	uint32_t pump_config_payload_uint1uint;
};

struct pump_config_payload_uint2uint {
	uint32_t pump_config_payload_uint2uint;
};

struct pump_config_payload {
	uint32_t pump_config_payload_pump_index_m;
	struct pump_config_payload_uint1uint pump_config_payload_uint1uint;
	bool pump_config_payload_uint1uint_present;
	struct pump_config_payload_uint2uint pump_config_payload_uint2uint;
	bool pump_config_payload_uint2uint_present;
};

struct temp_config_payload_uint1float {
	double temp_config_payload_uint1float;
};

struct temp_config_payload_uint2float {
	double temp_config_payload_uint2float;
};

struct temp_config_payload_uint3float {
	double temp_config_payload_uint3float;
};

struct temp_config_payload {
	uint32_t temp_config_payload_thermometer_index_m;
	struct temp_config_payload_uint1float temp_config_payload_uint1float;
	bool temp_config_payload_uint1float_present;
	struct temp_config_payload_uint2float temp_config_payload_uint2float;
	bool temp_config_payload_uint2float_present;
	struct temp_config_payload_uint3float temp_config_payload_uint3float;
	bool temp_config_payload_uint3float_present;
};

struct glow_config_payload_uint1uint {
	uint32_t glow_config_payload_uint1uint;
};

struct glow_config_payload {
	uint32_t glow_config_payload_glow_index_m;
	struct glow_config_payload_uint1uint glow_config_payload_uint1uint;
	bool glow_config_payload_uint1uint_present;
};

struct data_subscription_payload {
	uint64_t data_subscription_payload_address_m;
};

struct state_command_payload_uint1int {
	int32_t state_command_payload_uint1int;
};

struct state_command_payload {
	uint32_t state_command_payload_mode_m;
	struct state_command_payload_uint1int state_command_payload_uint1int;
	bool state_command_payload_uint1int_present;
};

struct motor_command_payload {
	uint32_t motor_command_payload_motor_index_m;
	int32_t motor_command_payload_uint1int;
};

struct pump_command_payload {
	uint32_t pump_command_payload_pump_index_m;
	int32_t pump_command_payload_uint1int;
};

struct glow_command_payload {
	uint32_t glow_command_payload_glow_index_m;
	int32_t glow_command_payload_uint1int;
};

struct temp_command_payload_motor_index_m {
	uint32_t temp_command_payload_motor_index_m;
};

struct temp_command_payload_uint3float {
	double temp_command_payload_uint3float;
};

struct temp_command_payload {
	uint32_t temp_command_payload_thermometer_index_m;
	uint32_t temp_command_payload_temp_cmd_type_m;
	struct temp_command_payload_motor_index_m temp_command_payload_motor_index_m;
	bool temp_command_payload_motor_index_m_present;
	struct temp_command_payload_uint3float temp_command_payload_uint3float;
	bool temp_command_payload_uint3float_present;
};

struct send_telemetry_payload_uint1uint {
	uint32_t send_telemetry_payload_uint1uint;
};

struct send_telemetry_payload {
	uint32_t send_telemetry_payload_telemetry_type_m;
	struct send_telemetry_payload_uint1uint send_telemetry_payload_uint1uint;
	bool send_telemetry_payload_uint1uint_present;
};

struct state_data_payload {
	bool state_data_payload_uint0bool;
	uint32_t state_data_payload_error_code_m;
	uint32_t state_data_payload_state_m;
	uint32_t state_data_payload_timestamp_m;
};

struct motor_data_payload_uint4int {
	int32_t motor_data_payload_uint4int;
};

struct motor_data_payload_uint5int {
	int32_t motor_data_payload_uint5int;
};

struct motor_data_payload_uint6uint {
	uint32_t motor_data_payload_uint6uint;
};

struct motor_data_payload_uint7uint {
	uint32_t motor_data_payload_uint7uint;
};

struct motor_data_payload {
	uint32_t motor_data_payload_motor_index_m;
	uint32_t motor_data_payload_timestamp_m;
	int32_t motor_data_payload_uint2int;
	int32_t motor_data_payload_uint3int;
	struct motor_data_payload_uint4int motor_data_payload_uint4int;
	bool motor_data_payload_uint4int_present;
	struct motor_data_payload_uint5int motor_data_payload_uint5int;
	bool motor_data_payload_uint5int_present;
	struct motor_data_payload_uint6uint motor_data_payload_uint6uint;
	bool motor_data_payload_uint6uint_present;
	struct motor_data_payload_uint7uint motor_data_payload_uint7uint;
	bool motor_data_payload_uint7uint_present;
};

struct pump_data_payload_uint3int {
	int32_t pump_data_payload_uint3int;
};

struct pump_data_payload {
	uint32_t pump_data_payload_pump_index_m;
	uint32_t pump_data_payload_timestamp_m;
	uint32_t pump_data_payload_pump_event_m;
	struct pump_data_payload_uint3int pump_data_payload_uint3int;
	bool pump_data_payload_uint3int_present;
};

struct glow_data_payload {
	uint32_t glow_data_payload_glow_index_m;
	uint32_t glow_data_payload_timestamp_m;
	bool glow_data_payload_uint2bool;
};

struct temp_data_payload_uint3bool {
	bool temp_data_payload_uint3bool;
};

struct temp_data_payload_uint4int {
	int32_t temp_data_payload_uint4int;
};

struct temp_data_payload_uint5float {
	double temp_data_payload_uint5float;
};

struct temp_data_payload {
	uint32_t temp_data_payload_thermometer_index_m;
	uint32_t temp_data_payload_timestamp_m;
	double temp_data_payload_uint2float;
	struct temp_data_payload_uint3bool temp_data_payload_uint3bool;
	bool temp_data_payload_uint3bool_present;
	struct temp_data_payload_uint4int temp_data_payload_uint4int;
	bool temp_data_payload_uint4int_present;
	struct temp_data_payload_uint5float temp_data_payload_uint5float;
	bool temp_data_payload_uint5float_present;
};

struct ping_response_payload {
	uint32_t ping_response_payload_timestamp_m;
};

struct error_state_reject_payload {
	uint32_t error_state_reject_payload_state_m;
};

#ifdef __cplusplus
}
#endif

#endif /* FUSAIN_CBOR_TYPES_H__ */
