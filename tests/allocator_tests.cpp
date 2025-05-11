// SPDX-License-Identifier: MIT
/*
 * This file is part of boislib,
 * a Collection of portable libraries to extended the C ecosystem.
 */

#include <gtest/gtest.h>
#include <cstdint>

#include "boislib/allocator.h"

constexpr unsigned int header_size = 2;
constexpr unsigned int footer_size = 2;
constexpr unsigned int min_block_size = 8;
constexpr unsigned int max_block_size = (0xFFFF ^ 0b111);

constexpr unsigned int tiny_buf_size = min_block_size * 3;
constexpr unsigned int small_buf_size = 256;
constexpr unsigned int medium_buf_size = max_block_size;
constexpr unsigned int big_buf_size = (max_block_size * 2) + 2;

class MemMgrInitTests : public testing::Test {
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

class MemMgrAllocateTests : public testing::Test {
	protected:
	struct mem mem;
	uint8_t* small_buf;

	void SetUp() override {
		small_buf = new uint8_t[small_buf_size];
		allocator_init(&mem, (void*)small_buf, small_buf_size);
	}

	void TearDown() override { delete[] small_buf; }
};

class MemMgrFreeTests : public testing::Test {
	protected:
	struct mem mem;
	uint8_t* tiny_buf;
	void *fst_blk, *sec_blk, *trd_blk;
	size_t size = min_block_size - header_size - footer_size;

	void SetUp() override {
		tiny_buf = new uint8_t[tiny_buf_size];
		allocator_init(&mem, (void*)tiny_buf, tiny_buf_size);
		fst_blk = allocator_new(&mem, size);
		sec_blk = allocator_new(&mem, size);
		trd_blk = allocator_new(&mem, size);
	}

	void TearDown() override { delete[] tiny_buf; }
};

class MemMgrRemainingTests : public testing::Test {
	protected:
	struct mem mem;
	uint8_t* tiny_buf;
	void* fst_blk;
	size_t size = min_block_size - header_size - footer_size;

	void SetUp() override {
		tiny_buf = new uint8_t[tiny_buf_size];
		allocator_init(&mem, (void*)tiny_buf, tiny_buf_size);
		fst_blk = allocator_new(&mem, size);
		allocator_new(&mem, size);
		allocator_delete(&mem, fst_blk);
	}

