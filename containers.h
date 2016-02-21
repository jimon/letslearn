#pragma once

#include <stdint.h>

uint32_t hash_fnv1(uint32_t key);

struct hashtable_t
{
	static const uint32_t invalid = 0xffffffffu;

	struct entry_t
	{
		uint32_t key = invalid;
		uint32_t value = invalid;

		inline void free() {key = invalid; value = invalid;}
		inline bool taken() const {return key != invalid || value != invalid;}
	};

	// to simplify memory management let's say
	// hashtable can only contain up to N elements
	static const uint32_t count = 256;
	entry_t arr[count];

	uint32_t index(uint32_t key, bool next_free = false) const;
	bool contains(uint32_t key) const {return index(key) != invalid;}
	bool set(uint32_t key, uint32_t value);
	uint32_t get(uint32_t key) const;
	void remove(uint32_t key);

	void print() const;
};
