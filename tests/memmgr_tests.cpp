// SPDX-License-Identifier: MPL-2.0

#include <gtest/gtest.h>
#include <cstdint>
#include "../src/memory/memmgr.h"

constexpr unsigned int header_size = 2;
constexpr unsigned int max_block_size = 65528;
constexpr unsigned int small_buf_size = 256;
constexpr unsigned int medium_buf_size = max_block_size;
constexpr unsigned int big_buf_size = (medium_buf_size * 2) + 2;

class MemMgrTests : public testing::Test {
	protected:
	struct mem mem;
	uint8_t* small_buf;
	uint8_t* medium_buf;
	uint8_t* big_buf;

	void SetUp() override {
		small_buf = new uint8_t[small_buf_size];
		medium_buf = new uint8_t[medium_buf_size];
		big_buf = new uint8_t[big_buf_size];
	}

	void TearDown() override {
		delete[] small_buf;
		delete[] medium_buf;
		delete[] big_buf;
	}
};

TEST_F(MemMgrTests, InitSmallMemory) {
	memmgr_init(&mem, (void*)small_buf, small_buf_size);
	ASSERT_EQ(mem.start, (void*)small_buf);
	ASSERT_EQ(mem.end, (void*)&small_buf[small_buf_size]);
	ASSERT_EQ(*(uint16_t*)small_buf, small_buf_size);
	ASSERT_EQ(*(uint16_t*)&small_buf[small_buf_size - header_size],
			  small_buf_size);
}

TEST_F(MemMgrTests, InitMediumMemory) {
	memmgr_init(&mem, (void*)medium_buf, medium_buf_size);
	ASSERT_EQ(mem.start, (void*)medium_buf);
	ASSERT_EQ(mem.end, (void*)&medium_buf[medium_buf_size]);
	ASSERT_EQ(*(uint16_t*)medium_buf, medium_buf_size);
	ASSERT_EQ(*(uint16_t*)&medium_buf[medium_buf_size - header_size],
			  medium_buf_size);
}

TEST_F(MemMgrTests, InitBigMemory) {
	memmgr_init(&mem, (void*)big_buf, big_buf_size);
	ASSERT_EQ(mem.start, (void*)big_buf);
	ASSERT_EQ(mem.end, (void*)&big_buf[big_buf_size - 2]);
	ASSERT_EQ(*(uint16_t*)big_buf, max_block_size);
	ASSERT_EQ(*(uint16_t*)&big_buf[max_block_size - header_size],
			  max_block_size);
	ASSERT_EQ(*(uint16_t*)&big_buf[max_block_size], max_block_size);
	ASSERT_EQ(*(uint16_t*)&big_buf[big_buf_size - 2 - header_size],
			  max_block_size);
}
