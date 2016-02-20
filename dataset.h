#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <initializer_list>

// to simplify memory management, access patterns, etc
// let's just do array of integers
struct dataset_t
{
	int32_t items[256] = {0};
	size_t count = 0;

	dataset_t() {}
	dataset_t(std::initializer_list<int32_t> init)
	{
		assert(init.size() < sizeof(items) / sizeof(items[0]));
		for(int32_t data: init)
			items[count++] = data;
	}

	inline void swap(size_t a, size_t b) {int32_t t = items[a]; items[a] = items[b]; items[b] = t;}

	static dataset_t random()
	{
		dataset_t result;
		result.count = rand() % (sizeof(items) / sizeof(items[0]));
		for(size_t i = 0; i < result.count; ++i)
			result.items[i] = rand();
		return result;
	}

	void print() const
	{
		printf("%zu: {", count); // TODO
		for(size_t i = 0; i < count; ++i)
			printf("%i%s", items[i], i + 1 == count ? "" : ", ");
		printf("}\n");
	}

	bool validate() const
	{
		int32_t last = INT32_MIN;
		for(size_t i = 0; i < count; ++i)
			if(last <= items[i])
				last = items[i];
			else
			{
				print();
				assert(false);
				return false;
			}
		return true;
	}
};
