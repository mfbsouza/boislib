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
#define MAX_BLOCK_SIZE (0xFFFF ^ 0b111)

#define IS_ALLOCATED(x) ((*(uint16_t*)(x)) & 0b1)
#define GET_SIZE(x) ((*(uint16_t*)(x)) & MAX_BLOCK_SIZE)
#define SET_SIZE(x, s) ((*(uint16_t*)(x)) = (s))
#define ALLOCATE(s) ((s) | 0b1)

static void* create_block(void* start, size_t size);

void memmgr_init(struct mem* mem_ctx, void* start, size_t size) {
	assert(mem_ctx);
	assert(start);
	assert(size >= MIN_BLOCK_SIZE);

	mem_ctx->start = start;
	mem_ctx->end = create_block(start, size);
}

void* memmgr_alloc(struct mem* mem_ctx, size_t size) {
	assert(mem_ctx);
	assert(size > 0);

	void* ret = NULL;
	uint8_t* ptr = (uint8_t*)mem_ctx->start;
	uint8_t* end = (uint8_t*)mem_ctx->end;

	/* compute the minimum block size to fit the user requested size */
	size += METADATA_SIZE;
	if (size % BYTE_ALIGN != 0) {
		size += BYTE_ALIGN - (size % BYTE_ALIGN);
	}

	/* look for a free chunk big enough to fit this size */
	while (ptr < end && (IS_ALLOCATED(ptr) || GET_SIZE(ptr) < size)) {
		ptr += GET_SIZE(ptr);
	}

	if (ptr < end) {
		/* check if it needs to break the chunk in two blocks */
		if (size < GET_SIZE(ptr)) {
			/* create the new free chunk */
			SET_SIZE((ptr + size), (GET_SIZE(ptr) - size));
			SET_SIZE((ptr + (GET_SIZE(ptr) - HEADER_SIZE)),
					 (GET_SIZE(ptr) - size));
		}
		/* allocate the block */
		SET_SIZE(ptr, ALLOCATE(size));
		SET_SIZE((ptr + (size - FOOTER_SIZE)), ALLOCATE(size));

		ret = (void*)(ptr + HEADER_SIZE);
	}
	return ret;
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
