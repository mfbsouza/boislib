// SPDX-License-Identifier: MIT
/*
 * This file is part of boislib,
 * a Collection of portable libraries to extended the C ecosystem.
 */

#ifndef __BOISLIB_ALLOCATOR_H__
#define __BOISLIB_ALLOCATOR_H__

/* This code implements a contiguous memory managing using
 * a implicit free list with bidirectional coalescing */

/*
				Heap
													 Block
	 +-------------------------+			 +------------+-------+
	 |                         |	  Header | Block Size |  a/f  | 2 bytes
											 +------------+-------+
	+---------+       +---------+			 |  Payload           |
	|         |       |         |			 +--------------------+
	|  Block  | . . . |  Block  |			 |  Padding (optional)|
	|         |       |         |			 +------------+-------+
	+---------+       +---------+	  Footer | Block Size |  a/f  | 2 bytes
											 +------------+-------+

			Free Heap
	 +------------+-------+                  +------------+-------+
	 |  48 bytes  | 0 0 F |                  |   8 bytes  | 0 0 a |
	 +------------+-------+                  +------------+-------+
	 |                    |                  |      payload       |
	 |                    |                  +--------------------+
	 |                    |                  |      padding       |
	 |                    |                  +------------+-------+
	 |                    |     alloc(1)     |   8 bytes  | 0 0 a |
	 |                    |  ------------->  +------------+-------+
	 |                    |                  |  32 bytes  | 0 0 f |
	 |                    |                  +------------+-------+
	 |                    |                  |                    |
	 |                    |                  |                    |
	 |                    |                  |                    |
	 +------------+-------+                  +------------+-------+
	 |  48 bytes  | 0 0 F |                  |  32 bytes  | 0 0 f |
	 +------------+-------+                  +------------+-------+

*/

#include <stddef.h>

/**
 * @brief the memory manager context struct contains information about the
 * Fake Heap
 *
 * @param *start: The start address of a continuous amount of memory
 * @param *end: The last usable address of the memory manager
 */
struct mem {
	void* start;
	void* end;
};

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * @brief initializes a given memory region as a heap
 *
 * @param *mem_ctx: the allocator context struct
 * @param *start: The start address of a contiguous amount of memory
 * @param size: how many bytes this memory region has
 */
void allocator_init(struct mem* mem_ctx, void* start, size_t size);

/**
 * @brief allocates a contiguous memory region
 *
 * @param *mem_ctx: the memory manager context struct
 * @param size: how many bytes to allocate
 *
 * @retval the start address of the allocated memory region
 */
void* allocator_new(struct mem* mem_ctx, size_t size);

/**
 * @brief frees a continuous memory region
 *
 * @param *mem_ctx: the memory manager context struct
 * @param *addr: the address of the allocated region
 */
void allocator_delete(struct mem* mem_ctx, void* addr);

/**
 * @brief computes the remaining free bytes
 *
 * @param *mem_ctx: the memory manager context struct
 *
 * @retval the amount of free bytes
 */
size_t allocator_remaining(struct mem* mem_ctx);

#if defined(__cplusplus)
}
#endif

#endif /* __BOISLIB_ALLOCATOR_H__ */
