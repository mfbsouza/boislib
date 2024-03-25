// SPDX-License-Identifier: MPL-2.0

#include "memmgr.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#define BYTE_ALIGN 8
#define HEADER_SIZE 2
#define FOOTER_SIZE HEADER_SIZE
#define METADATA_SIZE HEADER_SIZE * 2
#define MIN_BLOCK_SIZE BYTE_ALIGN
#define MAX_BLOCK_SIZE (0xFF00 | 0xF0 | 0b1000)

static void* create_block(void* start, size_t size);

void memmgr_init(struct mem* mem_ctx, void* start, size_t size) {
	assert(mem_ctx);
	assert(start);
	assert(size >= MIN_BLOCK_SIZE);

	mem_ctx->start = start;
	mem_ctx->end = create_block(start, size);
}

static void* create_block(void* start, size_t size) {
	uint16_t* header = NULL;
	uint16_t* footer = NULL;
	size_t remainder = 0;
	void* end = NULL;

	if (size < MIN_BLOCK_SIZE)
		return start;

	if (size <= MAX_BLOCK_SIZE) {
		remainder = size % BYTE_ALIGN;
		size -= remainder;
	} else {
		remainder = size - MAX_BLOCK_SIZE;
		size = MAX_BLOCK_SIZE;
	}
	end = (void*)((uint8_t*)start + size);

	header = start;
	footer = (void*)((uint8_t*)end - FOOTER_SIZE);
	*header = *footer = (uint16_t)size;

	return create_block(end, remainder);
}
