/*
 * Generated using zcbor version 0.9.1
 * https://github.com/NordicSemiconductor/zcbor
 * Generated with a --default-max-qty of 3
 */

#ifndef CBOR_ENCODE_H__
#define CBOR_ENCODE_H__

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


int cbor_encode_motor_config_payload(
		uint8_t *payload, size_t payload_len,
		const struct motor_config_payload *input,
		size_t *payload_len_out);


int cbor_encode_pump_config_payload(
		uint8_t *payload, size_t payload_len,
		const struct pump_config_payload *input,
		size_t *payload_len_out);


int cbor_encode_temp_config_payload(
		uint8_t *payload, size_t payload_len,
		const struct temp_config_payload *input,
		size_t *payload_len_out);


int cbor_encode_glow_config_payload(
		uint8_t *payload, size_t payload_len,
		const struct glow_config_payload *input,
		size_t *payload_len_out);


int cbor_encode_data_subscription_payload(
		uint8_t *payload, size_t payload_len,
		const struct data_subscription_payload *input,
		size_t *payload_len_out);


int cbor_encode_telemetry_config_payload(
		uint8_t *payload, size_t payload_len,
		const struct telemetry_config_payload *input,
		size_t *payload_len_out);


int cbor_encode_timeout_config_payload(
		uint8_t *payload, size_t payload_len,
		const struct timeout_config_payload *input,
		size_t *payload_len_out);


int cbor_encode_state_command_payload(
		uint8_t *payload, size_t payload_len,
		const struct state_command_payload *input,
		size_t *payload_len_out);


int cbor_encode_motor_command_payload(
		uint8_t *payload, size_t payload_len,
		const struct motor_command_payload *input,
		size_t *payload_len_out);


int cbor_encode_pump_command_payload(
		uint8_t *payload, size_t payload_len,
		const struct pump_command_payload *input,
		size_t *payload_len_out);


int cbor_encode_glow_command_payload(
		uint8_t *payload, size_t payload_len,
		const struct glow_command_payload *input,
		size_t *payload_len_out);


int cbor_encode_temp_command_payload(
		uint8_t *payload, size_t payload_len,
		const struct temp_command_payload *input,
		size_t *payload_len_out);


int cbor_encode_send_telemetry_payload(
		uint8_t *payload, size_t payload_len,
		const struct send_telemetry_payload *input,
		size_t *payload_len_out);


int cbor_encode_state_data_payload(
		uint8_t *payload, size_t payload_len,
		const struct state_data_payload *input,
		size_t *payload_len_out);


int cbor_encode_motor_data_payload(
		uint8_t *payload, size_t payload_len,
		const struct motor_data_payload *input,
		size_t *payload_len_out);


int cbor_encode_pump_data_payload(
		uint8_t *payload, size_t payload_len,
		const struct pump_data_payload *input,
		size_t *payload_len_out);


int cbor_encode_glow_data_payload(
		uint8_t *payload, size_t payload_len,
		const struct glow_data_payload *input,
		size_t *payload_len_out);


int cbor_encode_temp_data_payload(
		uint8_t *payload, size_t payload_len,
		const struct temp_data_payload *input,
		size_t *payload_len_out);


int cbor_encode_device_announce_payload(
		uint8_t *payload, size_t payload_len,
		const struct device_announce_payload *input,
		size_t *payload_len_out);


int cbor_encode_ping_response_payload(
		uint8_t *payload, size_t payload_len,
		const struct ping_response_payload *input,
		size_t *payload_len_out);


int cbor_encode_error_invalid_cmd_payload(
		uint8_t *payload, size_t payload_len,
		const struct error_invalid_cmd_payload *input,
		size_t *payload_len_out);


int cbor_encode_error_state_reject_payload(
		uint8_t *payload, size_t payload_len,
		const struct error_state_reject_payload *input,
		size_t *payload_len_out);


#ifdef __cplusplus
}
#endif

#endif /* CBOR_ENCODE_H__ */
