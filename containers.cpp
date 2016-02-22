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

uint32_t rbtree_t::allocate()
{
	for(uint32_t i = 0; i < count; ++i)
		if(!arr[i].taken())
		{
			arr[i].free();
			return i;
		}
	return invalid;
}

uint32_t rbtree_t::grandparent(uint32_t index) const
{
	return index != invalid && arr[index].parent != invalid ?
			arr[arr[index].parent].parent : invalid;
}

uint32_t rbtree_t::sibling(uint32_t index) const
{
	if(index != invalid && arr[index].parent != invalid)
		return arr[arr[index].parent].left == index ?
				arr[arr[index].parent].right : arr[arr[index].parent].left;
	return invalid;
}

uint32_t rbtree_t::uncle(uint32_t index) const
{
	return index != invalid ? sibling(arr[index].parent) : invalid;
}

bool rbtree_t::color(uint32_t index) const
{
	return index != invalid ? arr[index].color : false;
}

bool rbtree_t::validate() const
{
	// root must be black
	if(root == invalid || arr[root].color)
		return false;

	// adjacent nodes should have different color
	static bool (*check_color)(const rbtree_t*, uint32_t) =
	[](const rbtree_t * tree, uint32_t index) -> bool
	{
		if(index == invalid)
			return true;
		if(tree->color(index) && (
		   tree->color(tree->arr[index].left) ||
		   tree->color(tree->arr[index].right) ||
		   tree->color(tree->arr[index].parent)))
			return false;
		return check_color(tree, tree->arr[index].left) &&
			   check_color(tree, tree->arr[index].right);
	};
	if(!check_color(this, root))
		return false;

	// verify if tree is balanced
	uint32_t target = 1;
	uint32_t left = root;
	while(left != invalid)
		if(!color(left = arr[left].left)) // leaf is when left == invalid
			++target;
	static bool (*check_length)(const rbtree_t*, uint32_t, uint32_t, uint32_t) =
	[](const rbtree_t * tree, uint32_t index, uint32_t current, uint32_t target) -> bool
	{
		if(index == invalid)
			return current + 1 == target;
		if(!tree->color(index))
			++current;
		return check_length(tree, tree->arr[index].left, current, target) &&
			   check_length(tree, tree->arr[index].right, current, target);
	};
	return check_length(this, root, 0, target);
}

uint32_t rbtree_t::find_index(uint32_t key) const
{
	uint32_t index = root;
	while(index != invalid && arr[index].key != key)
		index = key < arr[index].value ? arr[index].left : arr[index].right;
	return index;
}

uint32_t rbtree_t::get(uint32_t key) const
{
	uint32_t index = find_index(key);
	return index != invalid ? arr[index].value : index;
}

void rbtree_t::swap(uint32_t i, uint32_t j)
{
	if(i == invalid || j == invalid)
		return;
	node_t t = arr[i];
	arr[i] = arr[j];
	arr[j] = t;
}

void rbtree_t::rotate_left(uint32_t index)
{
	if(index == invalid)
		return;

	//  B   ->  A
	// D A     B C
	//    C   D

	uint32_t index_a = index;
	uint32_t index_b = arr[index].right;
	if(index_b == invalid)
		return;
	swap(index_b, index_a);
	arr[index_b].right = arr[index_a].left;
	if(arr[index_b].right != invalid)
		arr[arr[index_b].right].parent = index_b;
	arr[index_a].left = index_b;
}

void rbtree_t::rotate_right(uint32_t index)
{
	if(index == invalid)
		return;

	//  A   ->  B
	// B C     D A
	//D           C

	uint32_t index_b = index;
	uint32_t index_a = arr[index].left;
	if(index_a == invalid)
		return;
	swap(index_b, index_a);
	arr[index_a].left = arr[index_b].right;
	if(arr[index_a].left != invalid)
		arr[arr[index_a].left].parent = index_a;
	arr[index_b].right = index_a;
}

void rbtree_t::set(uint32_t key, uint32_t value)
{
	if(root == invalid)
	{
		root = allocate();
		arr[root].key = key;
		arr[root].value = value;
		return;
	}

	//uint32_t index = root, last_index = invalid;
	//while(index != invalid && arr[index].key != key)
	//	index = key < arr[index].value ? arr[index].left : arr[index].right;

}
