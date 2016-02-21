#include "dataset.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

dataset_t::dataset_t(std::initializer_list<uint32_t> init)
{
	assert(init.size() < sizeof(items) / sizeof(items[0]));
	for(uint32_t data: init)
		items[count++] = data;
}
dataset_t dataset_t::random(size_t count)
{
	dataset_t result;
	result.count = count ? count : rand() % (sizeof(items) / sizeof(items[0]));
	for(size_t i = 0; i < result.count; ++i)
		result.items[i] = rand();
	return result;
}

void dataset_t::print() const
{
	printf("%zu: {", count); // TODO
	for(size_t i = 0; i < count; ++i)
		printf("%u%s", items[i], i + 1 == count ? "" : ", ");
	printf("}\n");
}

bool dataset_t::validate() const
{
	uint32_t last = 0;
	for(size_t i = 0; i < count; ++i)
		if(last <= items[i])
			last = items[i];
		else
			return false;
	return true;
}
