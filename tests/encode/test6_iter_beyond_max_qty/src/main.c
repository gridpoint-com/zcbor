/*
 * Copyright (c) 2025 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/ztest.h>
#include <iter_beyond_max_qty_encode.h>
#include <zcbor_encode.h>
#include <zcbor_decode.h>
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

	if (!ctx || len == 0 || !data) {
		return 0;
	}
	if (ctx->pos + len > ctx->size) {
		return 0;
	}

	memcpy(&ctx->buf[ctx->pos], data, len);
	ctx->pos += len;
	return len;
}

/* Iterator context: yields record_t elements from an array, one at a time. */
struct record_iter_ctx {
	const struct record_t *items;
	size_t count;
	size_t pos;
};

static int record_iter_next(void *user_ctx, const void **elem_out)
{
	struct record_iter_ctx *c = (struct record_iter_ctx *)user_ctx;

	if (!c || !elem_out) {
		return -EINVAL;
	}
	if (c->pos >= c->count) {
		return 0; /* no more elements */
	}

	*elem_out = &c->items[c->pos];
	c->pos++;
	return 1;
}

/*
 * Test that streaming encode can iterate beyond DEFAULT_MAX_QTY.
 *
 * The schema is generated with --default-max-qty 3, so the struct can hold
 * at most 3 record_t elements.  But the iterator yields 5 elements.  If the
 * iterator path correctly uses SIZE_MAX (unbounded), all 5 records will be
 * encoded.  If it incorrectly uses DEFAULT_MAX_QTY, only 3 will appear.
 *
 * Verification uses the low-level zcbor decode API since no generated decoder
 * is available (encode-only build with -e, no -d).
 */
ZTEST(cbor_encode_test6, test_iter_beyond_max_qty)
{
	/* 5 records — more than DEFAULT_MAX_QTY (3). */
	const struct record_t all_records[] = {
		{ .id = 0, .value = 100 },
		{ .id = 1, .value = 101 },
		{ .id = 2, .value = 102 },
		{ .id = 3, .value = 103 },
		{ .id = 4, .value = 104 },
	};

	/* Fill the struct with only the first 3 (all it can hold). */
	struct collection_t coll = {
		.label = { .value = "test", .len = 4 },
		.record_t_m = {
			{ .id = 0, .value = 100 },
			{ .id = 1, .value = 101 },
			{ .id = 2, .value = 102 },
		},
		.record_t_m_count = 3,
	};

	uint8_t output[256];
	size_t out_len;

	struct stream_ctx ctx = {
		.buf = output,
		.size = sizeof(output),
		.pos = 0,
	};

	/* Iterator yields all 5 records. */
	struct record_iter_ctx iter = {
		.items = all_records, .count = 5, .pos = 0,
	};

	struct cbor_stream_io_collection_t io = { 0 };
	io.collection_t_records_record_t_m.ctx  = &iter;
	io.collection_t_records_record_t_m.next = record_iter_next;

	int rc = cbor_stream_encode_collection_t(stream_write, &ctx, &coll,
						 &io, &out_len);
	zassert_equal(ZCBOR_SUCCESS, rc, "stream encode failed: %d", rc);
	zassert_equal(out_len, ctx.pos, NULL);

	/*
	 * Low-level decode: verify the CBOR contains all 5 records.
	 *
	 * Expected structure:
	 *   [                       ; outer list (collection_t)
	 *     "test",               ; label (tstr)
	 *     [                     ; inner list (records)
	 *       [0, 100],           ; record 0
	 *       [1, 101],           ; record 1
	 *       [2, 102],           ; record 2
	 *       [3, 103],           ; record 3
	 *       [4, 104],           ; record 4
	 *     ]
	 *   ]
	 */
	ZCBOR_STATE_D(zsd, 3, output, out_len, 1, 0);

	/* Outer list */
	bool ok = zcbor_list_start_decode(zsd);
	zassert_true(ok, "outer list start decode failed");

	/* label: "test" */
	struct zcbor_string label;
	ok = zcbor_tstr_decode(zsd, &label);
	zassert_true(ok, "label decode failed");
	zassert_equal(4, label.len, NULL);
	zassert_mem_equal("test", label.value, 4, NULL);

	/* Inner list (records) */
	ok = zcbor_list_start_decode(zsd);
	zassert_true(ok, "inner list start decode failed");

	/* Decode all 5 records */
	for (uint32_t i = 0; i < 5; i++) {
		ok = zcbor_list_start_decode(zsd);
		zassert_true(ok, "record %u list start failed", i);

		uint32_t id;
		ok = zcbor_uint32_decode(zsd, &id);
		zassert_true(ok, "record %u id decode failed", i);
		zassert_equal(i, id, "record %u id mismatch: got %u", i, id);

		int32_t value;
		ok = zcbor_int32_decode(zsd, &value);
		zassert_true(ok, "record %u value decode failed", i);
		zassert_equal((int32_t)(100 + i), value,
			      "record %u value mismatch: got %d", i, value);

		ok = zcbor_list_end_decode(zsd);
		zassert_true(ok, "record %u list end failed", i);
	}

	/* No more records — attempting another list_start should fail. */
	ok = zcbor_list_start_decode(zsd);
	zassert_false(ok, "should not have a 6th record");
	/* Clear the expected error so subsequent decodes work. */
	zcbor_pop_error(zsd);

	ok = zcbor_list_end_decode(zsd);
	zassert_true(ok, "inner list end failed");

	ok = zcbor_list_end_decode(zsd);
	zassert_true(ok, "outer list end failed");
}

ZTEST_SUITE(cbor_encode_test6, NULL, NULL, NULL, NULL, NULL);
