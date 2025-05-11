// SPDX-License-Identifier: MIT
/*
 * This file is part of boislib,
 * a Collection of portable libraries to extended the C ecosystem.
 */

#include "circular_queue.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

void queue_init(struct queue* queue_ctx,
				void* start,
				size_t elmt_size,
				size_t buf_size) {
	assert(queue_ctx);
	assert(start);
	assert(elmt_size > 0);
	assert(buf_size > 0);

	queue_ctx->start = start;
	queue_ctx->max_elmts = buf_size / elmt_size;
	queue_ctx->elmt_size = elmt_size;
	queue_ctx->head = queue_ctx->tail = queue_ctx->elmt_cnt = 0;
}

void* queue_alloc(struct queue* queue_ctx) {
	assert(queue_ctx);
	void* ret = NULL;

	if (queue_full(queue_ctx)) {
		ret = NULL;
	} else {
		ret = (uint8_t*)queue_ctx->start +
			  (queue_ctx->tail * queue_ctx->elmt_size);
		queue_ctx->tail = (queue_ctx->tail + 1) % queue_ctx->max_elmts;
		queue_ctx->elmt_cnt += 1;
	}
	return ret;
}

size_t queue_push(struct queue* queue_ctx, void* elmt_addr) {
	assert(queue_ctx);
	assert(elmt_addr);
	size_t ret = 0;
	void* dest = NULL;

	if ((dest = queue_alloc(queue_ctx)) != NULL) {
		memcpy(dest, elmt_addr, queue_ctx->elmt_size);
		ret = queue_ctx->elmt_size;
	}
	return ret;
}

void* queue_pop(struct queue* queue_ctx) {
	assert(queue_ctx);
	void* ret = NULL;

	if ((ret = queue_peek(queue_ctx)) != NULL) {
		queue_ctx->head = (queue_ctx->head + 1) % queue_ctx->max_elmts;
		queue_ctx->elmt_cnt -= 1;
	}
	return ret;
}

void* queue_peek(struct queue* queue_ctx) {
	assert(queue_ctx);
	void* ret = NULL;

	if (queue_empty(queue_ctx)) {
		ret = NULL;
	} else {
		ret = (uint8_t*)queue_ctx->start +
			  (queue_ctx->head * queue_ctx->elmt_size);
	}
	return ret;
}

bool inline queue_empty(struct queue* queue_ctx) {
	assert(queue_ctx);
	if (queue_ctx->elmt_cnt == 0) {
		return true;
	}
	return false;
}

bool inline queue_full(struct queue* queue_ctx) {
	assert(queue_ctx);
	if (queue_ctx->elmt_cnt == queue_ctx->max_elmts) {
		return true;
	}
	return false;
}

size_t inline queue_remaining(struct queue* queue_ctx) {
	assert(queue_ctx);
	return queue_ctx->max_elmts - queue_ctx->elmt_cnt;
}
