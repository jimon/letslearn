#pragma once

#include <stdint.h>
#include <initializer_list>

// to simplify memory management, access patterns, etc
// let's just do array of integers
struct dataset_t
{
	uint32_t items[256] = {0};
	size_t count = 0;

	dataset_t() = default;
	dataset_t(std::initializer_list<uint32_t> init);
	static dataset_t random(size_t count = 0);

	inline void swap(size_t a, size_t b) {uint32_t t = items[a]; items[a] = items[b]; items[b] = t;}
	void print() const;
	bool validate() const;
};
