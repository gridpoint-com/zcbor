/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/ztest.h>
#include <pet_encode.h>
#include <zcbor_encode.h>

#if !defined(ZCBOR_CANONICAL) || defined(STREAMING)
#define TEST_INDEFINITE_LENGTH_ARRAYS
#endif
#include <common_test.h>

#ifdef STREAMING
#include <string.h>
#include <errno.h>

struct stream_ctx {
	uint8_t *buf;
	size_t size;
	size_t pos;
};

static size_t stream_write(void *user_data, const uint8_t *data, size_t len)
{
	struct stream_ctx *ctx = (struct stream_ctx *)user_data;

	if (!ctx) {
		return 0;
	}
	if (len == 0) {
		return 0;
	}
	if (!data) {
		return 0;
	}
	if (ctx->pos + len > ctx->size) {
		return 0;
	}

	memcpy(&ctx->buf[ctx->pos], data, len);
	ctx->pos += len;
	return len;
}

struct bstr_push_ctx {
	const uint8_t *data;
	size_t len;
};

static bool bstr_push(void *user_data, zcbor_state_t *state)
{
	struct bstr_push_ctx *ctx = (struct bstr_push_ctx *)user_data;

	if (!ctx) {
		return false;
	}

	return zcbor_bstr_encode_chunk(state, ctx->data, ctx->len);
}
#endif /* STREAMING */

#ifdef STREAMING
struct bstr_chunk_ctx {
	int idx;
	const uint8_t *chunks[2];
	size_t lens[2];
};

static int bstr_next_chunk(void *user_ctx, const uint8_t **ptr, size_t *len)
{
	struct bstr_chunk_ctx *c = (struct bstr_chunk_ctx *)user_ctx;

	if (!c || !ptr || !len) {
		return -EINVAL;
	}

	if (c->idx >= 2) {
		return 0;
	}

	*ptr = c->chunks[c->idx];
	*len = c->lens[c->idx];
	c->idx++;
	return 1;
}
#endif /* STREAMING */


/* This test uses generated code to encode a 'Pet' instance. It populates the
 * generated struct, and runs the generated encoding function, then checks that
 * everything is correct.
 */
ZTEST(cbor_encode_test2, test_pet)
{
	struct Pet pet = {
		.names = {{.value = "foo", .len = 3}, {.value = "bar", .len = 3}},
		.names_count = 2,
		.birthday = {.value = (uint8_t[]){1,2,3,4,5,6,7,8}, .len = 8},
		.species_choice = Pet_species_dog_c
	};
	uint8_t exp_output[] = {
		LIST(3),
		LIST(2),
			0x63, 0x66, 0x6f, 0x6f, /* foo */
			0x63, 0x62, 0x61, 0x72, /* bar */
		END
		0x48, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
		0x02, /* 2: dog */
		END
	};

	uint8_t output[25];
	size_t out_len;

	/* Check that encoding succeeded. */
#ifdef STREAMING
	struct stream_ctx ctx = {
		.buf = output,
		.size = sizeof(output),
		.pos = 0,
	};

	struct bstr_push_ctx push_ctx = {
		.data = pet.birthday.value,
		.len = pet.birthday.len,
	};
	struct cbor_stream_io_Pet io = { 0 };

	io.chunks_out_Timestamp.ctx = &push_ctx;
	io.chunks_out_Timestamp.call = bstr_push;

	int rc = cbor_stream_encode_Pet(stream_write, &ctx, &pet, NULL, &out_len);
	zassert_equal(ZCBOR_SUCCESS, rc, NULL);
	zassert_equal(out_len, ctx.pos, NULL);
#else
	zassert_equal(ZCBOR_SUCCESS, cbor_encode_Pet(output, sizeof(output), &pet, &out_len), NULL);
#endif

	/* Check that the resulting length is correct. */
	zassert_equal(sizeof(exp_output), out_len, NULL);
	/* Check the payload contents. */
	zassert_mem_equal(exp_output, output, sizeof(exp_output), NULL);

#ifdef STREAMING
	const uint8_t *exp_output_push = exp_output;
	size_t exp_output_push_len = sizeof(exp_output);
	uint8_t output_push[32];
	struct stream_ctx push_out_ctx = {
		.buf = output_push,
		.size = sizeof(output_push),
		.pos = 0,
	};
	size_t out_len_push;

	rc = cbor_stream_encode_Pet(stream_write, &push_out_ctx, &pet, &io, &out_len_push);
	zassert_equal(ZCBOR_SUCCESS, rc, NULL);
	zassert_equal(out_len_push, push_out_ctx.pos, NULL);
	zassert_equal(exp_output_push_len, out_len_push, NULL);
	zassert_mem_equal(exp_output_push, output_push, exp_output_push_len, NULL);
#endif
}


/* This test uses the CBOR encoding library directly, i.e. no generated code.
 * It has no checking against a CDDL schema, but follows the "Pet" structure.
 * It sets up the zcbor_state_t variable.
 * It then makes a number of calls to functions in zcbor_encode.h and checks the
 * resulting payload agains the expected output.
 */
