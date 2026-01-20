/*
 * Copyright (c) 2026
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Generated using zcbor version 0.9.1
 * https://github.com/NordicSemiconductor/zcbor
 * Generated with a --default-max-qty of 3
 */

#ifndef STREAMING_CHUNKS_TYPES_H__
#define STREAMING_CHUNKS_TYPES_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <zcbor_common.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Which value for --default-max-qty this file was created with.
 *
 *  The define is used in the other generated file to do a build-time
 *  compatibility check.
 *
 *  See `zcbor --help` for more information about --default-max-qty
 */
#define ZCBOR_GENERATED_DEFAULT_MAX_QTY 3

/* Allow build-system override. */
#ifndef DEFAULT_MAX_QTY
#define DEFAULT_MAX_QTY ZCBOR_GENERATED_DEFAULT_MAX_QTY
#endif

/* Allow build-system override for streaming state array size. */
#ifndef ZCBOR_STREAM_STATE_ARRAY_SIZE
#define ZCBOR_STREAM_STATE_ARRAY_SIZE 8
#endif

struct StreamItem {
	struct zcbor_string StreamItem_name;
	struct zcbor_string StreamItem_payload;
};

#ifdef __cplusplus
}
#endif

#endif /* STREAMING_CHUNKS_TYPES_H__ */
