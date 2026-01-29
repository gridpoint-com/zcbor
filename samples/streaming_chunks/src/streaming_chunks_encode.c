/*
 * Copyright (c) 2026
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
#include "streaming_chunks_encode.h"
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
	/* no repeated iters */

	/* Text string fields (chunk_out) */
	struct zcbor_chunk_out chunks_out_StreamItem_name;

	/* Byte string fields (chunk_out) */
	struct zcbor_chunk_out chunks_out_StreamItem_payload;

	/* Text string fields (chunk_in) */
	struct zcbor_chunk_in chunks_in_StreamItem_name;

	/* Byte string fields (chunk_in) */
	struct zcbor_chunk_in chunks_in_StreamItem_payload;
};

static bool encode_StreamItem(zcbor_state_t *state, const struct StreamItem *input);


static bool encode_StreamItem(
		zcbor_state_t *state, const struct StreamItem *input)
{
	zcbor_log("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool res = (((zcbor_map_start_encode(state, ZCBOR_VALUE_IS_INDEFINITE_LENGTH) && (((((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"name", tmp_str.len = sizeof("name") - 1, &tmp_str)))))
	&& ((((const struct cbor_stream_io *)zcbor_get_stream_io(state)) && ((const struct cbor_stream_io *)zcbor_get_stream_io(state))->chunks_out_StreamItem_name.call) ? (zcbor_tstr_chunk_out(state, ((const struct cbor_stream_io *)zcbor_get_stream_io(state))->chunks_out_StreamItem_name.call, ((const struct cbor_stream_io *)zcbor_get_stream_io(state))->chunks_out_StreamItem_name.ctx)) : ((zcbor_tstr_encode(state, (&(*input).StreamItem_name))))))
	&& (((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"payload", tmp_str.len = sizeof("payload") - 1, &tmp_str)))))
	&& ((((const struct cbor_stream_io *)zcbor_get_stream_io(state)) && ((const struct cbor_stream_io *)zcbor_get_stream_io(state))->chunks_out_StreamItem_payload.call) ? (zcbor_bstr_chunk_out(state, ((const struct cbor_stream_io *)zcbor_get_stream_io(state))->chunks_out_StreamItem_payload.call, ((const struct cbor_stream_io *)zcbor_get_stream_io(state))->chunks_out_StreamItem_payload.ctx)) : ((zcbor_bstr_encode(state, (&(*input).StreamItem_payload))))))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_map_end_encode(state, ZCBOR_VALUE_IS_INDEFINITE_LENGTH))));

	log_result(state, res, __func__);
	return res;
}

int cbor_encode_StreamItem(
		uint8_t *payload, size_t payload_len,
		const struct StreamItem *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[2 + ZCBOR_CONST_STATE_SLOTS];

	return zcbor_entry_function(payload, payload_len, (void *)input, payload_len_out, states,
		(zcbor_decoder_t *)encode_StreamItem, sizeof(states) / sizeof(zcbor_state_t), 1);
}

int cbor_stream_encode_StreamItem(
		zcbor_stream_write_fn stream_write, void *stream_user_data,
		const struct StreamItem *input,
		const cbor_stream_io_StreamItem *io,
		size_t *bytes_written_out)
{
	zcbor_state_t states[ZCBOR_STREAM_STATE_ARRAY_SIZE];
	zcbor_new_encode_state_streaming(states, sizeof(states) / sizeof(states[0]),
		stream_write, stream_user_data, 1);
	struct cbor_stream_io io_local = {0};
	if (io) {
				io_local.chunks_out_StreamItem_name = io->chunks_out_StreamItem_name;
		io_local.chunks_out_StreamItem_payload = io->chunks_out_StreamItem_payload;
		zcbor_set_stream_io(&states[0], &io_local);
	} else {
		zcbor_set_stream_io(&states[0], NULL);
	}
	bool ok = encode_StreamItem(&states[0], input);
	if (!ok) {
		int err = zcbor_pop_error(&states[0]);
		return (err == ZCBOR_SUCCESS) ? ZCBOR_ERR_UNKNOWN : err;
	}
	if (bytes_written_out) {
		*bytes_written_out = zcbor_stream_bytes_written(&states[0]);
	}
	return ZCBOR_SUCCESS;
}