	void TearDown() override { delete[] tiny_buf; }
};

TEST_F(MemMgrInitTests, SmallMemory) {
	allocator_init(&mem, (void*)small_buf, small_buf_size);
	ASSERT_EQ(mem.start, (void*)small_buf);
	ASSERT_EQ(mem.end, (void*)&small_buf[small_buf_size]);
	ASSERT_EQ(*(uint16_t*)small_buf, small_buf_size);
	ASSERT_EQ(*(uint16_t*)&small_buf[small_buf_size - header_size],
			  small_buf_size);
}

TEST_F(MemMgrInitTests, MediumMemory) {
	allocator_init(&mem, (void*)medium_buf, medium_buf_size);
	ASSERT_EQ(mem.start, (void*)medium_buf);
	ASSERT_EQ(mem.end, (void*)&medium_buf[medium_buf_size]);
	ASSERT_EQ(*(uint16_t*)medium_buf, medium_buf_size);
	ASSERT_EQ(*(uint16_t*)&medium_buf[medium_buf_size - header_size],
			  medium_buf_size);
}

TEST_F(MemMgrInitTests, BigMemory) {
	allocator_init(&mem, (void*)big_buf, big_buf_size);
	ASSERT_EQ(mem.start, (void*)big_buf);
	ASSERT_EQ(mem.end, (void*)&big_buf[big_buf_size - 2]);
	ASSERT_EQ(*(uint16_t*)big_buf, max_block_size);
	ASSERT_EQ(*(uint16_t*)&big_buf[max_block_size - header_size],
			  max_block_size);
	ASSERT_EQ(*(uint16_t*)&big_buf[max_block_size], max_block_size);
	ASSERT_EQ(*(uint16_t*)&big_buf[big_buf_size - 2 - header_size],
			  max_block_size);
}

TEST_F(MemMgrAllocateTests, TryMoreThanBufferSize) {
	void* ret;
	ret = allocator_new(&mem, small_buf_size);
	ASSERT_EQ(ret, nullptr);
}

TEST_F(MemMgrAllocateTests, AllMemoryInOneBlock) {
	void* ret;
	int size = small_buf_size - header_size - footer_size;
	uint8_t* first_usable_byte = (uint8_t*)mem.start + header_size;
	void* allocated_header = mem.start;
	void* allocated_footer = (void*)((uint8_t*)mem.end - footer_size);

	ret = allocator_new(&mem, size);
	ASSERT_EQ(ret, (void*)first_usable_byte);
	ASSERT_EQ((small_buf_size | 0b1), *(uint16_t*)allocated_header);
	ASSERT_EQ((small_buf_size | 0b1), *(uint16_t*)allocated_footer);
}

TEST_F(MemMgrAllocateTests, AllMemoryInMinimumBlockSize) {
	int i;
	int size = min_block_size - header_size - footer_size;
	int remaining_size = small_buf_size;
	int block_amount = small_buf_size / min_block_size;
	uint8_t* first_usable_byte = (uint8_t*)mem.start + header_size;
	void *ret, *allocated_header, *allocated_footer;
	void *free_header, *free_footer;

	for (i = 0; i < block_amount; i++) {
		ret = allocator_new(&mem, size);
		ASSERT_EQ(ret, (void*)first_usable_byte);

		allocated_header = (void*)((uint8_t*)ret - header_size);
		ASSERT_EQ((min_block_size | 0b1), *(uint16_t*)allocated_header);

		allocated_footer = (void*)((uint8_t*)allocated_header +
								   (min_block_size - footer_size));
		ASSERT_EQ((min_block_size | 0b1), *(uint16_t*)allocated_footer);

		if (i < block_amount - 1) {
			free_header = (void*)((uint8_t*)allocated_footer + footer_size);
			ASSERT_EQ((remaining_size - min_block_size),
					  *(uint16_t*)free_header);

			free_footer =
				(void*)((uint8_t*)free_header +
						(remaining_size - min_block_size - footer_size));
			ASSERT_EQ((remaining_size - min_block_size),
					  *(uint16_t*)free_footer);

			first_usable_byte = (uint8_t*)((uint8_t*)free_header + header_size);
			remaining_size -= min_block_size;
		}
	}
}

TEST_F(MemMgrFreeTests, WrongAddress) {
	int i;
	allocator_delete(&mem, &i);
	ASSERT_EQ((min_block_size | 0b1), *(uint16_t*)&tiny_buf[0]);
	ASSERT_EQ((min_block_size | 0b1),
			  *(uint16_t*)&tiny_buf[tiny_buf_size - footer_size]);
}

TEST_F(MemMgrFreeTests, NoCoalescing) {
	allocator_delete(&mem, sec_blk);
	ASSERT_EQ(min_block_size, *(uint16_t*)&tiny_buf[min_block_size]);
	ASSERT_EQ(min_block_size,
			  *(uint16_t*)&tiny_buf[min_block_size * 2 - footer_size]);
}

TEST_F(MemMgrFreeTests, Coalescing) {
	allocator_delete(&mem, fst_blk);
	allocator_delete(&mem, trd_blk);
	allocator_delete(&mem, sec_blk);
	ASSERT_EQ(tiny_buf_size, *(uint16_t*)&tiny_buf[0]);
	ASSERT_EQ(tiny_buf_size,
			  *(uint16_t*)&tiny_buf[tiny_buf_size - footer_size]);
}

TEST_F(MemMgrFreeTests, AllReadyFreed) {
	allocator_delete(&mem, fst_blk);
	allocator_delete(&mem, trd_blk);
	allocator_delete(&mem, sec_blk);
	allocator_delete(&mem, sec_blk);
	ASSERT_EQ(tiny_buf_size, *(uint16_t*)&tiny_buf[0]);
	ASSERT_EQ(tiny_buf_size,
			  *(uint16_t*)&tiny_buf[tiny_buf_size - footer_size]);
}

TEST_F(MemMgrRemainingTests, MiddleBlockAllocated) {
	size_t remaining_size = allocator_remaining(&mem);
	ASSERT_EQ(remaining_size,
			  (min_block_size * 2 - (header_size + footer_size) * 2));
}
