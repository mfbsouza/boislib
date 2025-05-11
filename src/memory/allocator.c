// SPDX-License-Identifier: MIT
/*
 * This file is part of boislib,
 * a Collection of portable libraries to extended the C ecosystem.
 */

#include "allocator.h"
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
static void* coalesce_block(const struct mem* mem_ctx, void* start);
static inline void alloc_block(void* start);
static inline void free_block(void* start);

void allocator_init(struct mem* mem_ctx, void* start, size_t size) {
	assert(mem_ctx);
	assert(start);
	assert(size >= MIN_BLOCK_SIZE);

	mem_ctx->start = start;
	mem_ctx->end = create_block(start, size);
}

void* allocator_new(struct mem* mem_ctx, size_t size) {
	assert(mem_ctx);
	assert(size > 0);

	size_t chunk_size;
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
		chunk_size = GET_SIZE(ptr);
		create_block(ptr, size);
		/* check if it needs to break the chunk in two blocks */
		if (size < chunk_size) {
			create_block((ptr + size), (chunk_size - size));
		}
		alloc_block(ptr);
		ret = (void*)(ptr + HEADER_SIZE);
	}
	return ret;
}

void allocator_delete(struct mem* mem_ctx, void* addr) {
	assert(mem_ctx);
	assert(addr);

	uint8_t* ptr = (uint8_t*)addr;
	uint8_t* start = (uint8_t*)mem_ctx->start;
	uint8_t* end = (uint8_t*)mem_ctx->end;

	/* make sure that the address is inside this heap */
	if (ptr <= start || ptr >= end)
		return;

	/* make sure that the block is allocated */
	ptr -= HEADER_SIZE;
	if (!IS_ALLOCATED(ptr))
		return;

	/* frees the block */
	free_block(ptr);
	coalesce_block(mem_ctx, ptr);
}

size_t allocator_remaining(struct mem* mem_ctx) {
	assert(mem_ctx);
	size_t size = 0;
	uint8_t* ptr = (uint8_t*)mem_ctx->start;
	uint8_t* end = (uint8_t*)mem_ctx->end;

	while (ptr < end) {
		if (!IS_ALLOCATED(ptr))
			size += (GET_SIZE(ptr) - METADATA_SIZE);
		ptr += GET_SIZE(ptr);
	}

	return size;
}

static void* create_block(void* start, size_t size) {
	void* header = NULL;
	void* footer = NULL;
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
	footer = (uint8_t*)end - FOOTER_SIZE;
	SET_SIZE(header, size);
	SET_SIZE(footer, size);

	return create_block(end, remainder);
}

static void* coalesce_block(const struct mem* mem_ctx, void* start) {
	void *new_header, *new_footer;
	uint8_t is_prev_allocated, is_next_allocated;

	size_t block_size = GET_SIZE(start);
	uint8_t* prev_blk_footer = (uint8_t*)start - HEADER_SIZE;
	uint8_t* next_blk_header = (uint8_t*)start + block_size;
	uint8_t* begin = (uint8_t*)mem_ctx->start;
	uint8_t* end = (uint8_t*)mem_ctx->end;

	if (prev_blk_footer > begin) {
		is_prev_allocated = IS_ALLOCATED(prev_blk_footer);
	} else {
		is_prev_allocated = 1;
	}
	if (next_blk_header < end) {
		is_next_allocated = IS_ALLOCATED(next_blk_header);
	} else {
		is_next_allocated = 1;
	}

	if (is_prev_allocated && is_next_allocated) {
		return NULL;
	}

	if (!is_prev_allocated) {
		new_footer = (uint8_t*)start + (block_size - FOOTER_SIZE);
		block_size += GET_SIZE(prev_blk_footer);
		new_header = (uint8_t*)start - GET_SIZE(prev_blk_footer);
	} else if (!is_next_allocated) {
		new_header = start;
		block_size += GET_SIZE(next_blk_header);
		new_footer = (uint8_t*)start + (block_size - FOOTER_SIZE);
	}
	SET_SIZE(new_header, block_size);
	SET_SIZE(new_footer, block_size);

	return coalesce_block(mem_ctx, new_header);
}

static inline void alloc_block(void* start) {
	size_t block_size = GET_SIZE(start);
	void* header = start;
	void* footer = (uint8_t*)start + (block_size - FOOTER_SIZE);
	SET_SIZE(header, ALLOCATE(block_size));
	SET_SIZE(footer, ALLOCATE(block_size));
}

static inline void free_block(void* start) {
	size_t block_size = GET_SIZE(start);
	void* header = start;
	void* footer = (uint8_t*)start + (block_size - FOOTER_SIZE);
	SET_SIZE(header, block_size);
	SET_SIZE(footer, block_size);
}
