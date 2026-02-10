/*
 * Copyright (c) 2025 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/ztest.h>
#include <shared_iter_encode.h>
#include <shared_iter_decode.h>
#include <zcbor_encode.h>
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

/* Iterator context: yields item_t elements from an array, one at a time. */
struct item_iter_ctx {
	const struct item_t *items;
	size_t count;
	size_t pos;
};

static int item_iter_next(void *user_ctx, const void **elem_out)
{
	struct item_iter_ctx *c = (struct item_iter_ctx *)user_ctx;

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
 * Test that streaming-encoding a container_t with all 3 optional lists
 * (list_a, list_b, list_c) produces correct CBOR when each list is driven
 * by its own iterator.
 *
 * This exercises the case where multiple fields reference the same repeated
 * type (items_t = [* item_t]).  Each field needs its own iterator slot in the
 * IO struct; if the code generator collapses them into one slot, only the
 * first list receives its items and the remaining lists encode as empty.
 *
 * The code generator now produces three separate fields:
 *   list_a_items_t_item_t_m
 *   list_b_items_t_item_t_m
 *   list_c_items_t_item_t_m
 * so each list drives its own iterator independently.
 */
ZTEST(cbor_encode_test5, test_shared_type_iter)
{
	/* Data for the three lists:
	 *   list_a = [{1,10}]
	 *   list_b = [{2,20},{3,30}]
	 *   list_c = [{4,40}]
	 */
	const struct item_t list_a_items[] = { { .id = 1, .value = 10 } };
	const struct item_t list_b_items[] = {
		{ .id = 2, .value = 20 },
		{ .id = 3, .value = 30 },
	};
	const struct item_t list_c_items[] = { { .id = 4, .value = 40 } };

	struct container_t container = {
		.name = { .value = "test", .len = 4 },
		.list_a = {
			.item_t_m = {
				{ .id = 1, .value = 10 },
			},
			.item_t_m_count = 1,
		},
		.list_a_present = true,
		.list_b = {
			.item_t_m = {
				{ .id = 2, .value = 20 },
				{ .id = 3, .value = 30 },
			},
			.item_t_m_count = 2,
		},
		.list_b_present = true,
		.list_c = {
			.item_t_m = {
				{ .id = 4, .value = 40 },
			},
			.item_t_m_count = 1,
		},
		.list_c_present = true,
	};

	uint8_t output[128];
	size_t out_len;

	struct stream_ctx ctx = {
		.buf = output,
		.size = sizeof(output),
		.pos = 0,
	};

	/* Set up per-list iterator contexts. */
	struct item_iter_ctx iter_a = {
		.items = list_a_items, .count = 1, .pos = 0,
	};
	struct item_iter_ctx iter_b = {
		.items = list_b_items, .count = 2, .pos = 0,
	};
	struct item_iter_ctx iter_c = {
		.items = list_c_items, .count = 1, .pos = 0,
	};

	/*
	 * Each list field now has its own iterator slot in the IO struct,
	 * so we can independently drive list_a, list_b and list_c.
	 */
	struct cbor_stream_io_container_t io = { 0 };
	io.list_a_items_t_item_t_m.ctx  = &iter_a;
	io.list_a_items_t_item_t_m.next = item_iter_next;
	io.list_b_items_t_item_t_m.ctx  = &iter_b;
	io.list_b_items_t_item_t_m.next = item_iter_next;
	io.list_c_items_t_item_t_m.ctx  = &iter_c;
	io.list_c_items_t_item_t_m.next = item_iter_next;

	int rc = cbor_stream_encode_container_t(stream_write, &ctx, &container,
						&io, &out_len);
	zassert_equal(ZCBOR_SUCCESS, rc, "stream encode failed: %d", rc);
	zassert_equal(out_len, ctx.pos, NULL);

	/* Decode the encoded output and verify contents. */
	struct container_t decoded = { 0 };
	size_t decode_len;

	zassert_equal(ZCBOR_SUCCESS,
		      cbor_decode_container_t(output, out_len,
					      &decoded, &decode_len),
		      "decode failed");

	/* Verify name */
	zassert_equal(4, decoded.name.len, NULL);
	zassert_mem_equal("test", decoded.name.value, 4, NULL);

	/* Verify list_a: [{1,10}] */
	zassert_true(decoded.list_a_present, "list_a should be present");
	zassert_equal(1, decoded.list_a.item_t_m_count, NULL);
	zassert_equal(1, decoded.list_a.item_t_m[0].id, NULL);
	zassert_equal(10, decoded.list_a.item_t_m[0].value, NULL);

	/* Verify list_b: [{2,20},{3,30}] */
	zassert_true(decoded.list_b_present, "list_b should be present");
	zassert_equal(2, decoded.list_b.item_t_m_count, NULL);
	zassert_equal(2, decoded.list_b.item_t_m[0].id, NULL);
	zassert_equal(20, decoded.list_b.item_t_m[0].value, NULL);
	zassert_equal(3, decoded.list_b.item_t_m[1].id, NULL);
	zassert_equal(30, decoded.list_b.item_t_m[1].value, NULL);

	/* Verify list_c: [{4,40}] */
	zassert_true(decoded.list_c_present, "list_c should be present");
	zassert_equal(1, decoded.list_c.item_t_m_count, NULL);
	zassert_equal(4, decoded.list_c.item_t_m[0].id, NULL);
	zassert_equal(40, decoded.list_c.item_t_m[0].value, NULL);
}

ZTEST_SUITE(cbor_encode_test5, NULL, NULL, NULL, NULL, NULL);
