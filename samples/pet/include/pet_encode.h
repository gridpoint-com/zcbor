/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Generated using zcbor version 0.9.1
 * https://github.com/NordicSemiconductor/zcbor
 * Generated with a --default-max-qty of 3
 */

#ifndef PET_ENCODE_H__
#define PET_ENCODE_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "zcbor_encode.h"
#include "zcbor_decode.h"
#include "pet_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#if ZCBOR_GENERATED_DEFAULT_MAX_QTY != 3
#error "The type file was generated with a different default_max_qty than this file"
#endif

int cbor_encode_Pet(
		uint8_t *payload, size_t payload_len,
		const struct Pet *input,
		size_t *payload_len_out);

/* Streaming encode helpers */
struct zcbor_stream_iter_io {
	void *ctx;
	zcbor_stream_iter next;
};
#ifndef ZCBOR_CHUNK_OUT_DEFINED
#define ZCBOR_CHUNK_OUT_DEFINED
struct zcbor_chunk_out {
	void *ctx;
	zcbor_stream_chunk_out call;
};
#endif
#ifndef ZCBOR_CHUNK_IN_DEFINED
#define ZCBOR_CHUNK_IN_DEFINED
struct zcbor_chunk_in {
	void *ctx;
	zcbor_stream_chunk_in call;
};
#endif

#ifndef CBOR_STREAM_IO_PET_DEFINED
#define CBOR_STREAM_IO_PET_DEFINED
struct cbor_stream_io_Pet {
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
#endif
typedef struct cbor_stream_io_Pet cbor_stream_io_Pet;

int cbor_stream_encode_Pet(
		zcbor_stream_write_fn stream_write, void *stream_user_data,
		const struct Pet *input,
		const cbor_stream_io_Pet *io,
		size_t *bytes_written_out);






#ifdef __cplusplus
}
#endif

#endif /* PET_ENCODE_H__ */
