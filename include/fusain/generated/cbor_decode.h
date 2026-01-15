/*
 * Generated using zcbor version 0.9.1
 * https://github.com/NordicSemiconductor/zcbor
 * Generated with a --default-max-qty of 3
 */

#ifndef FUSAIN_CBOR_DECODE_H__
#define FUSAIN_CBOR_DECODE_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#if DEFAULT_MAX_QTY != 3
#error "The type file was generated with a different default_max_qty than this file"
#endif


int cbor_decode_motor_config_payload(
		const uint8_t *payload, size_t payload_len,
		struct motor_config_payload *result,
		size_t *payload_len_out);


int cbor_decode_pump_config_payload(
		const uint8_t *payload, size_t payload_len,
		struct pump_config_payload *result,
		size_t *payload_len_out);


int cbor_decode_temp_config_payload(
		const uint8_t *payload, size_t payload_len,
		struct temp_config_payload *result,
		size_t *payload_len_out);


int cbor_decode_glow_config_payload(
		const uint8_t *payload, size_t payload_len,
		struct glow_config_payload *result,
		size_t *payload_len_out);


int cbor_decode_data_subscription_payload(
		const uint8_t *payload, size_t payload_len,
		struct data_subscription_payload *result,
		size_t *payload_len_out);


int cbor_decode_telemetry_config_payload(
		const uint8_t *payload, size_t payload_len,
		struct telemetry_config_payload *result,
		size_t *payload_len_out);


int cbor_decode_timeout_config_payload(
		const uint8_t *payload, size_t payload_len,
		struct timeout_config_payload *result,
		size_t *payload_len_out);


int cbor_decode_state_command_payload(
		const uint8_t *payload, size_t payload_len,
		struct state_command_payload *result,
		size_t *payload_len_out);


int cbor_decode_motor_command_payload(
		const uint8_t *payload, size_t payload_len,
		struct motor_command_payload *result,
		size_t *payload_len_out);


int cbor_decode_pump_command_payload(
		const uint8_t *payload, size_t payload_len,
		struct pump_command_payload *result,
		size_t *payload_len_out);


int cbor_decode_glow_command_payload(
		const uint8_t *payload, size_t payload_len,
		struct glow_command_payload *result,
		size_t *payload_len_out);


int cbor_decode_temp_command_payload(
		const uint8_t *payload, size_t payload_len,
		struct temp_command_payload *result,
		size_t *payload_len_out);


int cbor_decode_send_telemetry_payload(
		const uint8_t *payload, size_t payload_len,
		struct send_telemetry_payload *result,
		size_t *payload_len_out);


int cbor_decode_state_data_payload(
		const uint8_t *payload, size_t payload_len,
		struct state_data_payload *result,
		size_t *payload_len_out);


int cbor_decode_motor_data_payload(
		const uint8_t *payload, size_t payload_len,
		struct motor_data_payload *result,
		size_t *payload_len_out);


int cbor_decode_pump_data_payload(
		const uint8_t *payload, size_t payload_len,
		struct pump_data_payload *result,
		size_t *payload_len_out);


int cbor_decode_glow_data_payload(
		const uint8_t *payload, size_t payload_len,
		struct glow_data_payload *result,
		size_t *payload_len_out);


int cbor_decode_temp_data_payload(
		const uint8_t *payload, size_t payload_len,
		struct temp_data_payload *result,
		size_t *payload_len_out);


int cbor_decode_device_announce_payload(
		const uint8_t *payload, size_t payload_len,
		struct device_announce_payload *result,
		size_t *payload_len_out);


int cbor_decode_ping_response_payload(
		const uint8_t *payload, size_t payload_len,
		struct ping_response_payload *result,
		size_t *payload_len_out);


int cbor_decode_error_invalid_cmd_payload(
		const uint8_t *payload, size_t payload_len,
		struct error_invalid_cmd_payload *result,
		size_t *payload_len_out);


int cbor_decode_error_state_reject_payload(
		const uint8_t *payload, size_t payload_len,
		struct error_state_reject_payload *result,
		size_t *payload_len_out);


#ifdef __cplusplus
}
#endif

#endif /* FUSAIN_CBOR_DECODE_H__ */
