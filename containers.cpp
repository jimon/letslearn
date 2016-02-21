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

linkedlist_t::linkedlist_t()
{
	for(uint32_t i = 0; i < count; ++i)
	{
		arr[i].prev = i == 0 ? count - 1 : i - 1;
		arr[i].next = i == count - 1 ? 0 : i + 1;
	}
}

uint32_t linkedlist_t::allocate()
{
	uint32_t result = next_free;
	next_free = arr[result].next == result ? invalid : arr[result].next;
	arr[arr[result].prev].next = arr[result].next;
	arr[arr[result].next].prev = arr[result].prev;
	return result;
}

void linkedlist_t::deallocate(uint32_t index)
{
	arr[index].free();

	if(next_free != invalid)
	{
		arr[index].prev = next_free;
		arr[index].next = arr[next_free].next;
		arr[arr[next_free].next].prev = index;
		arr[next_free].next = index;
	}
	else
	{
		arr[index].next = index;
		arr[index].prev = index;
		next_free = index;
	}
}

uint32_t linkedlist_t::insert_after(uint32_t value, uint32_t index)
{
	index = index == invalid ? last_index : index;
	uint32_t new_index = allocate();
	if(new_index == invalid)
		return invalid;

	if(index != invalid)
	{
		arr[new_index].prev = index;
		arr[new_index].next = arr[index].next;
		arr[arr[index].next].prev = new_index;
		arr[index].next = new_index;
	}
	else
	{
		arr[new_index].prev = new_index;
		arr[new_index].next = new_index;
	}

	arr[new_index].value = value;
	last_index = new_index;
	return new_index;
}

uint32_t linkedlist_t::insert_before(uint32_t value, uint32_t index)
{
	index = index == invalid ? last_index : index;
	uint32_t new_index = allocate();
	if(new_index == invalid)
		return invalid;

	if(index != invalid)
	{
		arr[new_index].next = index;
		arr[new_index].prev = arr[index].prev;
		arr[arr[index].prev].next = new_index;
		arr[index].prev = new_index;
	}
	else
	{
		arr[new_index].prev = new_index;
		arr[new_index].next = new_index;
	}

	arr[new_index].value = value;
	last_index = new_index;
	return new_index;
}

void linkedlist_t::remove(uint32_t index)
{
	if(index != invalid)
	{
		if(last_index == index)
			last_index = arr[index].prev != index ? arr[index].prev : invalid;
		arr[arr[index].prev].next = arr[index].next;
		arr[arr[index].next].prev = arr[index].prev;
		deallocate(index);
	}
}

void linkedlist_t::print() const
{
	for(uint32_t i = 0; i < count; ++i)
		printf("%u : (%u %u %u)\n", i, arr[i].value, arr[i].prev, arr[i].next);
	printf("next free %u\n", next_free);
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
