// SPDX-License-Identifier: MIT
/*
 * This file is part of boislib,
 * a Collection of portable libraries to extended the C ecosystem.
 */

#ifndef __BOISLIB_CIRCULAR_QUEUE_H__
#define __BOISLIB_CIRCULAR_QUEUE_H__

#include <stdbool.h>
#include <stddef.h>

/**
 * @brief the queue context struct contains information about the queue
 *
 * @param *start: the start address of a continuous amount of memory
 * @param head: the head of the queue
 * @param tail: the tail of the queue
 * @param elmt_size: the element size in bytes of the queue
 * @param elmt_cnt: the current amount of elements in the queue
 * @param max_elmts: the maximum amount of elements the queue can hold
 * @param *end: the last usable address of the queue
 */
struct queue {
	void* start;
	size_t head;
	size_t tail;
	size_t elmt_size;
	size_t elmt_cnt;
	size_t max_elmts;
};

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * @brief initializes the queue to manage a continuous amount of memory
 * by a given queue context struct
 *
 * @param *queue_ctx: the queue context struct
 * @param *start: the start address of a continuous memory location
 * @param elmt_size: the size in bytes of a element in the queue
 * @param buf_size: how many bytes this memory region has
 */
void queue_init(struct queue* queue_ctx,
				void* start,
				size_t elmt_size,
				size_t buf_size);

/**
 * @brief checks if a queue is empty
 *
 * @param *queue_ctx: the queue context struct
 *
 * @retval false or true
 */
bool queue_empty(struct queue* queue_ctx);

/**
 * @brief checks if a queue is full
 *
 * @param *queue_ctx: the queue context struct
 *
 * @retval false or true
 */
bool queue_full(struct queue* queue_ctx);

/**
 * @brief request allocation for one element in the queue
 *
 * @param *queue_ctx: the queue context struct
 *
 * @retval the start address allocated for the element
 */
void* queue_alloc(struct queue* queue_ctx);

/**
 * @brief copies a given element into the queue
 *
 * @param *queue_ctx: the queue context struct
 * @param elmt_addr: the start address of the element to be inserted
 *
 * @retval how many bytes were copied
 */
size_t queue_push(struct queue* queue_ctx, void* elmt_addr);

/**
 * @brief peeks the next element to be read from the queue
 *
 * @retval the element address or null if there is no element to read
 */
void* queue_peek(struct queue* queue_ctx);

/**
 * @brief removes a element from the queue
 *
 * @retval the element address or null if there is no element to read
 */
void* queue_pop(struct queue* queue_ctx);

/**
 * @brief gets how many free elements are left in the queue
 *
 * @param *queue_ctx: the queue context struct
 *
 * @retval how many free elements are left in the queue
 */
size_t queue_remaining(struct queue* queue_ctx);

#if defined(__cplusplus)
}
#endif

#endif /* __BOISLIB_CIRCULAR_QUEUE_H__ */
