// SPDX-License-Identifier: MIT
/*
 * This file is part of boislib,
 * a Collection of portable libraries to extended the C ecosystem.
 */

#include <gtest/gtest.h>
#include <cstdint>
#include "../circular_queue.h"

constexpr unsigned int buf_size = 256;
constexpr unsigned int elmt_size = sizeof(int);
constexpr unsigned int max_elmts = buf_size / elmt_size;

class CircularQueueTests : public testing::Test {
	protected:
	struct queue queue;
	uint8_t* buf;

	void SetUp() override {
		buf = new uint8_t[buf_size];
		queue_init(&queue, buf, sizeof(int), buf_size);
	}

	void TearDown() override { delete[] buf; }
};

TEST_F(CircularQueueTests, Init) {
	ASSERT_EQ(queue.start, buf);
	ASSERT_EQ(queue.elmt_size, elmt_size);
	ASSERT_EQ(queue.max_elmts, max_elmts);
	ASSERT_EQ(queue.head, 0);
	ASSERT_EQ(queue.tail, 0);
	ASSERT_EQ(queue.elmt_cnt, 0);
}

TEST_F(CircularQueueTests, Alloc) {
	void* ret;
	ret = queue_alloc(&queue);
	ASSERT_EQ(ret, buf);
	ret = queue_alloc(&queue);
	ASSERT_EQ(ret, (uint8_t*)buf + elmt_size);
}

TEST_F(CircularQueueTests, Push) {
	int var = 10;
	queue_push(&queue, &var);
	ASSERT_EQ(*(int*)buf, var);
}

TEST_F(CircularQueueTests, Peek) {
	void* ret;
	ret = queue_peek(&queue);
	ASSERT_EQ(ret, nullptr);
	ret = queue_alloc(&queue);
	ret = NULL;
	ret = queue_peek(&queue);
	ASSERT_EQ(ret, buf);
}

TEST_F(CircularQueueTests, Pop) {
	void* ret;
	int var = 10;
	queue_push(&queue, &var);
	ret = queue_pop(&queue);
	ASSERT_EQ(*(int*)ret, var);
}

TEST_F(CircularQueueTests, Empty) {
	bool ret = queue_empty(&queue);
	ASSERT_EQ(ret, true);
}

TEST_F(CircularQueueTests, Full) {
	void* ret;
	size_t i;
	for (i = 0; i < max_elmts; i++) {
		ret = queue_alloc(&queue);
		ASSERT_NE(ret, nullptr);
	}
	ret = queue_alloc(&queue);
	ASSERT_EQ(ret, nullptr);
}

TEST_F(CircularQueueTests, Remaining) {
	size_t ret = queue_remaining(&queue);
	ASSERT_EQ(ret, max_elmts);
}
