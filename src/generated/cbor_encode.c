/*
 * Generated using zcbor version 0.9.1
 * https://github.com/NordicSemiconductor/zcbor
 * Generated with a --default-max-qty of 3
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <zcbor_encode.h>
#include <zcbor_print.h>
#include <fusain/generated/cbor_encode.h>

#if DEFAULT_MAX_QTY != 3
#error "The type file was generated with a different default_max_qty than this file"
#endif

#define log_result(state, result, func) do { \
	if (!result) { \
		zcbor_trace_file(state); \
		zcbor_log("%s error: %s\r\n", func, zcbor_error_str(zcbor_peek_error(state))); \
	} else { \
		zcbor_log("%s success\r\n", func); \
	} \
} while(0)

static bool encode_repeated_motor_config_payload_uint1uint(zcbor_state_t *state, const struct motor_config_payload_uint1uint *input);
static bool encode_repeated_motor_config_payload_uint2float(zcbor_state_t *state, const struct motor_config_payload_uint2float *input);
static bool encode_repeated_motor_config_payload_uint3float(zcbor_state_t *state, const struct motor_config_payload_uint3float *input);
static bool encode_repeated_motor_config_payload_uint4float(zcbor_state_t *state, const struct motor_config_payload_uint4float *input);
static bool encode_repeated_motor_config_payload_uint5int(zcbor_state_t *state, const struct motor_config_payload_uint5int *input);
static bool encode_repeated_motor_config_payload_uint6int(zcbor_state_t *state, const struct motor_config_payload_uint6int *input);
static bool encode_repeated_motor_config_payload_uint7uint(zcbor_state_t *state, const struct motor_config_payload_uint7uint *input);
static bool encode_repeated_pump_config_payload_uint1uint(zcbor_state_t *state, const struct pump_config_payload_uint1uint *input);
static bool encode_repeated_pump_config_payload_uint2uint(zcbor_state_t *state, const struct pump_config_payload_uint2uint *input);
static bool encode_repeated_temp_config_payload_uint1float(zcbor_state_t *state, const struct temp_config_payload_uint1float *input);
static bool encode_repeated_temp_config_payload_uint2float(zcbor_state_t *state, const struct temp_config_payload_uint2float *input);
static bool encode_repeated_temp_config_payload_uint3float(zcbor_state_t *state, const struct temp_config_payload_uint3float *input);
static bool encode_repeated_glow_config_payload_uint1uint(zcbor_state_t *state, const struct glow_config_payload_uint1uint *input);
static bool encode_repeated_state_command_payload_uint1int(zcbor_state_t *state, const struct state_command_payload_uint1int *input);
static bool encode_repeated_temp_command_payload_motor_index_m(zcbor_state_t *state, const struct temp_command_payload_motor_index_m *input);
static bool encode_repeated_temp_command_payload_uint3float(zcbor_state_t *state, const struct temp_command_payload_uint3float *input);
static bool encode_repeated_send_telemetry_payload_uint1uint(zcbor_state_t *state, const struct send_telemetry_payload_uint1uint *input);
static bool encode_repeated_motor_data_payload_uint4int(zcbor_state_t *state, const struct motor_data_payload_uint4int *input);
static bool encode_repeated_motor_data_payload_uint5int(zcbor_state_t *state, const struct motor_data_payload_uint5int *input);
static bool encode_repeated_motor_data_payload_uint6uint(zcbor_state_t *state, const struct motor_data_payload_uint6uint *input);
static bool encode_repeated_motor_data_payload_uint7uint(zcbor_state_t *state, const struct motor_data_payload_uint7uint *input);
static bool encode_repeated_pump_data_payload_uint3int(zcbor_state_t *state, const struct pump_data_payload_uint3int *input);
static bool encode_repeated_temp_data_payload_uint3bool(zcbor_state_t *state, const struct temp_data_payload_uint3bool *input);
static bool encode_repeated_temp_data_payload_uint4int(zcbor_state_t *state, const struct temp_data_payload_uint4int *input);
static bool encode_repeated_temp_data_payload_uint5float(zcbor_state_t *state, const struct temp_data_payload_uint5float *input);
static bool encode_error_state_reject_payload(zcbor_state_t *state, const struct error_state_reject_payload *input);
static bool encode_error_invalid_cmd_payload(zcbor_state_t *state, const struct error_invalid_cmd_payload *input);
static bool encode_ping_response_payload(zcbor_state_t *state, const struct ping_response_payload *input);
static bool encode_device_announce_payload(zcbor_state_t *state, const struct device_announce_payload *input);
static bool encode_temp_data_payload(zcbor_state_t *state, const struct temp_data_payload *input);
static bool encode_glow_data_payload(zcbor_state_t *state, const struct glow_data_payload *input);
static bool encode_pump_data_payload(zcbor_state_t *state, const struct pump_data_payload *input);
static bool encode_motor_data_payload(zcbor_state_t *state, const struct motor_data_payload *input);
static bool encode_state_data_payload(zcbor_state_t *state, const struct state_data_payload *input);
static bool encode_send_telemetry_payload(zcbor_state_t *state, const struct send_telemetry_payload *input);
static bool encode_temp_command_payload(zcbor_state_t *state, const struct temp_command_payload *input);
static bool encode_glow_command_payload(zcbor_state_t *state, const struct glow_command_payload *input);
static bool encode_pump_command_payload(zcbor_state_t *state, const struct pump_command_payload *input);
static bool encode_motor_command_payload(zcbor_state_t *state, const struct motor_command_payload *input);
static bool encode_state_command_payload(zcbor_state_t *state, const struct state_command_payload *input);
static bool encode_timeout_config_payload(zcbor_state_t *state, const struct timeout_config_payload *input);
static bool encode_telemetry_config_payload(zcbor_state_t *state, const struct telemetry_config_payload *input);
static bool encode_data_subscription_payload(zcbor_state_t *state, const struct data_subscription_payload *input);
static bool encode_glow_config_payload(zcbor_state_t *state, const struct glow_config_payload *input);
static bool encode_temp_config_payload(zcbor_state_t *state, const struct temp_config_payload *input);
static bool encode_pump_config_payload(zcbor_state_t *state, const struct pump_config_payload *input);
static bool encode_motor_config_payload(zcbor_state_t *state, const struct motor_config_payload *input);


static bool encode_repeated_motor_config_payload_uint1uint(
		zcbor_state_t *state, const struct motor_config_payload_uint1uint *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_uint32_put(state, (1))))
	&& ((((*input).motor_config_payload_uint1uint <= UINT32_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))
	&& (zcbor_uint32_encode(state, (&(*input).motor_config_payload_uint1uint)))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_repeated_motor_config_payload_uint2float(
		zcbor_state_t *state, const struct motor_config_payload_uint2float *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_uint32_put(state, (2))))
	&& (zcbor_float64_encode(state, (&(*input).motor_config_payload_uint2float)))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_repeated_motor_config_payload_uint3float(
		zcbor_state_t *state, const struct motor_config_payload_uint3float *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_uint32_put(state, (3))))
	&& (zcbor_float64_encode(state, (&(*input).motor_config_payload_uint3float)))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_repeated_motor_config_payload_uint4float(
		zcbor_state_t *state, const struct motor_config_payload_uint4float *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_uint32_put(state, (4))))
	&& (zcbor_float64_encode(state, (&(*input).motor_config_payload_uint4float)))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_repeated_motor_config_payload_uint5int(
		zcbor_state_t *state, const struct motor_config_payload_uint5int *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_uint32_put(state, (5))))
	&& (zcbor_int32_encode(state, (&(*input).motor_config_payload_uint5int)))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_repeated_motor_config_payload_uint6int(
		zcbor_state_t *state, const struct motor_config_payload_uint6int *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_uint32_put(state, (6))))
	&& (zcbor_int32_encode(state, (&(*input).motor_config_payload_uint6int)))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_repeated_motor_config_payload_uint7uint(
		zcbor_state_t *state, const struct motor_config_payload_uint7uint *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_uint32_put(state, (7))))
	&& ((((*input).motor_config_payload_uint7uint <= UINT32_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))
	&& (zcbor_uint32_encode(state, (&(*input).motor_config_payload_uint7uint)))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_repeated_pump_config_payload_uint1uint(
		zcbor_state_t *state, const struct pump_config_payload_uint1uint *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_uint32_put(state, (1))))
	&& (zcbor_uint32_encode(state, (&(*input).pump_config_payload_uint1uint)))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_repeated_pump_config_payload_uint2uint(
		zcbor_state_t *state, const struct pump_config_payload_uint2uint *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_uint32_put(state, (2))))
	&& (zcbor_uint32_encode(state, (&(*input).pump_config_payload_uint2uint)))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_repeated_temp_config_payload_uint1float(
		zcbor_state_t *state, const struct temp_config_payload_uint1float *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_uint32_put(state, (1))))
	&& (zcbor_float64_encode(state, (&(*input).temp_config_payload_uint1float)))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_repeated_temp_config_payload_uint2float(
		zcbor_state_t *state, const struct temp_config_payload_uint2float *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_uint32_put(state, (2))))
	&& (zcbor_float64_encode(state, (&(*input).temp_config_payload_uint2float)))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_repeated_temp_config_payload_uint3float(
		zcbor_state_t *state, const struct temp_config_payload_uint3float *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_uint32_put(state, (3))))
	&& (zcbor_float64_encode(state, (&(*input).temp_config_payload_uint3float)))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_repeated_glow_config_payload_uint1uint(
		zcbor_state_t *state, const struct glow_config_payload_uint1uint *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_uint32_put(state, (1))))
	&& (zcbor_uint32_encode(state, (&(*input).glow_config_payload_uint1uint)))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_repeated_state_command_payload_uint1int(
		zcbor_state_t *state, const struct state_command_payload_uint1int *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_uint32_put(state, (1))))
	&& (zcbor_int32_encode(state, (&(*input).state_command_payload_uint1int)))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_repeated_temp_command_payload_motor_index_m(
		zcbor_state_t *state, const struct temp_command_payload_motor_index_m *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_uint32_put(state, (2))))
	&& ((((((*input).temp_command_payload_motor_index_m <= UINT8_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))
	&& (zcbor_uint32_encode(state, (&(*input).temp_command_payload_motor_index_m)))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_repeated_temp_command_payload_uint3float(
		zcbor_state_t *state, const struct temp_command_payload_uint3float *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_uint32_put(state, (3))))
	&& (zcbor_float64_encode(state, (&(*input).temp_command_payload_uint3float)))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_repeated_send_telemetry_payload_uint1uint(
		zcbor_state_t *state, const struct send_telemetry_payload_uint1uint *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_uint32_put(state, (1))))
	&& (zcbor_uint32_encode(state, (&(*input).send_telemetry_payload_uint1uint)))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_repeated_motor_data_payload_uint4int(
		zcbor_state_t *state, const struct motor_data_payload_uint4int *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_uint32_put(state, (4))))
	&& (zcbor_int32_encode(state, (&(*input).motor_data_payload_uint4int)))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_repeated_motor_data_payload_uint5int(
		zcbor_state_t *state, const struct motor_data_payload_uint5int *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_uint32_put(state, (5))))
	&& (zcbor_int32_encode(state, (&(*input).motor_data_payload_uint5int)))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_repeated_motor_data_payload_uint6uint(
		zcbor_state_t *state, const struct motor_data_payload_uint6uint *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_uint32_put(state, (6))))
	&& ((((*input).motor_data_payload_uint6uint <= UINT32_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))
	&& (zcbor_uint32_encode(state, (&(*input).motor_data_payload_uint6uint)))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_repeated_motor_data_payload_uint7uint(
		zcbor_state_t *state, const struct motor_data_payload_uint7uint *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_uint32_put(state, (7))))
	&& ((((*input).motor_data_payload_uint7uint <= UINT32_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))
	&& (zcbor_uint32_encode(state, (&(*input).motor_data_payload_uint7uint)))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_repeated_pump_data_payload_uint3int(
		zcbor_state_t *state, const struct pump_data_payload_uint3int *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_uint32_put(state, (3))))
	&& (zcbor_int32_encode(state, (&(*input).pump_data_payload_uint3int)))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_repeated_temp_data_payload_uint3bool(
		zcbor_state_t *state, const struct temp_data_payload_uint3bool *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_uint32_put(state, (3))))
	&& (zcbor_bool_encode(state, (&(*input).temp_data_payload_uint3bool)))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_repeated_temp_data_payload_uint4int(
		zcbor_state_t *state, const struct temp_data_payload_uint4int *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_uint32_put(state, (4))))
	&& (zcbor_int32_encode(state, (&(*input).temp_data_payload_uint4int)))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_repeated_temp_data_payload_uint5float(
		zcbor_state_t *state, const struct temp_data_payload_uint5float *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_uint32_put(state, (5))))
	&& (zcbor_float64_encode(state, (&(*input).temp_data_payload_uint5float)))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_error_state_reject_payload(
		zcbor_state_t *state, const struct error_state_reject_payload *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_map_start_encode(state, 1) && (((((zcbor_uint32_put(state, (0))))
	&& ((((((*input).error_state_reject_payload_state_m <= UINT8_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))
	&& (zcbor_uint32_encode(state, (&(*input).error_state_reject_payload_state_m))))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_map_end_encode(state, 1))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_error_invalid_cmd_payload(
		zcbor_state_t *state, const struct error_invalid_cmd_payload *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_map_start_encode(state, 1) && (((((zcbor_uint32_put(state, (0))))
	&& (zcbor_int32_encode(state, (&(*input).error_invalid_cmd_payload_uint0int))))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_map_end_encode(state, 1))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_ping_response_payload(
		zcbor_state_t *state, const struct ping_response_payload *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_map_start_encode(state, 1) && (((((zcbor_uint32_put(state, (0))))
	&& ((((((*input).ping_response_payload_timestamp_m <= UINT32_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))
	&& (zcbor_uint32_encode(state, (&(*input).ping_response_payload_timestamp_m))))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_map_end_encode(state, 1))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_device_announce_payload(
		zcbor_state_t *state, const struct device_announce_payload *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_map_start_encode(state, 4) && (((((zcbor_uint32_put(state, (0))))
	&& ((((*input).device_announce_payload_uint0uint <= UINT8_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))
	&& (zcbor_uint32_encode(state, (&(*input).device_announce_payload_uint0uint))))
	&& (((zcbor_uint32_put(state, (1))))
	&& ((((*input).device_announce_payload_uint1uint <= UINT8_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))
	&& (zcbor_uint32_encode(state, (&(*input).device_announce_payload_uint1uint))))
	&& (((zcbor_uint32_put(state, (2))))
	&& ((((*input).device_announce_payload_uint2uint <= UINT8_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))
	&& (zcbor_uint32_encode(state, (&(*input).device_announce_payload_uint2uint))))
	&& (((zcbor_uint32_put(state, (3))))
	&& ((((*input).device_announce_payload_uint3uint <= UINT8_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))
	&& (zcbor_uint32_encode(state, (&(*input).device_announce_payload_uint3uint))))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_map_end_encode(state, 4))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_temp_data_payload(
		zcbor_state_t *state, const struct temp_data_payload *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_map_start_encode(state, 6) && (((((zcbor_uint32_put(state, (0))))
	&& ((((((*input).temp_data_payload_thermometer_index_m <= UINT8_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))
	&& (zcbor_uint32_encode(state, (&(*input).temp_data_payload_thermometer_index_m))))
	&& (((zcbor_uint32_put(state, (1))))
	&& ((((((*input).temp_data_payload_timestamp_m <= UINT32_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))
	&& (zcbor_uint32_encode(state, (&(*input).temp_data_payload_timestamp_m))))
	&& (((zcbor_uint32_put(state, (2))))
	&& (zcbor_float64_encode(state, (&(*input).temp_data_payload_uint2float))))
	&& (!(*input).temp_data_payload_uint3bool_present || encode_repeated_temp_data_payload_uint3bool(state, (&(*input).temp_data_payload_uint3bool)))
	&& (!(*input).temp_data_payload_uint4int_present || encode_repeated_temp_data_payload_uint4int(state, (&(*input).temp_data_payload_uint4int)))
	&& (!(*input).temp_data_payload_uint5float_present || encode_repeated_temp_data_payload_uint5float(state, (&(*input).temp_data_payload_uint5float)))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_map_end_encode(state, 6))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_glow_data_payload(
		zcbor_state_t *state, const struct glow_data_payload *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_map_start_encode(state, 3) && (((((zcbor_uint32_put(state, (0))))
	&& ((((((*input).glow_data_payload_glow_index_m <= UINT8_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))
	&& (zcbor_uint32_encode(state, (&(*input).glow_data_payload_glow_index_m))))
	&& (((zcbor_uint32_put(state, (1))))
	&& ((((((*input).glow_data_payload_timestamp_m <= UINT32_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))
	&& (zcbor_uint32_encode(state, (&(*input).glow_data_payload_timestamp_m))))
	&& (((zcbor_uint32_put(state, (2))))
	&& (zcbor_bool_encode(state, (&(*input).glow_data_payload_uint2bool))))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_map_end_encode(state, 3))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_pump_data_payload(
		zcbor_state_t *state, const struct pump_data_payload *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_map_start_encode(state, 4) && (((((zcbor_uint32_put(state, (0))))
	&& ((((((*input).pump_data_payload_pump_index_m <= UINT8_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))
	&& (zcbor_uint32_encode(state, (&(*input).pump_data_payload_pump_index_m))))
	&& (((zcbor_uint32_put(state, (1))))
	&& ((((((*input).pump_data_payload_timestamp_m <= UINT32_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))
	&& (zcbor_uint32_encode(state, (&(*input).pump_data_payload_timestamp_m))))
	&& (((zcbor_uint32_put(state, (2))))
	&& ((((((*input).pump_data_payload_pump_event_m <= UINT8_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))
	&& (zcbor_uint32_encode(state, (&(*input).pump_data_payload_pump_event_m))))
	&& (!(*input).pump_data_payload_uint3int_present || encode_repeated_pump_data_payload_uint3int(state, (&(*input).pump_data_payload_uint3int)))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_map_end_encode(state, 4))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_motor_data_payload(
		zcbor_state_t *state, const struct motor_data_payload *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_map_start_encode(state, 8) && (((((zcbor_uint32_put(state, (0))))
	&& ((((((*input).motor_data_payload_motor_index_m <= UINT8_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))
	&& (zcbor_uint32_encode(state, (&(*input).motor_data_payload_motor_index_m))))
	&& (((zcbor_uint32_put(state, (1))))
	&& ((((((*input).motor_data_payload_timestamp_m <= UINT32_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))
	&& (zcbor_uint32_encode(state, (&(*input).motor_data_payload_timestamp_m))))
	&& (((zcbor_uint32_put(state, (2))))
	&& (zcbor_int32_encode(state, (&(*input).motor_data_payload_uint2int))))
	&& (((zcbor_uint32_put(state, (3))))
	&& (zcbor_int32_encode(state, (&(*input).motor_data_payload_uint3int))))
	&& (!(*input).motor_data_payload_uint4int_present || encode_repeated_motor_data_payload_uint4int(state, (&(*input).motor_data_payload_uint4int)))
	&& (!(*input).motor_data_payload_uint5int_present || encode_repeated_motor_data_payload_uint5int(state, (&(*input).motor_data_payload_uint5int)))
	&& (!(*input).motor_data_payload_uint6uint_present || encode_repeated_motor_data_payload_uint6uint(state, (&(*input).motor_data_payload_uint6uint)))
	&& (!(*input).motor_data_payload_uint7uint_present || encode_repeated_motor_data_payload_uint7uint(state, (&(*input).motor_data_payload_uint7uint)))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_map_end_encode(state, 8))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_state_data_payload(
		zcbor_state_t *state, const struct state_data_payload *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_map_start_encode(state, 4) && (((((zcbor_uint32_put(state, (0))))
	&& (zcbor_bool_encode(state, (&(*input).state_data_payload_uint0bool))))
	&& (((zcbor_uint32_put(state, (1))))
	&& ((((((*input).state_data_payload_error_code_m <= UINT8_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))
	&& (zcbor_uint32_encode(state, (&(*input).state_data_payload_error_code_m))))
	&& (((zcbor_uint32_put(state, (2))))
	&& ((((((*input).state_data_payload_state_m <= UINT8_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))
	&& (zcbor_uint32_encode(state, (&(*input).state_data_payload_state_m))))
	&& (((zcbor_uint32_put(state, (3))))
	&& ((((((*input).state_data_payload_timestamp_m <= UINT32_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))
	&& (zcbor_uint32_encode(state, (&(*input).state_data_payload_timestamp_m))))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_map_end_encode(state, 4))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_send_telemetry_payload(
		zcbor_state_t *state, const struct send_telemetry_payload *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_map_start_encode(state, 2) && (((((zcbor_uint32_put(state, (0))))
	&& ((((((*input).send_telemetry_payload_telemetry_type_m <= UINT8_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))
	&& (zcbor_uint32_encode(state, (&(*input).send_telemetry_payload_telemetry_type_m))))
	&& (!(*input).send_telemetry_payload_uint1uint_present || encode_repeated_send_telemetry_payload_uint1uint(state, (&(*input).send_telemetry_payload_uint1uint)))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_map_end_encode(state, 2))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_temp_command_payload(
		zcbor_state_t *state, const struct temp_command_payload *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_map_start_encode(state, 4) && (((((zcbor_uint32_put(state, (0))))
	&& ((((((*input).temp_command_payload_thermometer_index_m <= UINT8_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))
	&& (zcbor_uint32_encode(state, (&(*input).temp_command_payload_thermometer_index_m))))
	&& (((zcbor_uint32_put(state, (1))))
	&& ((((((*input).temp_command_payload_temp_cmd_type_m <= UINT8_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))
	&& (zcbor_uint32_encode(state, (&(*input).temp_command_payload_temp_cmd_type_m))))
	&& (!(*input).temp_command_payload_motor_index_m_present || encode_repeated_temp_command_payload_motor_index_m(state, (&(*input).temp_command_payload_motor_index_m)))
	&& (!(*input).temp_command_payload_uint3float_present || encode_repeated_temp_command_payload_uint3float(state, (&(*input).temp_command_payload_uint3float)))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_map_end_encode(state, 4))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_glow_command_payload(
		zcbor_state_t *state, const struct glow_command_payload *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_map_start_encode(state, 2) && (((((zcbor_uint32_put(state, (0))))
	&& ((((((*input).glow_command_payload_glow_index_m <= UINT8_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))
	&& (zcbor_uint32_encode(state, (&(*input).glow_command_payload_glow_index_m))))
	&& (((zcbor_uint32_put(state, (1))))
	&& (zcbor_int32_encode(state, (&(*input).glow_command_payload_uint1int))))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_map_end_encode(state, 2))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_pump_command_payload(
		zcbor_state_t *state, const struct pump_command_payload *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_map_start_encode(state, 2) && (((((zcbor_uint32_put(state, (0))))
	&& ((((((*input).pump_command_payload_pump_index_m <= UINT8_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))
	&& (zcbor_uint32_encode(state, (&(*input).pump_command_payload_pump_index_m))))
	&& (((zcbor_uint32_put(state, (1))))
	&& (zcbor_int32_encode(state, (&(*input).pump_command_payload_uint1int))))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_map_end_encode(state, 2))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_motor_command_payload(
		zcbor_state_t *state, const struct motor_command_payload *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_map_start_encode(state, 2) && (((((zcbor_uint32_put(state, (0))))
	&& ((((((*input).motor_command_payload_motor_index_m <= UINT8_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))
	&& (zcbor_uint32_encode(state, (&(*input).motor_command_payload_motor_index_m))))
	&& (((zcbor_uint32_put(state, (1))))
	&& (zcbor_int32_encode(state, (&(*input).motor_command_payload_uint1int))))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_map_end_encode(state, 2))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_state_command_payload(
		zcbor_state_t *state, const struct state_command_payload *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_map_start_encode(state, 2) && (((((zcbor_uint32_put(state, (0))))
	&& ((((((*input).state_command_payload_mode_m <= UINT8_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))
	&& (zcbor_uint32_encode(state, (&(*input).state_command_payload_mode_m))))
	&& (!(*input).state_command_payload_uint1int_present || encode_repeated_state_command_payload_uint1int(state, (&(*input).state_command_payload_uint1int)))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_map_end_encode(state, 2))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_timeout_config_payload(
		zcbor_state_t *state, const struct timeout_config_payload *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_map_start_encode(state, 2) && (((((zcbor_uint32_put(state, (0))))
	&& (zcbor_bool_encode(state, (&(*input).timeout_config_payload_uint0bool))))
	&& (((zcbor_uint32_put(state, (1))))
	&& (zcbor_uint32_encode(state, (&(*input).timeout_config_payload_uint1uint))))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_map_end_encode(state, 2))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_telemetry_config_payload(
		zcbor_state_t *state, const struct telemetry_config_payload *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_map_start_encode(state, 2) && (((((zcbor_uint32_put(state, (0))))
	&& (zcbor_bool_encode(state, (&(*input).telemetry_config_payload_uint0bool))))
	&& (((zcbor_uint32_put(state, (1))))
	&& (zcbor_uint32_encode(state, (&(*input).telemetry_config_payload_uint1uint))))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_map_end_encode(state, 2))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_data_subscription_payload(
		zcbor_state_t *state, const struct data_subscription_payload *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_map_start_encode(state, 1) && (((((zcbor_uint32_put(state, (0))))
	&& (zcbor_uint64_encode(state, (&(*input).data_subscription_payload_address_m))))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_map_end_encode(state, 1))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_glow_config_payload(
		zcbor_state_t *state, const struct glow_config_payload *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_map_start_encode(state, 2) && (((((zcbor_uint32_put(state, (0))))
	&& ((((((*input).glow_config_payload_glow_index_m <= UINT8_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))
	&& (zcbor_uint32_encode(state, (&(*input).glow_config_payload_glow_index_m))))
	&& (!(*input).glow_config_payload_uint1uint_present || encode_repeated_glow_config_payload_uint1uint(state, (&(*input).glow_config_payload_uint1uint)))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_map_end_encode(state, 2))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_temp_config_payload(
		zcbor_state_t *state, const struct temp_config_payload *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_map_start_encode(state, 4) && (((((zcbor_uint32_put(state, (0))))
	&& ((((((*input).temp_config_payload_thermometer_index_m <= UINT8_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))
	&& (zcbor_uint32_encode(state, (&(*input).temp_config_payload_thermometer_index_m))))
	&& (!(*input).temp_config_payload_uint1float_present || encode_repeated_temp_config_payload_uint1float(state, (&(*input).temp_config_payload_uint1float)))
	&& (!(*input).temp_config_payload_uint2float_present || encode_repeated_temp_config_payload_uint2float(state, (&(*input).temp_config_payload_uint2float)))
	&& (!(*input).temp_config_payload_uint3float_present || encode_repeated_temp_config_payload_uint3float(state, (&(*input).temp_config_payload_uint3float)))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_map_end_encode(state, 4))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_pump_config_payload(
		zcbor_state_t *state, const struct pump_config_payload *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_map_start_encode(state, 3) && (((((zcbor_uint32_put(state, (0))))
	&& ((((((*input).pump_config_payload_pump_index_m <= UINT8_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))
	&& (zcbor_uint32_encode(state, (&(*input).pump_config_payload_pump_index_m))))
	&& (!(*input).pump_config_payload_uint1uint_present || encode_repeated_pump_config_payload_uint1uint(state, (&(*input).pump_config_payload_uint1uint)))
	&& (!(*input).pump_config_payload_uint2uint_present || encode_repeated_pump_config_payload_uint2uint(state, (&(*input).pump_config_payload_uint2uint)))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_map_end_encode(state, 3))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_motor_config_payload(
		zcbor_state_t *state, const struct motor_config_payload *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_map_start_encode(state, 8) && (((((zcbor_uint32_put(state, (0))))
	&& ((((((*input).motor_config_payload_motor_index_m <= UINT8_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))
	&& (zcbor_uint32_encode(state, (&(*input).motor_config_payload_motor_index_m))))
	&& (!(*input).motor_config_payload_uint1uint_present || encode_repeated_motor_config_payload_uint1uint(state, (&(*input).motor_config_payload_uint1uint)))
	&& (!(*input).motor_config_payload_uint2float_present || encode_repeated_motor_config_payload_uint2float(state, (&(*input).motor_config_payload_uint2float)))
	&& (!(*input).motor_config_payload_uint3float_present || encode_repeated_motor_config_payload_uint3float(state, (&(*input).motor_config_payload_uint3float)))
	&& (!(*input).motor_config_payload_uint4float_present || encode_repeated_motor_config_payload_uint4float(state, (&(*input).motor_config_payload_uint4float)))
	&& (!(*input).motor_config_payload_uint5int_present || encode_repeated_motor_config_payload_uint5int(state, (&(*input).motor_config_payload_uint5int)))
	&& (!(*input).motor_config_payload_uint6int_present || encode_repeated_motor_config_payload_uint6int(state, (&(*input).motor_config_payload_uint6int)))
	&& (!(*input).motor_config_payload_uint7uint_present || encode_repeated_motor_config_payload_uint7uint(state, (&(*input).motor_config_payload_uint7uint)))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_map_end_encode(state, 8))));

	log_result(state, res, __func__);
	return res;
}



int cbor_encode_motor_config_payload(
		uint8_t *payload, size_t payload_len,
		const struct motor_config_payload *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	return zcbor_entry_function(payload, payload_len, (void *)input, payload_len_out, states,
		(zcbor_decoder_t *)encode_motor_config_payload, sizeof(states) / sizeof(zcbor_state_t), 1);
}


int cbor_encode_pump_config_payload(
		uint8_t *payload, size_t payload_len,
		const struct pump_config_payload *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	return zcbor_entry_function(payload, payload_len, (void *)input, payload_len_out, states,
		(zcbor_decoder_t *)encode_pump_config_payload, sizeof(states) / sizeof(zcbor_state_t), 1);
}


int cbor_encode_temp_config_payload(
		uint8_t *payload, size_t payload_len,
		const struct temp_config_payload *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	return zcbor_entry_function(payload, payload_len, (void *)input, payload_len_out, states,
		(zcbor_decoder_t *)encode_temp_config_payload, sizeof(states) / sizeof(zcbor_state_t), 1);
}


int cbor_encode_glow_config_payload(
		uint8_t *payload, size_t payload_len,
		const struct glow_config_payload *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	return zcbor_entry_function(payload, payload_len, (void *)input, payload_len_out, states,
		(zcbor_decoder_t *)encode_glow_config_payload, sizeof(states) / sizeof(zcbor_state_t), 1);
}


int cbor_encode_data_subscription_payload(
		uint8_t *payload, size_t payload_len,
		const struct data_subscription_payload *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	return zcbor_entry_function(payload, payload_len, (void *)input, payload_len_out, states,
		(zcbor_decoder_t *)encode_data_subscription_payload, sizeof(states) / sizeof(zcbor_state_t), 1);
}


int cbor_encode_telemetry_config_payload(
		uint8_t *payload, size_t payload_len,
		const struct telemetry_config_payload *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	return zcbor_entry_function(payload, payload_len, (void *)input, payload_len_out, states,
		(zcbor_decoder_t *)encode_telemetry_config_payload, sizeof(states) / sizeof(zcbor_state_t), 1);
}


int cbor_encode_timeout_config_payload(
		uint8_t *payload, size_t payload_len,
		const struct timeout_config_payload *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	return zcbor_entry_function(payload, payload_len, (void *)input, payload_len_out, states,
		(zcbor_decoder_t *)encode_timeout_config_payload, sizeof(states) / sizeof(zcbor_state_t), 1);
}


int cbor_encode_state_command_payload(
		uint8_t *payload, size_t payload_len,
		const struct state_command_payload *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	return zcbor_entry_function(payload, payload_len, (void *)input, payload_len_out, states,
		(zcbor_decoder_t *)encode_state_command_payload, sizeof(states) / sizeof(zcbor_state_t), 1);
}


int cbor_encode_motor_command_payload(
		uint8_t *payload, size_t payload_len,
		const struct motor_command_payload *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	return zcbor_entry_function(payload, payload_len, (void *)input, payload_len_out, states,
		(zcbor_decoder_t *)encode_motor_command_payload, sizeof(states) / sizeof(zcbor_state_t), 1);
}


int cbor_encode_pump_command_payload(
		uint8_t *payload, size_t payload_len,
		const struct pump_command_payload *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	return zcbor_entry_function(payload, payload_len, (void *)input, payload_len_out, states,
		(zcbor_decoder_t *)encode_pump_command_payload, sizeof(states) / sizeof(zcbor_state_t), 1);
}


int cbor_encode_glow_command_payload(
		uint8_t *payload, size_t payload_len,
		const struct glow_command_payload *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	return zcbor_entry_function(payload, payload_len, (void *)input, payload_len_out, states,
		(zcbor_decoder_t *)encode_glow_command_payload, sizeof(states) / sizeof(zcbor_state_t), 1);
}


int cbor_encode_temp_command_payload(
		uint8_t *payload, size_t payload_len,
		const struct temp_command_payload *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	return zcbor_entry_function(payload, payload_len, (void *)input, payload_len_out, states,
		(zcbor_decoder_t *)encode_temp_command_payload, sizeof(states) / sizeof(zcbor_state_t), 1);
}


int cbor_encode_send_telemetry_payload(
		uint8_t *payload, size_t payload_len,
		const struct send_telemetry_payload *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	return zcbor_entry_function(payload, payload_len, (void *)input, payload_len_out, states,
		(zcbor_decoder_t *)encode_send_telemetry_payload, sizeof(states) / sizeof(zcbor_state_t), 1);
}


int cbor_encode_state_data_payload(
		uint8_t *payload, size_t payload_len,
		const struct state_data_payload *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	return zcbor_entry_function(payload, payload_len, (void *)input, payload_len_out, states,
		(zcbor_decoder_t *)encode_state_data_payload, sizeof(states) / sizeof(zcbor_state_t), 1);
}


int cbor_encode_motor_data_payload(
		uint8_t *payload, size_t payload_len,
		const struct motor_data_payload *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	return zcbor_entry_function(payload, payload_len, (void *)input, payload_len_out, states,
		(zcbor_decoder_t *)encode_motor_data_payload, sizeof(states) / sizeof(zcbor_state_t), 1);
}


int cbor_encode_pump_data_payload(
		uint8_t *payload, size_t payload_len,
		const struct pump_data_payload *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	return zcbor_entry_function(payload, payload_len, (void *)input, payload_len_out, states,
		(zcbor_decoder_t *)encode_pump_data_payload, sizeof(states) / sizeof(zcbor_state_t), 1);
}


int cbor_encode_glow_data_payload(
		uint8_t *payload, size_t payload_len,
		const struct glow_data_payload *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	return zcbor_entry_function(payload, payload_len, (void *)input, payload_len_out, states,
		(zcbor_decoder_t *)encode_glow_data_payload, sizeof(states) / sizeof(zcbor_state_t), 1);
}


int cbor_encode_temp_data_payload(
		uint8_t *payload, size_t payload_len,
		const struct temp_data_payload *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	return zcbor_entry_function(payload, payload_len, (void *)input, payload_len_out, states,
		(zcbor_decoder_t *)encode_temp_data_payload, sizeof(states) / sizeof(zcbor_state_t), 1);
}


int cbor_encode_device_announce_payload(
		uint8_t *payload, size_t payload_len,
		const struct device_announce_payload *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	return zcbor_entry_function(payload, payload_len, (void *)input, payload_len_out, states,
		(zcbor_decoder_t *)encode_device_announce_payload, sizeof(states) / sizeof(zcbor_state_t), 1);
}


int cbor_encode_ping_response_payload(
		uint8_t *payload, size_t payload_len,
		const struct ping_response_payload *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	return zcbor_entry_function(payload, payload_len, (void *)input, payload_len_out, states,
		(zcbor_decoder_t *)encode_ping_response_payload, sizeof(states) / sizeof(zcbor_state_t), 1);
}


int cbor_encode_error_invalid_cmd_payload(
		uint8_t *payload, size_t payload_len,
		const struct error_invalid_cmd_payload *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	return zcbor_entry_function(payload, payload_len, (void *)input, payload_len_out, states,
		(zcbor_decoder_t *)encode_error_invalid_cmd_payload, sizeof(states) / sizeof(zcbor_state_t), 1);
}


int cbor_encode_error_state_reject_payload(
		uint8_t *payload, size_t payload_len,
		const struct error_state_reject_payload *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	return zcbor_entry_function(payload, payload_len, (void *)input, payload_len_out, states,
		(zcbor_decoder_t *)encode_error_state_reject_payload, sizeof(states) / sizeof(zcbor_state_t), 1);
}
