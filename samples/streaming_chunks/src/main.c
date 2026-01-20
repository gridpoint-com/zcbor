/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <zcbor_encode.h>

#include "streaming_chunks_decode.h"
#include "streaming_chunks_encode.h"

struct stream_ctx {
	uint8_t *buf;
	size_t size;
	size_t pos;
};

static size_t stream_write(void *user_data, const uint8_t *data, size_t len)
{
	struct stream_ctx *ctx = (struct stream_ctx *)user_data;

	if (!ctx || !data || len == 0) {
		return 0;
	}
	if (ctx->pos + len > ctx->size) {
		return 0;
	}

	memcpy(&ctx->buf[ctx->pos], data, len);
	ctx->pos += len;
	return len;
}

struct chunk_ctx {
	const uint8_t *data;
	size_t len;
	size_t chunk_size;
	size_t offset;
};

struct decode_chunk_ctx {
	uint8_t *buf;
	size_t size;
	size_t len;
};

static bool tstr_chunks(void *user_ctx, zcbor_state_t *state)
{
	struct chunk_ctx *ctx = (struct chunk_ctx *)user_ctx;

	if (!ctx || !state) {
		return false;
	}

	while (ctx->offset < ctx->len) {
		size_t remaining = ctx->len - ctx->offset;
		size_t chunk_len = (remaining > ctx->chunk_size) ? ctx->chunk_size : remaining;

		if (!zcbor_tstr_encode_chunk(state, &ctx->data[ctx->offset], chunk_len)) {
			return false;
		}
		ctx->offset += chunk_len;
	}

	return true;
}

static bool bstr_chunks(void *user_ctx, zcbor_state_t *state)
{
	struct chunk_ctx *ctx = (struct chunk_ctx *)user_ctx;

	if (!ctx || !state) {
		return false;
	}

	while (ctx->offset < ctx->len) {
		size_t remaining = ctx->len - ctx->offset;
		size_t chunk_len = (remaining > ctx->chunk_size) ? ctx->chunk_size : remaining;

		if (!zcbor_bstr_encode_chunk(state, &ctx->data[ctx->offset], chunk_len)) {
			return false;
		}
		ctx->offset += chunk_len;
	}

	return true;
}

static bool decode_chunks(void *user_ctx, const uint8_t *data, size_t len)
{
	struct decode_chunk_ctx *ctx = (struct decode_chunk_ctx *)user_ctx;

	if (!ctx || !data) {
		return false;
	}
	if (len == 0) {
		return true;
	}
	if (ctx->len + len > ctx->size) {
		return false;
	}

	memcpy(&ctx->buf[ctx->len], data, len);
	ctx->len += len;
	return true;
}

static void print_decoded(const struct StreamItem *decoded)
{
	printf("Decoded name: %.*s\r\n",
		(int)decoded->StreamItem_name.len,
		decoded->StreamItem_name.value);
	printf("Decoded payload:");
	for (size_t i = 0; i < decoded->StreamItem_payload.len; i++) {
		printf(" %02x", decoded->StreamItem_payload.value[i]);
	}
	printf("\r\n");
}

static int run_streaming(void)
{
	uint8_t buffer[128];
	size_t out_len = 0;
	struct stream_ctx stream = {
		.buf = buffer,
		.size = sizeof(buffer),
		.pos = 0,
	};

	const uint8_t name[] = "Long Chunky Name to be streamed with lots of characters";
	const uint8_t payload[] = { 0xde, 0xad, 0xbe, 0xef, 0xca, 0xfe, 0xba, 0xbe };

	struct StreamItem item = {
		.StreamItem_name = { .value = name, .len = sizeof(name) - 1 },
		.StreamItem_payload = { .value = payload, .len = sizeof(payload) },
	};

	struct chunk_ctx name_ctx = {
		.data = name,
		.len = sizeof(name) - 1,
		.chunk_size = 4,
		.offset = 0,
	};

	struct chunk_ctx payload_ctx = {
		.data = payload,
		.len = sizeof(payload),
		.chunk_size = 3,
		.offset = 0,
	};

	struct cbor_stream_io_StreamItem io = {0};
	io.chunks_out_StreamItem_name.ctx = &name_ctx;
	io.chunks_out_StreamItem_name.call = tstr_chunks;
	io.chunks_out_StreamItem_payload.ctx = &payload_ctx;
	io.chunks_out_StreamItem_payload.call = bstr_chunks;

	int err = cbor_stream_encode_StreamItem(stream_write, &stream, &item, &io, &out_len);
	if (err != ZCBOR_SUCCESS || out_len != stream.pos) {
		printf("Streaming encode failed: %d\r\n", err);
		return 1;
	}

	struct StreamItem decoded = {0};

	uint8_t name_buf[64];
	uint8_t payload_buf[32];
	struct decode_chunk_ctx name_out = {
		.buf = name_buf,
		.size = sizeof(name_buf),
		.len = 0,
	};
	struct decode_chunk_ctx payload_out = {
		.buf = payload_buf,
		.size = sizeof(payload_buf),
		.len = 0,
	};

	io.chunks_in_StreamItem_name.ctx = &name_out;
	io.chunks_in_StreamItem_name.call = decode_chunks;
	io.chunks_in_StreamItem_payload.ctx = &payload_out;
	io.chunks_in_StreamItem_payload.call = decode_chunks;

	err = cbor_stream_decode_StreamItem(buffer, out_len, &decoded, &io, NULL);
	if (err != ZCBOR_SUCCESS) {
		printf("Decode failed: %d\r\n", err);
		return 1;
	}

	decoded.StreamItem_name.value = name_out.buf;
	decoded.StreamItem_name.len = name_out.len;
	decoded.StreamItem_payload.value = payload_out.buf;
	decoded.StreamItem_payload.len = payload_out.len;

	print_decoded(&decoded);
	return 0;
}

int main(void)
{
	if (run_streaming() != 0) {
		return 1;
	}
	return 0;
}
