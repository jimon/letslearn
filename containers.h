#pragma once

#include <stdint.h>

uint32_t hash_fnv1(uint32_t key);

struct linkedlist_t
{
	static const uint32_t invalid = 0xffffffffu;

	struct node_t
	{
		uint32_t value = invalid;
		uint32_t prev = invalid;
		uint32_t next = invalid;
		inline void free() {value = invalid;}
		inline bool taken() const {return value != invalid;}
	};

	// to simplify memory management let's say
	// linked list can only contain up to N elements
	static const uint32_t count = 7;
	node_t arr[count];
	uint32_t last_index = invalid;
	uint32_t next_free = 0;

	linkedlist_t();
	uint32_t allocate();
	void deallocate(uint32_t index);
	uint32_t last() const {return last_index;}
	uint32_t next(uint32_t index) const {return index != invalid ? arr[index].next : invalid;}
	uint32_t prev(uint32_t index) const {return index != invalid ? arr[index].prev: invalid;}
	uint32_t value(uint32_t index) const {return index != invalid ? arr[index].value: invalid;}
	uint32_t insert(uint32_t value) {return insert_after(value, last());}
	uint32_t insert_after(uint32_t value, uint32_t index);
	uint32_t insert_before(uint32_t value, uint32_t index);
	void remove(uint32_t index);
	void print() const;
};

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

struct rbtree_t
{
	static const uint32_t invalid = 0xffffffffu;

	struct node_t
	{
		uint32_t key = invalid;
		uint32_t value = invalid;
		uint32_t left = invalid;
		uint32_t right = invalid;
		uint32_t parent = invalid;
		bool color = false; // 0 black, 1 red

		inline void free()
		{
			key = invalid;
			value = invalid;
			left = invalid;
			right = invalid;
			parent = invalid;
			color = false;
		}
		inline bool taken() const {return key != invalid;}
	};

	// to simplify memory management let's say
	// rb tree can only contain up to N elements
	static const uint32_t count = 256;
	node_t arr[count];
	uint32_t root = invalid;

	// public
	uint32_t get(uint32_t key) const;
	void set(uint32_t key, uint32_t value);
	void remove(uint32_t key);

	// private
	uint32_t allocate();

	uint32_t parent(uint32_t index) const;
	uint32_t left(uint32_t index) const;
	uint32_t right(uint32_t index) const;
	uint32_t grandparent(uint32_t index) const;
	uint32_t sibling(uint32_t index) const;
	uint32_t uncle(uint32_t index) const;
	bool color(uint32_t index) const;
	void set_color(uint32_t index, bool color);
	bool validate() const;

	uint32_t find_index(uint32_t key) const;
	void swap(uint32_t old_node, uint32_t new_node);
	void rotate_left(uint32_t index);
	void rotate_right(uint32_t index);

	void balance(uint32_t index);
	void rebalance(uint32_t index);
	void print(uint32_t height = 0) const;
};
