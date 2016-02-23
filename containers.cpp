#include "containers.h"
#include <stdio.h>
#include <assert.h>

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
		index = key < arr[index].key ? arr[index].left : arr[index].right;
	return index;
}

uint32_t rbtree_t::get(uint32_t key) const
{
	uint32_t index = find_index(key);
	return index != invalid ? arr[index].value : index;
}

void rbtree_t::swap(uint32_t old_node, uint32_t new_node)
{
	if(old_node == invalid || new_node == invalid)
		return;
	else if(root == old_node)
		root = new_node;
	else
	{
		assert(arr[old_node].parent != invalid);
		if(arr[arr[old_node].parent].left == old_node)
			arr[arr[old_node].parent].left = new_node;
		else
			arr[arr[old_node].parent].right = new_node;
	}
	arr[new_node].parent = arr[old_node].parent;
	arr[old_node].parent = new_node;
}

void rbtree_t::rotate_left(uint32_t index)
{
	if(index == invalid)
		return;

	//  B   ->  A
	// D A     B C
	//    C   D

	uint32_t index_b = index;
	uint32_t index_a = arr[index].right;
	if(index_a == invalid)
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

	uint32_t index_a = index;
	uint32_t index_b = arr[index].left;
	if(index_b == invalid)
		return;
	swap(index_a, index_b);
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
		balance(root);
		return;
	}

	uint32_t index = root, last_index = invalid;
	bool left = false;

	while(index != invalid && arr[index].key != key)
	{
		last_index = index;
		left = key < arr[index].key;
		index = left ? arr[index].left : arr[index].right;
	}

	if(index != invalid)
	{
		arr[index].value = value;
		return;
	}

	uint32_t new_node = allocate();
	arr[new_node].key = key;
	arr[new_node].value = value;
	arr[new_node].parent = last_index;
	if(left)
		arr[last_index].left = new_node;
	else
		arr[last_index].right = new_node;

	balance(new_node);
	assert(validate());
}

void rbtree_t::balance(uint32_t index)
{
	if(index == invalid)
		return;

	if(arr[index].parent == invalid)
	{
		arr[index].color = false;
		return;
	}
	else
		arr[index].color = true;

	if(color(arr[index].parent) == false)
		return;

	assert(grandparent(index) != invalid);

	if(color(uncle(index)) == true)
	{
		arr[arr[index].parent].color = false;
		uint32_t uncle_index = uncle(index);
		if(uncle_index != invalid)
			arr[uncle_index].color = false;
		uint32_t grandparent_index = grandparent(index);
		arr[grandparent_index].color = true;
		balance(grandparent_index);
		return;
	}

	if(arr[arr[index].parent].right == index && arr[index].parent == arr[grandparent(index)].left)
	{
		rotate_left(arr[index].parent);
		index = arr[index].left;
	}
	else if(arr[arr[index].parent].left == index && arr[index].parent == arr[grandparent(index)].right)
	{
		rotate_right(arr[index].parent);
		index = arr[index].right;
	}

	arr[arr[index].parent].color = false;
	arr[grandparent(index)].color = true;

	if(arr[arr[index].parent].left == index && arr[index].parent == arr[grandparent(index)].left)
	{
		rotate_right(grandparent(index));
	}
	else
	{
		assert(arr[arr[index].parent].right == index && arr[index].parent == arr[grandparent(index)].right);
		rotate_left(grandparent(index));
	}
}

void rbtree_t::print(uint32_t height) const
{
	if(!height)
	{
		for(uint32_t i = 0; i < count; ++i)
			printf("%02u %s %02i:%02i %02i (%i:%i)\n", i,
				   root == i ? "X" : arr[i].color ? "r": "b",
				   arr[i].left, arr[i].right,
				   arr[i].parent,
				   arr[i].key, arr[i].value);
	}

	static void (*pprint)(const rbtree_t*, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, bool) =
	[](const rbtree_t * tree, uint32_t index, uint32_t deep, uint32_t target, uint32_t total, uint32_t parent, bool space)
	{
		if(deep == target)
		{
			if(index != invalid)
			{
				if(tree->arr[index].parent != parent)
					printf("*");
				else
					printf("%u", tree->arr[index].key);
					//printf("%u", tree->arr[index].color ? 1 : 0);
			}
			else
				printf("x");
			if(space)
				printf("%*s", (1 << (total - target + 1)) - 1, "");
		}
		else
		{
			pprint(tree, index != invalid ? tree->arr[index].left : invalid, deep + 1, target, total, index, true);
			pprint(tree, index != invalid ? tree->arr[index].right : invalid, deep + 1, target, total, index, space);
		}
	};

	if(height)
		for(uint32_t i = 0; i <= height; ++i)
		{
			if(i < height)
				printf("%*s", (1 << (height - i)) - 1, "");
			pprint(this, root, 0, i, height, invalid, false);
			printf("\n");
		}

	printf("valid tree : %s\n", validate() ? "yes" : "no");
}