ZTEST(cbor_encode_test2, test_pet_raw)
{
	uint8_t payload[100] = {0};
#ifdef STREAMING
	struct stream_ctx ctx = {
		.buf = payload,
		.size = sizeof(payload),
		.pos = 0,
	};
	zcbor_state_t states[4];
	zcbor_new_encode_state_streaming(states, 4, stream_write, &ctx, 1);
	zcbor_state_t *state = states;
#else
	ZCBOR_STATE_E(state, 4, payload, sizeof(payload), 1);
#endif

	uint8_t exp_output[] = {
		LIST(3),
		LIST(2),
			0x65, 0x66, 0x69, 0x72, 0x73, 0x74, /* first */
			0x66, 0x73, 0x65, 0x63, 0x6f, 0x6e, 0x64, /* second */
		END
		0x48, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
		0x02, /* 2: dog */
		END
	};

	bool res = zcbor_list_start_encode(state, 0);
	zassert_true(res, NULL);

	res = res && zcbor_list_start_encode(state, 0);
	zassert_true(res, NULL);
	res = res && zcbor_tstr_put_lit(state, "first");
	zassert_true(res, NULL);
	res = res && zcbor_tstr_put_lit(state, "second");
	zassert_true(res, NULL);
	res = res && zcbor_list_end_encode(state, 0);
	zassert_true(res, NULL);
	uint8_t timestamp[8] = {1, 2, 3, 4, 5, 6, 7, 8};
	struct zcbor_string timestamp_str = {
		.value = timestamp,
		.len = sizeof(timestamp),
	};
	res = res && zcbor_bstr_encode(state, &timestamp_str);
	zassert_true(res, NULL);
	res = res && zcbor_uint32_put(state, 2 /* dog */);
	zassert_true(res, NULL);
	res = res && zcbor_list_end_encode(state, 0);

	/* Check that encoding succeeded. */
	zassert_true(res, NULL);
	/* Check that the resulting length is correct. */
#ifdef STREAMING
	zassert_equal(sizeof(exp_output), ctx.pos, "%d != %d\r\n",
		sizeof(exp_output), ctx.pos);
#else
	zassert_equal(sizeof(exp_output), state->payload - payload, "%d != %d\r\n",
		sizeof(exp_output), state->payload - payload);
#endif
	/* Check the payload contents. */
	zassert_mem_equal(exp_output, payload, sizeof(exp_output), NULL);
}

#ifdef STREAMING
ZTEST(cbor_encode_test2, test_bstr_indefinite_chunks)
{
	/* Expect: 0x5f (bstr indefinite), 0x43 01 02 03, 0x42 04 05, 0xff (break) */
	uint8_t output[16];
	struct stream_ctx ctx = {
		.buf = output,
		.size = sizeof(output),
		.pos = 0,
	};

	const uint8_t c1[] = { 0x01, 0x02, 0x03 };
	const uint8_t c2[] = { 0x04, 0x05 };
	struct bstr_chunk_ctx chunks = {
		.idx = 0,
		.chunks = { c1, c2 },
		.lens = { sizeof(c1), sizeof(c2) },
	};

	zcbor_state_t s[4];
	zcbor_new_encode_state_streaming(s, 4, stream_write, &ctx, 1);

	zassert_true(zcbor_bstr_encode_indefinite_chunks(s, bstr_next_chunk, &chunks), NULL);
	zassert_equal(ctx.pos, 1 + 1 + sizeof(c1) + 1 + sizeof(c2) + 1, NULL);

	const uint8_t exp[] = { 0x5f, 0x43, 0x01, 0x02, 0x03, 0x42, 0x04, 0x05, 0xff };
	zassert_mem_equal(output, exp, ctx.pos, NULL);
}
#endif /* STREAMING */

#ifdef STREAMING
ZTEST(cbor_encode_test2, test_streaming_container_headers)
{
	/* In streaming mode, lists/maps must be indefinite-length (0x9f/0xbf ... 0xff). */
	uint8_t output[32];
	struct stream_ctx ctx = {
		.buf = output,
		.size = sizeof(output),
		.pos = 0,
	};

	zcbor_state_t s[4];
	zcbor_new_encode_state_streaming(s, 4, stream_write, &ctx, 1);

	zassert_true(zcbor_list_start_encode(s, 3), NULL);
	zassert_true(zcbor_uint32_put(s, 1), NULL);
	zassert_true(zcbor_list_end_encode(s, 3), NULL);

	zassert_true(ctx.pos >= 2, NULL);
	zassert_equal(output[0], 0x9f, NULL);
	zassert_equal(output[ctx.pos - 1], 0xff, NULL);
}
#endif /* STREAMING */

ZTEST_SUITE(cbor_encode_test2, NULL, NULL, NULL, NULL, NULL);
