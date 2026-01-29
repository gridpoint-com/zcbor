/*
 * Copyright (c) 2026
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Generated using zcbor version 0.9.1
 * https://github.com/NordicSemiconductor/zcbor
 * Generated with a --default-max-qty of 3
 */

#ifndef STREAMING_CHUNKS_DECODE_H__
#define STREAMING_CHUNKS_DECODE_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "zcbor_encode.h"
#include "zcbor_decode.h"
#include "streaming_chunks_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#if ZCBOR_GENERATED_DEFAULT_MAX_QTY != 3
#error "The type file was generated with a different default_max_qty than this file"
#endif

int cbor_decode_StreamItem(
		const uint8_t *payload, size_t payload_len,
		struct StreamItem *result,
		size_t *payload_len_out);





/* Streaming decode helpers */
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

#ifndef CBOR_STREAM_IO_STREAMITEM_DEFINED
#define CBOR_STREAM_IO_STREAMITEM_DEFINED
struct cbor_stream_io_StreamItem {
	/* Text string fields (chunk_out) */
	struct zcbor_chunk_out chunks_out_StreamItem_name;

	/* Byte string fields (chunk_out) */
	struct zcbor_chunk_out chunks_out_StreamItem_payload;

	/* Text string fields (chunk_in) */
	struct zcbor_chunk_in chunks_in_StreamItem_name;

	/* Byte string fields (chunk_in) */
	struct zcbor_chunk_in chunks_in_StreamItem_payload;
};
#endif
typedef struct cbor_stream_io_StreamItem cbor_stream_io_StreamItem;

int cbor_stream_decode_StreamItem(
		const uint8_t *payload, size_t payload_len,
		struct StreamItem *result,
		const cbor_stream_io_StreamItem *io,
		size_t *payload_len_out);


#ifdef __cplusplus
}
#endif

#endif /* STREAMING_CHUNKS_DECODE_H__ */
