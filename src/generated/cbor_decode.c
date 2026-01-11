/*
 * Generated using zcbor version 0.9.1
 * https://github.com/NordicSemiconductor/zcbor
 * Generated with a --default-max-qty of 3
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <zcbor_decode.h>
#include <zcbor_print.h>
#include <fusain/generated/cbor_decode.h>

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

static bool decode_repeated_motor_config_payload_uint1uint(zcbor_state_t *state, struct motor_config_payload_uint1uint *result);
static bool decode_repeated_motor_config_payload_uint2float(zcbor_state_t *state, struct motor_config_payload_uint2float *result);
static bool decode_repeated_motor_config_payload_uint3float(zcbor_state_t *state, struct motor_config_payload_uint3float *result);
static bool decode_repeated_motor_config_payload_uint4float(zcbor_state_t *state, struct motor_config_payload_uint4float *result);
static bool decode_repeated_motor_config_payload_uint5int(zcbor_state_t *state, struct motor_config_payload_uint5int *result);
static bool decode_repeated_motor_config_payload_uint6int(zcbor_state_t *state, struct motor_config_payload_uint6int *result);
static bool decode_repeated_motor_config_payload_uint7uint(zcbor_state_t *state, struct motor_config_payload_uint7uint *result);
static bool decode_repeated_pump_config_payload_uint1uint(zcbor_state_t *state, struct pump_config_payload_uint1uint *result);
static bool decode_repeated_pump_config_payload_uint2uint(zcbor_state_t *state, struct pump_config_payload_uint2uint *result);
static bool decode_repeated_temp_config_payload_uint1float(zcbor_state_t *state, struct temp_config_payload_uint1float *result);
static bool decode_repeated_temp_config_payload_uint2float(zcbor_state_t *state, struct temp_config_payload_uint2float *result);
static bool decode_repeated_temp_config_payload_uint3float(zcbor_state_t *state, struct temp_config_payload_uint3float *result);
static bool decode_repeated_glow_config_payload_uint1uint(zcbor_state_t *state, struct glow_config_payload_uint1uint *result);
static bool decode_repeated_state_command_payload_uint1int(zcbor_state_t *state, struct state_command_payload_uint1int *result);
static bool decode_repeated_temp_command_payload_motor_index_m(zcbor_state_t *state, struct temp_command_payload_motor_index_m *result);
static bool decode_repeated_temp_command_payload_uint3float(zcbor_state_t *state, struct temp_command_payload_uint3float *result);
static bool decode_repeated_send_telemetry_payload_uint1uint(zcbor_state_t *state, struct send_telemetry_payload_uint1uint *result);
static bool decode_repeated_motor_data_payload_uint4int(zcbor_state_t *state, struct motor_data_payload_uint4int *result);
static bool decode_repeated_motor_data_payload_uint5int(zcbor_state_t *state, struct motor_data_payload_uint5int *result);
static bool decode_repeated_motor_data_payload_uint6uint(zcbor_state_t *state, struct motor_data_payload_uint6uint *result);
static bool decode_repeated_motor_data_payload_uint7uint(zcbor_state_t *state, struct motor_data_payload_uint7uint *result);
static bool decode_repeated_pump_data_payload_uint3int(zcbor_state_t *state, struct pump_data_payload_uint3int *result);
static bool decode_repeated_temp_data_payload_uint3bool(zcbor_state_t *state, struct temp_data_payload_uint3bool *result);
static bool decode_repeated_temp_data_payload_uint4int(zcbor_state_t *state, struct temp_data_payload_uint4int *result);
static bool decode_repeated_temp_data_payload_uint5float(zcbor_state_t *state, struct temp_data_payload_uint5float *result);
static bool decode_error_state_reject_payload(zcbor_state_t *state, struct error_state_reject_payload *result);
static bool decode_error_invalid_cmd_payload(zcbor_state_t *state, struct error_invalid_cmd_payload *result);
static bool decode_ping_response_payload(zcbor_state_t *state, struct ping_response_payload *result);
static bool decode_device_announce_payload(zcbor_state_t *state, struct device_announce_payload *result);
static bool decode_temp_data_payload(zcbor_state_t *state, struct temp_data_payload *result);
static bool decode_glow_data_payload(zcbor_state_t *state, struct glow_data_payload *result);
static bool decode_pump_data_payload(zcbor_state_t *state, struct pump_data_payload *result);
static bool decode_motor_data_payload(zcbor_state_t *state, struct motor_data_payload *result);
static bool decode_state_data_payload(zcbor_state_t *state, struct state_data_payload *result);
static bool decode_send_telemetry_payload(zcbor_state_t *state, struct send_telemetry_payload *result);
static bool decode_temp_command_payload(zcbor_state_t *state, struct temp_command_payload *result);
static bool decode_glow_command_payload(zcbor_state_t *state, struct glow_command_payload *result);
static bool decode_pump_command_payload(zcbor_state_t *state, struct pump_command_payload *result);
static bool decode_motor_command_payload(zcbor_state_t *state, struct motor_command_payload *result);
static bool decode_state_command_payload(zcbor_state_t *state, struct state_command_payload *result);
static bool decode_timeout_config_payload(zcbor_state_t *state, struct timeout_config_payload *result);
static bool decode_telemetry_config_payload(zcbor_state_t *state, struct telemetry_config_payload *result);
static bool decode_data_subscription_payload(zcbor_state_t *state, struct data_subscription_payload *result);
static bool decode_glow_config_payload(zcbor_state_t *state, struct glow_config_payload *result);
static bool decode_temp_config_payload(zcbor_state_t *state, struct temp_config_payload *result);
static bool decode_pump_config_payload(zcbor_state_t *state, struct pump_config_payload *result);
static bool decode_motor_config_payload(zcbor_state_t *state, struct motor_config_payload *result);


static bool decode_repeated_motor_config_payload_uint1uint(
		zcbor_state_t *state, struct motor_config_payload_uint1uint *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_uint32_expect(state, (1))))
	&& (zcbor_uint32_decode(state, (&(*result).motor_config_payload_uint1uint)))
	&& ((((*result).motor_config_payload_uint1uint <= UINT32_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_repeated_motor_config_payload_uint2float(
		zcbor_state_t *state, struct motor_config_payload_uint2float *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_uint32_expect(state, (2))))
	&& (zcbor_float_decode(state, (&(*result).motor_config_payload_uint2float)))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_repeated_motor_config_payload_uint3float(
		zcbor_state_t *state, struct motor_config_payload_uint3float *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_uint32_expect(state, (3))))
	&& (zcbor_float_decode(state, (&(*result).motor_config_payload_uint3float)))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_repeated_motor_config_payload_uint4float(
		zcbor_state_t *state, struct motor_config_payload_uint4float *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_uint32_expect(state, (4))))
	&& (zcbor_float_decode(state, (&(*result).motor_config_payload_uint4float)))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_repeated_motor_config_payload_uint5int(
		zcbor_state_t *state, struct motor_config_payload_uint5int *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_uint32_expect(state, (5))))
	&& (zcbor_int32_decode(state, (&(*result).motor_config_payload_uint5int)))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_repeated_motor_config_payload_uint6int(
		zcbor_state_t *state, struct motor_config_payload_uint6int *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_uint32_expect(state, (6))))
	&& (zcbor_int32_decode(state, (&(*result).motor_config_payload_uint6int)))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_repeated_motor_config_payload_uint7uint(
		zcbor_state_t *state, struct motor_config_payload_uint7uint *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_uint32_expect(state, (7))))
	&& (zcbor_uint32_decode(state, (&(*result).motor_config_payload_uint7uint)))
	&& ((((*result).motor_config_payload_uint7uint <= UINT32_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_repeated_pump_config_payload_uint1uint(
		zcbor_state_t *state, struct pump_config_payload_uint1uint *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_uint32_expect(state, (1))))
	&& (zcbor_uint32_decode(state, (&(*result).pump_config_payload_uint1uint)))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_repeated_pump_config_payload_uint2uint(
		zcbor_state_t *state, struct pump_config_payload_uint2uint *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_uint32_expect(state, (2))))
	&& (zcbor_uint32_decode(state, (&(*result).pump_config_payload_uint2uint)))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_repeated_temp_config_payload_uint1float(
		zcbor_state_t *state, struct temp_config_payload_uint1float *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_uint32_expect(state, (1))))
	&& (zcbor_float_decode(state, (&(*result).temp_config_payload_uint1float)))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_repeated_temp_config_payload_uint2float(
		zcbor_state_t *state, struct temp_config_payload_uint2float *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_uint32_expect(state, (2))))
	&& (zcbor_float_decode(state, (&(*result).temp_config_payload_uint2float)))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_repeated_temp_config_payload_uint3float(
		zcbor_state_t *state, struct temp_config_payload_uint3float *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_uint32_expect(state, (3))))
	&& (zcbor_float_decode(state, (&(*result).temp_config_payload_uint3float)))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_repeated_glow_config_payload_uint1uint(
		zcbor_state_t *state, struct glow_config_payload_uint1uint *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_uint32_expect(state, (1))))
	&& (zcbor_uint32_decode(state, (&(*result).glow_config_payload_uint1uint)))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_repeated_state_command_payload_uint1int(
		zcbor_state_t *state, struct state_command_payload_uint1int *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_uint32_expect(state, (1))))
	&& (zcbor_int32_decode(state, (&(*result).state_command_payload_uint1int)))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_repeated_temp_command_payload_motor_index_m(
		zcbor_state_t *state, struct temp_command_payload_motor_index_m *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_uint32_expect(state, (2))))
	&& (zcbor_uint32_decode(state, (&(*result).temp_command_payload_motor_index_m)))
	&& ((((((*result).temp_command_payload_motor_index_m <= UINT8_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_repeated_temp_command_payload_uint3float(
		zcbor_state_t *state, struct temp_command_payload_uint3float *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_uint32_expect(state, (3))))
	&& (zcbor_float_decode(state, (&(*result).temp_command_payload_uint3float)))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_repeated_send_telemetry_payload_uint1uint(
		zcbor_state_t *state, struct send_telemetry_payload_uint1uint *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_uint32_expect(state, (1))))
	&& (zcbor_uint32_decode(state, (&(*result).send_telemetry_payload_uint1uint)))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_repeated_motor_data_payload_uint4int(
		zcbor_state_t *state, struct motor_data_payload_uint4int *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_uint32_expect(state, (4))))
	&& (zcbor_int32_decode(state, (&(*result).motor_data_payload_uint4int)))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_repeated_motor_data_payload_uint5int(
		zcbor_state_t *state, struct motor_data_payload_uint5int *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_uint32_expect(state, (5))))
	&& (zcbor_int32_decode(state, (&(*result).motor_data_payload_uint5int)))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_repeated_motor_data_payload_uint6uint(
		zcbor_state_t *state, struct motor_data_payload_uint6uint *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_uint32_expect(state, (6))))
	&& (zcbor_uint32_decode(state, (&(*result).motor_data_payload_uint6uint)))
	&& ((((*result).motor_data_payload_uint6uint <= UINT32_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_repeated_motor_data_payload_uint7uint(
		zcbor_state_t *state, struct motor_data_payload_uint7uint *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_uint32_expect(state, (7))))
	&& (zcbor_uint32_decode(state, (&(*result).motor_data_payload_uint7uint)))
	&& ((((*result).motor_data_payload_uint7uint <= UINT32_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_repeated_pump_data_payload_uint3int(
		zcbor_state_t *state, struct pump_data_payload_uint3int *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_uint32_expect(state, (3))))
	&& (zcbor_int32_decode(state, (&(*result).pump_data_payload_uint3int)))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_repeated_temp_data_payload_uint3bool(
		zcbor_state_t *state, struct temp_data_payload_uint3bool *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_uint32_expect(state, (3))))
	&& (zcbor_bool_decode(state, (&(*result).temp_data_payload_uint3bool)))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_repeated_temp_data_payload_uint4int(
		zcbor_state_t *state, struct temp_data_payload_uint4int *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_uint32_expect(state, (4))))
	&& (zcbor_int32_decode(state, (&(*result).temp_data_payload_uint4int)))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_repeated_temp_data_payload_uint5float(
		zcbor_state_t *state, struct temp_data_payload_uint5float *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_uint32_expect(state, (5))))
	&& (zcbor_float_decode(state, (&(*result).temp_data_payload_uint5float)))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_error_state_reject_payload(
		zcbor_state_t *state, struct error_state_reject_payload *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_map_start_decode(state) && (((((zcbor_uint32_expect(state, (0))))
	&& (zcbor_uint32_decode(state, (&(*result).error_state_reject_payload_state_m)))
	&& ((((((*result).error_state_reject_payload_state_m <= UINT8_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false)))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_map_end_decode(state))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_error_invalid_cmd_payload(
		zcbor_state_t *state, struct error_invalid_cmd_payload *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_map_start_decode(state) && (((((zcbor_uint32_expect(state, (0))))
	&& (zcbor_int32_decode(state, (&(*result).error_invalid_cmd_payload_uint0int))))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_map_end_decode(state))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_ping_response_payload(
		zcbor_state_t *state, struct ping_response_payload *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_map_start_decode(state) && (((((zcbor_uint32_expect(state, (0))))
	&& (zcbor_uint32_decode(state, (&(*result).ping_response_payload_timestamp_m)))
	&& ((((((*result).ping_response_payload_timestamp_m <= UINT32_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false)))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_map_end_decode(state))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_device_announce_payload(
		zcbor_state_t *state, struct device_announce_payload *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_map_start_decode(state) && (((((zcbor_uint32_expect(state, (0))))
	&& (zcbor_uint32_decode(state, (&(*result).device_announce_payload_uint0uint)))
	&& ((((*result).device_announce_payload_uint0uint <= UINT8_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false)))
	&& (((zcbor_uint32_expect(state, (1))))
	&& (zcbor_uint32_decode(state, (&(*result).device_announce_payload_uint1uint)))
	&& ((((*result).device_announce_payload_uint1uint <= UINT8_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false)))
	&& (((zcbor_uint32_expect(state, (2))))
	&& (zcbor_uint32_decode(state, (&(*result).device_announce_payload_uint2uint)))
	&& ((((*result).device_announce_payload_uint2uint <= UINT8_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false)))
	&& (((zcbor_uint32_expect(state, (3))))
	&& (zcbor_uint32_decode(state, (&(*result).device_announce_payload_uint3uint)))
	&& ((((*result).device_announce_payload_uint3uint <= UINT8_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false)))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_map_end_decode(state))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_temp_data_payload(
		zcbor_state_t *state, struct temp_data_payload *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_map_start_decode(state) && (((((zcbor_uint32_expect(state, (0))))
	&& (zcbor_uint32_decode(state, (&(*result).temp_data_payload_thermometer_index_m)))
	&& ((((((*result).temp_data_payload_thermometer_index_m <= UINT8_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false)))
	&& (((zcbor_uint32_expect(state, (1))))
	&& (zcbor_uint32_decode(state, (&(*result).temp_data_payload_timestamp_m)))
	&& ((((((*result).temp_data_payload_timestamp_m <= UINT32_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false)))
	&& (((zcbor_uint32_expect(state, (2))))
	&& (zcbor_float_decode(state, (&(*result).temp_data_payload_uint2float))))
	&& zcbor_present_decode(&((*result).temp_data_payload_uint3bool_present), (zcbor_decoder_t *)decode_repeated_temp_data_payload_uint3bool, state, (&(*result).temp_data_payload_uint3bool))
	&& zcbor_present_decode(&((*result).temp_data_payload_uint4int_present), (zcbor_decoder_t *)decode_repeated_temp_data_payload_uint4int, state, (&(*result).temp_data_payload_uint4int))
	&& zcbor_present_decode(&((*result).temp_data_payload_uint5float_present), (zcbor_decoder_t *)decode_repeated_temp_data_payload_uint5float, state, (&(*result).temp_data_payload_uint5float))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_map_end_decode(state))));

	if (false) {
		/* For testing that the types of the arguments are correct.
		 * A compiler error here means a bug in zcbor.
		 */
		decode_repeated_temp_data_payload_uint3bool(state, (&(*result).temp_data_payload_uint3bool));
		decode_repeated_temp_data_payload_uint4int(state, (&(*result).temp_data_payload_uint4int));
		decode_repeated_temp_data_payload_uint5float(state, (&(*result).temp_data_payload_uint5float));
	}

	log_result(state, res, __func__);
	return res;
}

