// SPDX-License-Identifier: MPL-2.0

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

#ifndef __MEMMGR_H__
#define __MEMMGR_H__

#include <stddef.h>

/**
 * @brief the memory manager context struct contains information about the
 * Fake Heap
 *
 * @param *heap_start: The start address of a continuous amount of memory
 * @param *heap_end: The last address of a continuous amount of memory
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
 * @param *mem_ctx: the memory manager context struct
 * @param *start: The start address of a contiguous amount of memory
 * @param size: how many bytes this memory region has
 */
void memmgr_init(struct mem* mem_ctx, void* start, size_t size);

/**
 * @brief allocates a contiguous memory region
 *
 * @param *mem_ctx: the memory manager context struct
 * @param size: how many bytes to allocate
 *
 * @retval the start address of the allocated memory region
 */
void* memmgr_alloc(struct mem* mem_ctx, size_t size);

/**
 * @brief frees a continuous memory region
 *
 * @param *mem_ctx: the memory manager context struct
 * @param *addr: the address of allocated region
 */
void memmgr_free(struct mem* mem_ctx, void* ptr);

/**
 * @brief computes the remaining free bytes
 *
 * @param *mem_ctx: the memory manager context struct
 *
 * @retval the amount of free bytes
 */
size_t memmgr_remaining(struct mem* mem_ctx);

#if defined(__cplusplus)
}
#endif

#endif /* __MEMMGR_H__ */