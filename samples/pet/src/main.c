/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zcbor_encode.h>
#include <stdio.h>
#include <string.h>
#include <pet_decode.h>
#include <pet_encode.h>
#include <pet1.h>

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

static void print_pet(const struct Pet *pet)
{
	printf("Name:");
	for (int i = 0; i < pet->names_count; i++) {
		printf(" %.*s", (int)pet->names[i].len, pet->names[i].value);
	}
	printf("\nBirthday: 0x");
	for (int i = 0; i < pet->birthday.len; i++) {
		printf("%02x", pet->birthday.value[i]);
	}
	switch (pet->species_choice) {
	case Pet_species_cat_c:
		printf("\nSpecies: Cat\n\n");
		return;
	case Pet_species_dog_c:
		printf("\nSpecies: Dog\n\n");
		return;
	case Pet_species_other_c:
		printf("\nSpecies: Other\n\n");
		return;
	}
}

/** First pet - from var in pet1.h. */
static void get_pet1(void)
{
	struct Pet decoded_pet;
	int err;

	err = cbor_decode_Pet(pet1, sizeof(pet1), &decoded_pet, NULL);
	if (err != ZCBOR_SUCCESS) {
		printf("Decoding failed for pet1: %d\r\n", err);
		return;
	}

	print_pet(&decoded_pet);
}

/** Second pet - encoded with zcbor C library. */
static void get_pet2(void)
{
	struct Pet decoded_pet;
	int err;
	uint8_t pet2[30];
	ZCBOR_STATE_E(encoding_state, 0, pet2, sizeof(pet2), 0);
	bool r = true;
	const uint8_t timestamp2[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 };

	r = r && zcbor_list_start_encode(encoding_state, 3);
	r = r && zcbor_list_start_encode(encoding_state, 3);
	r = r && zcbor_tstr_put_lit(encoding_state, "Danny");
	r = r && zcbor_tstr_put_lit(encoding_state, "the");
	r = r && zcbor_tstr_put_lit(encoding_state, "Dog");
	r = r && zcbor_list_end_encode(encoding_state, 3);
	r = r && zcbor_bstr_put_arr(encoding_state, timestamp2);
	r = r && zcbor_uint64_put(encoding_state, 2);
	r = r && zcbor_list_end_encode(encoding_state, 3);

	if (!r) {
		printf("Encoding failed for pet2: %d\r\n", zcbor_peek_error(encoding_state));
		return;
	}

	err = cbor_decode_Pet(pet2, sizeof(pet2), &decoded_pet, NULL);

	if (err != ZCBOR_SUCCESS) {
		printf("Decoding failed for pet2: %d\r\n", err);
		return;
	}

	print_pet(&decoded_pet);
}

/** Third pet - encoded with zcbor-generated code. */
static void get_pet3(void)
{
	struct Pet decoded_pet;
	struct Pet encoded_pet;
	int err;
	uint8_t pet3[30];
	const uint8_t first_name[] = "Gary";
	const uint8_t last_name[] = "Giraffe";
	const uint8_t timestamp3[] = { 0x01, 0x02, 0x03, 0x04, 0x0a, 0x0b, 0x0c, 0x0d };

	encoded_pet.names[0].value = first_name;
	encoded_pet.names[0].len = sizeof(first_name) - 1;
	encoded_pet.names[1].value = last_name;
	encoded_pet.names[1].len = sizeof(last_name) - 1;
	encoded_pet.names_count = 2;
	encoded_pet.birthday.value = timestamp3;
	encoded_pet.birthday.len = sizeof(timestamp3);
	encoded_pet.species_choice = Pet_species_other_c;

	err = cbor_encode_Pet(pet3, sizeof(pet3), &encoded_pet, NULL);

	if (err != ZCBOR_SUCCESS) {
		printf("Encoding failed for pet3: %d\r\n", err);
		return;
	}

	err = cbor_decode_Pet(pet3, sizeof(pet3), &decoded_pet, NULL);

	if (err != ZCBOR_SUCCESS) {
		printf("Decoding failed for pet3: %d\r\n", err);
		return;
	}

	print_pet(&decoded_pet);
}

/** Fourth pet - encoded with zcbor-generated streaming entrypoint. */
static void get_pet4_streaming(void)
{
	struct Pet decoded_pet;
	struct Pet encoded_pet;
	int err;
	uint8_t pet4[30];
	size_t out_len = 0;
	struct stream_ctx ctx = {
		.buf = pet4,
		.size = sizeof(pet4),
		.pos = 0,
	};
	const uint8_t first_name[] = "Sammy";
	const uint8_t last_name[] = "Streaming";
	const uint8_t timestamp4[] = { 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x11, 0x22 };

	encoded_pet.names[0].value = first_name;
	encoded_pet.names[0].len = sizeof(first_name) - 1;
	encoded_pet.names[1].value = last_name;
	encoded_pet.names[1].len = sizeof(last_name) - 1;
	encoded_pet.names_count = 2;
	encoded_pet.birthday.value = timestamp4;
	encoded_pet.birthday.len = sizeof(timestamp4);
	encoded_pet.species_choice = Pet_species_cat_c;

	err = cbor_stream_encode_Pet(stream_write, &ctx, &encoded_pet, NULL, &out_len);
	if (err != ZCBOR_SUCCESS || out_len != ctx.pos) {
		printf("Streaming encode failed for pet4: %d\r\n", err);
		return;
	}

	err = cbor_decode_Pet(pet4, out_len, &decoded_pet, NULL);
	if (err != ZCBOR_SUCCESS) {
		printf("Decoding failed for pet4: %d\r\n", err);
		return;
	}

	print_pet(&decoded_pet);
}

int main(void)
{
	get_pet1();
	get_pet2();
	get_pet3();
	get_pet4_streaming();
	return 0;
}
