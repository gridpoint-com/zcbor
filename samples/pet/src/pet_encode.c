/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Generated using zcbor version 0.9.1
 * https://github.com/NordicSemiconductor/zcbor
 * Generated with a --default-max-qty of 3
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "zcbor_encode.h"
#include "pet_encode.h"
#include "zcbor_print.h"

#if ZCBOR_GENERATED_DEFAULT_MAX_QTY != 3
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

/* Streaming io struct (internal, schema-wide). */
struct cbor_stream_io {
	/* Repeated fields (iterator) */
	struct zcbor_stream_iter_io Pet_name_names;

	/* Text string fields (chunk_out) */
	/* no tstr chunk_out io */

	/* Byte string fields (chunk_out) */
	struct zcbor_chunk_out chunks_out_Timestamp;

	/* Text string fields (chunk_in) */
	/* no tstr chunk_in io */

	/* Byte string fields (chunk_in) */
	struct zcbor_chunk_in chunks_in_Timestamp;
};

static bool encode_Pet(zcbor_state_t *state, const struct Pet *input);


static bool encode_Pet(
		zcbor_state_t *state, const struct Pet *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_list_start_encode(state, ZCBOR_VALUE_IS_INDEFINITE_LENGTH) && ((((zcbor_list_start_encode(state, ZCBOR_VALUE_IS_INDEFINITE_LENGTH) && ((((((const struct cbor_stream_io *)zcbor_get_stream_io(state)) && ((const struct cbor_stream_io *)zcbor_get_stream_io(state))->Pet_name_names.next) ? (zcbor_multi_encode_iter_minmax(1, DEFAULT_MAX_QTY, (zcbor_encoder_t *)zcbor_tstr_encode, state, ((const struct cbor_stream_io *)zcbor_get_stream_io(state))->Pet_name_names.next, ((const struct cbor_stream_io *)zcbor_get_stream_io(state))->Pet_name_names.ctx)) : (zcbor_multi_encode_minmax(1, DEFAULT_MAX_QTY, &(*input).names_count, (zcbor_encoder_t *)zcbor_tstr_encode, state, (*&(*input).names), sizeof(struct zcbor_string))))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_list_end_encode(state, ZCBOR_VALUE_IS_INDEFINITE_LENGTH)))
	&& (((((((*input).birthday.len == 8)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))
	&& (zcbor_bstr_encode(state, (&(*input).birthday))))
	&& ((((*input).species_choice == Pet_species_cat_c) ? ((zcbor_uint32_put(state, (1))))
	: (((*input).species_choice == Pet_species_dog_c) ? ((zcbor_uint32_put(state, (2))))
	: (((*input).species_choice == Pet_species_other_c) ? ((zcbor_uint32_put(state, (3))))
	: false))))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_list_end_encode(state, ZCBOR_VALUE_IS_INDEFINITE_LENGTH))));

	log_result(state, res, __func__);
	return res;
}

int cbor_encode_Pet(
		uint8_t *payload, size_t payload_len,
		const struct Pet *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[3 + ZCBOR_CONST_STATE_SLOTS];

	return zcbor_entry_function(payload, payload_len, (void *)input, payload_len_out, states,
		(zcbor_decoder_t *)encode_Pet, sizeof(states) / sizeof(zcbor_state_t), 1);
}

int cbor_stream_encode_Pet(
		zcbor_stream_write_fn stream_write, void *stream_user_data,
		const struct Pet *input,
		const cbor_stream_io_Pet *io,
		size_t *bytes_written_out)
{
	zcbor_state_t states[ZCBOR_STREAM_STATE_ARRAY_SIZE];
	zcbor_new_encode_state_streaming(states, sizeof(states) / sizeof(states[0]),
		stream_write, stream_user_data, 1);
	struct cbor_stream_io io_local = {0};
	if (io) {
				io_local.Pet_name_names = io->Pet_name_names;
		io_local.chunks_out_Timestamp = io->chunks_out_Timestamp;
		zcbor_set_stream_io(&states[0], &io_local);
	} else {
		zcbor_set_stream_io(&states[0], NULL);
	}
	bool ok = encode_Pet(&states[0], input);
	if (!ok) {
		int err = zcbor_pop_error(&states[0]);
		return (err == ZCBOR_SUCCESS) ? ZCBOR_ERR_UNKNOWN : err;
	}
	if (bytes_written_out) {
		*bytes_written_out = zcbor_stream_bytes_written(&states[0]);
	}
	return ZCBOR_SUCCESS;
}
