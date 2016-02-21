#include "containers.h"
#include <stdio.h>

uint32_t hash_fnv1(uint32_t key)
{
	const uint32_t prime = 16777619u;
	uint32_t result = 2166136261u;
	result *= prime; result ^=  key        & 0xff;
	result *= prime; result ^= (key >>  8) & 0xff;
	result *= prime; result ^= (key >> 16) & 0xff;
	result *= prime; result ^= (key >> 24) & 0xff;
	return result;
}

uint32_t hashtable_t::index(uint32_t key, bool next_free) const
{
	// linear probing
	uint32_t index = hash_fnv1(key) % count;
	for(uint32_t i = 0, j = 0; i < count; ++i) // wrapping once
	{
		j = (i + index) % count;
		if(!next_free && arr[j].taken() && arr[j].key == key || next_free && !arr[j].taken())
			return j;
	}
	return invalid;
}

bool hashtable_t::set(uint32_t key, uint32_t value)
{
	uint32_t i = index(key, true);
	if(i != invalid)
	{
		arr[i].key = key;
		arr[i].value = value;
		return true;
	}
	return false;
}

uint32_t hashtable_t::get(uint32_t key) const
{
	uint32_t i = index(key);
	return (i != invalid) ? arr[i].value : invalid;
}

void hashtable_t::remove(uint32_t key)
{
	uint32_t i = index(key);
	if(i != invalid)
		arr[i].free();
}

void hashtable_t::print() const
{
	uint32_t taken = 0;
	for(uint32_t i = 0; i < count; ++i)
	{
		if(arr[i].taken())
		{
			++taken;
			printf("%03u -> (%u %u)\n", i, arr[i].key, arr[i].value);
		}
		else
			printf("%03u -> free\n", i);
	}
	printf("load factor : %.3f\n", (float)taken / (float)count);
}

uint32_t btree_t::next_free() const
{
	for(uint32_t i = 0; i < count; ++i)
		if(!arr[i].taken())
			return i;
	return invalid;
}

bool btree_t::set(uint32_t key, uint32_t value)
{
	return false;
}

void btree_t::remove(uint32_t value)
{
}

uint32_t btree_t::get(uint32_t key) const
{
	return invalid;
}