static bool decode_glow_data_payload(
		zcbor_state_t *state, struct glow_data_payload *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_map_start_decode(state) && (((((zcbor_uint32_expect(state, (0))))
	&& (zcbor_uint32_decode(state, (&(*result).glow_data_payload_glow_index_m)))
	&& ((((((*result).glow_data_payload_glow_index_m <= UINT8_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false)))
	&& (((zcbor_uint32_expect(state, (1))))
	&& (zcbor_uint32_decode(state, (&(*result).glow_data_payload_timestamp_m)))
	&& ((((((*result).glow_data_payload_timestamp_m <= UINT32_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false)))
	&& (((zcbor_uint32_expect(state, (2))))
	&& (zcbor_bool_decode(state, (&(*result).glow_data_payload_uint2bool))))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_map_end_decode(state))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_pump_data_payload(
		zcbor_state_t *state, struct pump_data_payload *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_map_start_decode(state) && (((((zcbor_uint32_expect(state, (0))))
	&& (zcbor_uint32_decode(state, (&(*result).pump_data_payload_pump_index_m)))
	&& ((((((*result).pump_data_payload_pump_index_m <= UINT8_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false)))
	&& (((zcbor_uint32_expect(state, (1))))
	&& (zcbor_uint32_decode(state, (&(*result).pump_data_payload_timestamp_m)))
	&& ((((((*result).pump_data_payload_timestamp_m <= UINT32_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false)))
	&& (((zcbor_uint32_expect(state, (2))))
	&& (zcbor_uint32_decode(state, (&(*result).pump_data_payload_pump_event_m)))
	&& ((((((*result).pump_data_payload_pump_event_m <= UINT8_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false)))
	&& zcbor_present_decode(&((*result).pump_data_payload_uint3int_present), (zcbor_decoder_t *)decode_repeated_pump_data_payload_uint3int, state, (&(*result).pump_data_payload_uint3int))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_map_end_decode(state))));

	if (false) {
		/* For testing that the types of the arguments are correct.
		 * A compiler error here means a bug in zcbor.
		 */
		decode_repeated_pump_data_payload_uint3int(state, (&(*result).pump_data_payload_uint3int));
	}

	log_result(state, res, __func__);
	return res;
}

static bool decode_motor_data_payload(
		zcbor_state_t *state, struct motor_data_payload *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_map_start_decode(state) && (((((zcbor_uint32_expect(state, (0))))
	&& (zcbor_uint32_decode(state, (&(*result).motor_data_payload_motor_index_m)))
	&& ((((((*result).motor_data_payload_motor_index_m <= UINT8_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false)))
	&& (((zcbor_uint32_expect(state, (1))))
	&& (zcbor_uint32_decode(state, (&(*result).motor_data_payload_timestamp_m)))
	&& ((((((*result).motor_data_payload_timestamp_m <= UINT32_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false)))
	&& (((zcbor_uint32_expect(state, (2))))
	&& (zcbor_int32_decode(state, (&(*result).motor_data_payload_uint2int))))
	&& (((zcbor_uint32_expect(state, (3))))
	&& (zcbor_int32_decode(state, (&(*result).motor_data_payload_uint3int))))
	&& zcbor_present_decode(&((*result).motor_data_payload_uint4int_present), (zcbor_decoder_t *)decode_repeated_motor_data_payload_uint4int, state, (&(*result).motor_data_payload_uint4int))
	&& zcbor_present_decode(&((*result).motor_data_payload_uint5int_present), (zcbor_decoder_t *)decode_repeated_motor_data_payload_uint5int, state, (&(*result).motor_data_payload_uint5int))
	&& zcbor_present_decode(&((*result).motor_data_payload_uint6uint_present), (zcbor_decoder_t *)decode_repeated_motor_data_payload_uint6uint, state, (&(*result).motor_data_payload_uint6uint))
	&& zcbor_present_decode(&((*result).motor_data_payload_uint7uint_present), (zcbor_decoder_t *)decode_repeated_motor_data_payload_uint7uint, state, (&(*result).motor_data_payload_uint7uint))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_map_end_decode(state))));

	if (false) {
		/* For testing that the types of the arguments are correct.
		 * A compiler error here means a bug in zcbor.
		 */
		decode_repeated_motor_data_payload_uint4int(state, (&(*result).motor_data_payload_uint4int));
		decode_repeated_motor_data_payload_uint5int(state, (&(*result).motor_data_payload_uint5int));
		decode_repeated_motor_data_payload_uint6uint(state, (&(*result).motor_data_payload_uint6uint));
		decode_repeated_motor_data_payload_uint7uint(state, (&(*result).motor_data_payload_uint7uint));
	}

	log_result(state, res, __func__);
	return res;
}

static bool decode_state_data_payload(
		zcbor_state_t *state, struct state_data_payload *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_map_start_decode(state) && (((((zcbor_uint32_expect(state, (0))))
	&& (zcbor_bool_decode(state, (&(*result).state_data_payload_uint0bool))))
	&& (((zcbor_uint32_expect(state, (1))))
	&& (zcbor_uint32_decode(state, (&(*result).state_data_payload_error_code_m)))
	&& ((((((*result).state_data_payload_error_code_m <= UINT8_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false)))
	&& (((zcbor_uint32_expect(state, (2))))
	&& (zcbor_uint32_decode(state, (&(*result).state_data_payload_state_m)))
	&& ((((((*result).state_data_payload_state_m <= UINT8_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false)))
	&& (((zcbor_uint32_expect(state, (3))))
	&& (zcbor_uint32_decode(state, (&(*result).state_data_payload_timestamp_m)))
	&& ((((((*result).state_data_payload_timestamp_m <= UINT32_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false)))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_map_end_decode(state))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_send_telemetry_payload(
		zcbor_state_t *state, struct send_telemetry_payload *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_map_start_decode(state) && (((((zcbor_uint32_expect(state, (0))))
	&& (zcbor_uint32_decode(state, (&(*result).send_telemetry_payload_telemetry_type_m)))
	&& ((((((*result).send_telemetry_payload_telemetry_type_m <= UINT8_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false)))
	&& zcbor_present_decode(&((*result).send_telemetry_payload_uint1uint_present), (zcbor_decoder_t *)decode_repeated_send_telemetry_payload_uint1uint, state, (&(*result).send_telemetry_payload_uint1uint))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_map_end_decode(state))));

	if (false) {
		/* For testing that the types of the arguments are correct.
		 * A compiler error here means a bug in zcbor.
		 */
		decode_repeated_send_telemetry_payload_uint1uint(state, (&(*result).send_telemetry_payload_uint1uint));
	}

	log_result(state, res, __func__);
	return res;
}

static bool decode_temp_command_payload(
		zcbor_state_t *state, struct temp_command_payload *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_map_start_decode(state) && (((((zcbor_uint32_expect(state, (0))))
	&& (zcbor_uint32_decode(state, (&(*result).temp_command_payload_thermometer_index_m)))
	&& ((((((*result).temp_command_payload_thermometer_index_m <= UINT8_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false)))
	&& (((zcbor_uint32_expect(state, (1))))
	&& (zcbor_uint32_decode(state, (&(*result).temp_command_payload_temp_cmd_type_m)))
	&& ((((((*result).temp_command_payload_temp_cmd_type_m <= UINT8_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false)))
	&& zcbor_present_decode(&((*result).temp_command_payload_motor_index_m_present), (zcbor_decoder_t *)decode_repeated_temp_command_payload_motor_index_m, state, (&(*result).temp_command_payload_motor_index_m))
	&& zcbor_present_decode(&((*result).temp_command_payload_uint3float_present), (zcbor_decoder_t *)decode_repeated_temp_command_payload_uint3float, state, (&(*result).temp_command_payload_uint3float))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_map_end_decode(state))));

	if (false) {
		/* For testing that the types of the arguments are correct.
		 * A compiler error here means a bug in zcbor.
		 */
		decode_repeated_temp_command_payload_motor_index_m(state, (&(*result).temp_command_payload_motor_index_m));
		decode_repeated_temp_command_payload_uint3float(state, (&(*result).temp_command_payload_uint3float));
	}

	log_result(state, res, __func__);
	return res;
}

static bool decode_glow_command_payload(
		zcbor_state_t *state, struct glow_command_payload *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_map_start_decode(state) && (((((zcbor_uint32_expect(state, (0))))
	&& (zcbor_uint32_decode(state, (&(*result).glow_command_payload_glow_index_m)))
	&& ((((((*result).glow_command_payload_glow_index_m <= UINT8_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false)))
	&& (((zcbor_uint32_expect(state, (1))))
	&& (zcbor_int32_decode(state, (&(*result).glow_command_payload_uint1int))))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_map_end_decode(state))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_pump_command_payload(
		zcbor_state_t *state, struct pump_command_payload *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_map_start_decode(state) && (((((zcbor_uint32_expect(state, (0))))
	&& (zcbor_uint32_decode(state, (&(*result).pump_command_payload_pump_index_m)))
	&& ((((((*result).pump_command_payload_pump_index_m <= UINT8_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false)))
	&& (((zcbor_uint32_expect(state, (1))))
	&& (zcbor_int32_decode(state, (&(*result).pump_command_payload_uint1int))))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_map_end_decode(state))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_motor_command_payload(
		zcbor_state_t *state, struct motor_command_payload *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_map_start_decode(state) && (((((zcbor_uint32_expect(state, (0))))
	&& (zcbor_uint32_decode(state, (&(*result).motor_command_payload_motor_index_m)))
	&& ((((((*result).motor_command_payload_motor_index_m <= UINT8_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false)))
	&& (((zcbor_uint32_expect(state, (1))))
	&& (zcbor_int32_decode(state, (&(*result).motor_command_payload_uint1int))))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_map_end_decode(state))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_state_command_payload(
		zcbor_state_t *state, struct state_command_payload *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_map_start_decode(state) && (((((zcbor_uint32_expect(state, (0))))
	&& (zcbor_uint32_decode(state, (&(*result).state_command_payload_mode_m)))
	&& ((((((*result).state_command_payload_mode_m <= UINT8_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false)))
	&& zcbor_present_decode(&((*result).state_command_payload_uint1int_present), (zcbor_decoder_t *)decode_repeated_state_command_payload_uint1int, state, (&(*result).state_command_payload_uint1int))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_map_end_decode(state))));

	if (false) {
		/* For testing that the types of the arguments are correct.
		 * A compiler error here means a bug in zcbor.
		 */
		decode_repeated_state_command_payload_uint1int(state, (&(*result).state_command_payload_uint1int));
	}

	log_result(state, res, __func__);
	return res;
}

static bool decode_timeout_config_payload(
		zcbor_state_t *state, struct timeout_config_payload *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_map_start_decode(state) && (((((zcbor_uint32_expect(state, (0))))
	&& (zcbor_bool_decode(state, (&(*result).timeout_config_payload_uint0bool))))
	&& (((zcbor_uint32_expect(state, (1))))
	&& (zcbor_uint32_decode(state, (&(*result).timeout_config_payload_uint1uint))))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_map_end_decode(state))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_telemetry_config_payload(
		zcbor_state_t *state, struct telemetry_config_payload *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_map_start_decode(state) && (((((zcbor_uint32_expect(state, (0))))
	&& (zcbor_bool_decode(state, (&(*result).telemetry_config_payload_uint0bool))))
	&& (((zcbor_uint32_expect(state, (1))))
	&& (zcbor_uint32_decode(state, (&(*result).telemetry_config_payload_uint1uint))))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_map_end_decode(state))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_data_subscription_payload(
		zcbor_state_t *state, struct data_subscription_payload *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_map_start_decode(state) && (((((zcbor_uint32_expect(state, (0))))
	&& (zcbor_uint64_decode(state, (&(*result).data_subscription_payload_address_m))))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_map_end_decode(state))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_glow_config_payload(
		zcbor_state_t *state, struct glow_config_payload *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_map_start_decode(state) && (((((zcbor_uint32_expect(state, (0))))
	&& (zcbor_uint32_decode(state, (&(*result).glow_config_payload_glow_index_m)))
	&& ((((((*result).glow_config_payload_glow_index_m <= UINT8_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false)))
	&& zcbor_present_decode(&((*result).glow_config_payload_uint1uint_present), (zcbor_decoder_t *)decode_repeated_glow_config_payload_uint1uint, state, (&(*result).glow_config_payload_uint1uint))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_map_end_decode(state))));

	if (false) {
		/* For testing that the types of the arguments are correct.
		 * A compiler error here means a bug in zcbor.
		 */
		decode_repeated_glow_config_payload_uint1uint(state, (&(*result).glow_config_payload_uint1uint));
	}

	log_result(state, res, __func__);
	return res;
}

static bool decode_temp_config_payload(
		zcbor_state_t *state, struct temp_config_payload *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_map_start_decode(state) && (((((zcbor_uint32_expect(state, (0))))
	&& (zcbor_uint32_decode(state, (&(*result).temp_config_payload_thermometer_index_m)))
	&& ((((((*result).temp_config_payload_thermometer_index_m <= UINT8_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false)))
	&& zcbor_present_decode(&((*result).temp_config_payload_uint1float_present), (zcbor_decoder_t *)decode_repeated_temp_config_payload_uint1float, state, (&(*result).temp_config_payload_uint1float))
	&& zcbor_present_decode(&((*result).temp_config_payload_uint2float_present), (zcbor_decoder_t *)decode_repeated_temp_config_payload_uint2float, state, (&(*result).temp_config_payload_uint2float))
	&& zcbor_present_decode(&((*result).temp_config_payload_uint3float_present), (zcbor_decoder_t *)decode_repeated_temp_config_payload_uint3float, state, (&(*result).temp_config_payload_uint3float))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_map_end_decode(state))));

	if (false) {
		/* For testing that the types of the arguments are correct.
		 * A compiler error here means a bug in zcbor.
		 */
		decode_repeated_temp_config_payload_uint1float(state, (&(*result).temp_config_payload_uint1float));
		decode_repeated_temp_config_payload_uint2float(state, (&(*result).temp_config_payload_uint2float));
		decode_repeated_temp_config_payload_uint3float(state, (&(*result).temp_config_payload_uint3float));
	}

	log_result(state, res, __func__);
	return res;
}

static bool decode_pump_config_payload(
		zcbor_state_t *state, struct pump_config_payload *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_map_start_decode(state) && (((((zcbor_uint32_expect(state, (0))))
	&& (zcbor_uint32_decode(state, (&(*result).pump_config_payload_pump_index_m)))
	&& ((((((*result).pump_config_payload_pump_index_m <= UINT8_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false)))
	&& zcbor_present_decode(&((*result).pump_config_payload_uint1uint_present), (zcbor_decoder_t *)decode_repeated_pump_config_payload_uint1uint, state, (&(*result).pump_config_payload_uint1uint))
	&& zcbor_present_decode(&((*result).pump_config_payload_uint2uint_present), (zcbor_decoder_t *)decode_repeated_pump_config_payload_uint2uint, state, (&(*result).pump_config_payload_uint2uint))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_map_end_decode(state))));

	if (false) {
		/* For testing that the types of the arguments are correct.
		 * A compiler error here means a bug in zcbor.
		 */
		decode_repeated_pump_config_payload_uint1uint(state, (&(*result).pump_config_payload_uint1uint));
		decode_repeated_pump_config_payload_uint2uint(state, (&(*result).pump_config_payload_uint2uint));
	}

	log_result(state, res, __func__);
	return res;
}

static bool decode_motor_config_payload(
		zcbor_state_t *state, struct motor_config_payload *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_map_start_decode(state) && (((((zcbor_uint32_expect(state, (0))))
	&& (zcbor_uint32_decode(state, (&(*result).motor_config_payload_motor_index_m)))
	&& ((((((*result).motor_config_payload_motor_index_m <= UINT8_MAX)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false)))
	&& zcbor_present_decode(&((*result).motor_config_payload_uint1uint_present), (zcbor_decoder_t *)decode_repeated_motor_config_payload_uint1uint, state, (&(*result).motor_config_payload_uint1uint))
	&& zcbor_present_decode(&((*result).motor_config_payload_uint2float_present), (zcbor_decoder_t *)decode_repeated_motor_config_payload_uint2float, state, (&(*result).motor_config_payload_uint2float))
	&& zcbor_present_decode(&((*result).motor_config_payload_uint3float_present), (zcbor_decoder_t *)decode_repeated_motor_config_payload_uint3float, state, (&(*result).motor_config_payload_uint3float))
	&& zcbor_present_decode(&((*result).motor_config_payload_uint4float_present), (zcbor_decoder_t *)decode_repeated_motor_config_payload_uint4float, state, (&(*result).motor_config_payload_uint4float))
	&& zcbor_present_decode(&((*result).motor_config_payload_uint5int_present), (zcbor_decoder_t *)decode_repeated_motor_config_payload_uint5int, state, (&(*result).motor_config_payload_uint5int))
	&& zcbor_present_decode(&((*result).motor_config_payload_uint6int_present), (zcbor_decoder_t *)decode_repeated_motor_config_payload_uint6int, state, (&(*result).motor_config_payload_uint6int))
	&& zcbor_present_decode(&((*result).motor_config_payload_uint7uint_present), (zcbor_decoder_t *)decode_repeated_motor_config_payload_uint7uint, state, (&(*result).motor_config_payload_uint7uint))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_map_end_decode(state))));

	if (false) {
		/* For testing that the types of the arguments are correct.
		 * A compiler error here means a bug in zcbor.
		 */
		decode_repeated_motor_config_payload_uint1uint(state, (&(*result).motor_config_payload_uint1uint));
		decode_repeated_motor_config_payload_uint2float(state, (&(*result).motor_config_payload_uint2float));
		decode_repeated_motor_config_payload_uint3float(state, (&(*result).motor_config_payload_uint3float));
		decode_repeated_motor_config_payload_uint4float(state, (&(*result).motor_config_payload_uint4float));
		decode_repeated_motor_config_payload_uint5int(state, (&(*result).motor_config_payload_uint5int));
		decode_repeated_motor_config_payload_uint6int(state, (&(*result).motor_config_payload_uint6int));
		decode_repeated_motor_config_payload_uint7uint(state, (&(*result).motor_config_payload_uint7uint));
	}

	log_result(state, res, __func__);
	return res;
}



int cbor_decode_motor_config_payload(
		const uint8_t *payload, size_t payload_len,
		struct motor_config_payload *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	return zcbor_entry_function(payload, payload_len, (void *)result, payload_len_out, states,
		(zcbor_decoder_t *)decode_motor_config_payload, sizeof(states) / sizeof(zcbor_state_t), 1);
}


int cbor_decode_pump_config_payload(
		const uint8_t *payload, size_t payload_len,
		struct pump_config_payload *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	return zcbor_entry_function(payload, payload_len, (void *)result, payload_len_out, states,
		(zcbor_decoder_t *)decode_pump_config_payload, sizeof(states) / sizeof(zcbor_state_t), 1);
}


int cbor_decode_temp_config_payload(
		const uint8_t *payload, size_t payload_len,
		struct temp_config_payload *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	return zcbor_entry_function(payload, payload_len, (void *)result, payload_len_out, states,
		(zcbor_decoder_t *)decode_temp_config_payload, sizeof(states) / sizeof(zcbor_state_t), 1);
}


int cbor_decode_glow_config_payload(
		const uint8_t *payload, size_t payload_len,
		struct glow_config_payload *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	return zcbor_entry_function(payload, payload_len, (void *)result, payload_len_out, states,
		(zcbor_decoder_t *)decode_glow_config_payload, sizeof(states) / sizeof(zcbor_state_t), 1);
}


int cbor_decode_data_subscription_payload(
		const uint8_t *payload, size_t payload_len,
		struct data_subscription_payload *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	return zcbor_entry_function(payload, payload_len, (void *)result, payload_len_out, states,
		(zcbor_decoder_t *)decode_data_subscription_payload, sizeof(states) / sizeof(zcbor_state_t), 1);
}


int cbor_decode_telemetry_config_payload(
		const uint8_t *payload, size_t payload_len,
		struct telemetry_config_payload *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	return zcbor_entry_function(payload, payload_len, (void *)result, payload_len_out, states,
		(zcbor_decoder_t *)decode_telemetry_config_payload, sizeof(states) / sizeof(zcbor_state_t), 1);
}


int cbor_decode_timeout_config_payload(
		const uint8_t *payload, size_t payload_len,
		struct timeout_config_payload *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	return zcbor_entry_function(payload, payload_len, (void *)result, payload_len_out, states,
		(zcbor_decoder_t *)decode_timeout_config_payload, sizeof(states) / sizeof(zcbor_state_t), 1);
}


int cbor_decode_state_command_payload(
		const uint8_t *payload, size_t payload_len,
		struct state_command_payload *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	return zcbor_entry_function(payload, payload_len, (void *)result, payload_len_out, states,
		(zcbor_decoder_t *)decode_state_command_payload, sizeof(states) / sizeof(zcbor_state_t), 1);
}


int cbor_decode_motor_command_payload(
		const uint8_t *payload, size_t payload_len,
		struct motor_command_payload *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	return zcbor_entry_function(payload, payload_len, (void *)result, payload_len_out, states,
		(zcbor_decoder_t *)decode_motor_command_payload, sizeof(states) / sizeof(zcbor_state_t), 1);
}


int cbor_decode_pump_command_payload(
		const uint8_t *payload, size_t payload_len,
		struct pump_command_payload *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	return zcbor_entry_function(payload, payload_len, (void *)result, payload_len_out, states,
		(zcbor_decoder_t *)decode_pump_command_payload, sizeof(states) / sizeof(zcbor_state_t), 1);
}


int cbor_decode_glow_command_payload(
		const uint8_t *payload, size_t payload_len,
		struct glow_command_payload *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	return zcbor_entry_function(payload, payload_len, (void *)result, payload_len_out, states,
		(zcbor_decoder_t *)decode_glow_command_payload, sizeof(states) / sizeof(zcbor_state_t), 1);
}


int cbor_decode_temp_command_payload(
		const uint8_t *payload, size_t payload_len,
		struct temp_command_payload *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	return zcbor_entry_function(payload, payload_len, (void *)result, payload_len_out, states,
		(zcbor_decoder_t *)decode_temp_command_payload, sizeof(states) / sizeof(zcbor_state_t), 1);
}


int cbor_decode_send_telemetry_payload(
		const uint8_t *payload, size_t payload_len,
		struct send_telemetry_payload *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	return zcbor_entry_function(payload, payload_len, (void *)result, payload_len_out, states,
		(zcbor_decoder_t *)decode_send_telemetry_payload, sizeof(states) / sizeof(zcbor_state_t), 1);
}


int cbor_decode_state_data_payload(
		const uint8_t *payload, size_t payload_len,
		struct state_data_payload *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	return zcbor_entry_function(payload, payload_len, (void *)result, payload_len_out, states,
		(zcbor_decoder_t *)decode_state_data_payload, sizeof(states) / sizeof(zcbor_state_t), 1);
}


int cbor_decode_motor_data_payload(
		const uint8_t *payload, size_t payload_len,
		struct motor_data_payload *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	return zcbor_entry_function(payload, payload_len, (void *)result, payload_len_out, states,
		(zcbor_decoder_t *)decode_motor_data_payload, sizeof(states) / sizeof(zcbor_state_t), 1);
}


int cbor_decode_pump_data_payload(
		const uint8_t *payload, size_t payload_len,
		struct pump_data_payload *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	return zcbor_entry_function(payload, payload_len, (void *)result, payload_len_out, states,
		(zcbor_decoder_t *)decode_pump_data_payload, sizeof(states) / sizeof(zcbor_state_t), 1);
}


int cbor_decode_glow_data_payload(
		const uint8_t *payload, size_t payload_len,
		struct glow_data_payload *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	return zcbor_entry_function(payload, payload_len, (void *)result, payload_len_out, states,
		(zcbor_decoder_t *)decode_glow_data_payload, sizeof(states) / sizeof(zcbor_state_t), 1);
}


int cbor_decode_temp_data_payload(
		const uint8_t *payload, size_t payload_len,
		struct temp_data_payload *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	return zcbor_entry_function(payload, payload_len, (void *)result, payload_len_out, states,
		(zcbor_decoder_t *)decode_temp_data_payload, sizeof(states) / sizeof(zcbor_state_t), 1);
}


int cbor_decode_device_announce_payload(
		const uint8_t *payload, size_t payload_len,
		struct device_announce_payload *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	return zcbor_entry_function(payload, payload_len, (void *)result, payload_len_out, states,
		(zcbor_decoder_t *)decode_device_announce_payload, sizeof(states) / sizeof(zcbor_state_t), 1);
}


int cbor_decode_ping_response_payload(
		const uint8_t *payload, size_t payload_len,
		struct ping_response_payload *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	return zcbor_entry_function(payload, payload_len, (void *)result, payload_len_out, states,
		(zcbor_decoder_t *)decode_ping_response_payload, sizeof(states) / sizeof(zcbor_state_t), 1);
}


int cbor_decode_error_invalid_cmd_payload(
		const uint8_t *payload, size_t payload_len,
		struct error_invalid_cmd_payload *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	return zcbor_entry_function(payload, payload_len, (void *)result, payload_len_out, states,
		(zcbor_decoder_t *)decode_error_invalid_cmd_payload, sizeof(states) / sizeof(zcbor_state_t), 1);
}


int cbor_decode_error_state_reject_payload(
		const uint8_t *payload, size_t payload_len,
		struct error_state_reject_payload *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	return zcbor_entry_function(payload, payload_len, (void *)result, payload_len_out, states,
		(zcbor_decoder_t *)decode_error_state_reject_payload, sizeof(states) / sizeof(zcbor_state_t), 1);
}
