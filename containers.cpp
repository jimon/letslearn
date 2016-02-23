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

uint32_t rbtree_t::get(uint32_t key) const
{
	uint32_t index = find_index(key);
	return index != invalid ? arr[index].value : index;
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
	bool left_key = false;

	while(index != invalid && arr[index].key != key)
	{
		last_index = index;
		left_key = key < arr[index].key;
		index = left_key ? left(index) : right(index);
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
	if(left_key)
		arr[last_index].left = new_node;
	else
		arr[last_index].right = new_node;

	balance(new_node);
	assert(validate());
}

void rbtree_t::remove(uint32_t key)
{
	uint32_t index = find_index(key);
	if(index == invalid)
		return;

	if(left(index) != invalid && right(index) != invalid)
	{
		uint32_t most_right = left(index);
		while(right(most_right) != invalid)
			most_right = right(most_right);
		arr[index].key = arr[most_right].key;
		arr[index].value = arr[most_right].value;
		index = most_right;
	}

	assert(left(index) == invalid || right(index) == invalid);
	uint32_t child = left(index) == invalid ? right(index) : left(index);
	if(color(index) == false)
	{
		arr[index].color = color(child);
		rebalance(index);
	}

	swap(index, child);
	if(parent(index) == invalid)
		set_color(child, false);

	if(right(parent(index)) == index)
		arr[parent(index)].right = invalid;
	else
		arr[parent(index)].left = invalid;
	arr[index].free();
	assert(validate());
}

uint32_t rbtree_t::allocate()
{
	// TODO optimize ?
	for(uint32_t i = 0; i < count; ++i)
		if(!arr[i].taken())
		{
			arr[i].free();
			return i;
		}
	return invalid;
}

uint32_t rbtree_t::parent(uint32_t index) const
{
	return index != invalid ? arr[index].parent : invalid;
}

uint32_t rbtree_t::left(uint32_t index) const
{
	return index != invalid ? arr[index].left : invalid;
}

uint32_t rbtree_t::right(uint32_t index) const
{
	return index != invalid ? arr[index].right : invalid;
}

uint32_t rbtree_t::grandparent(uint32_t index) const
{
	return parent(parent(index));
}

uint32_t rbtree_t::sibling(uint32_t index) const
{
	return left(parent(index)) == index ? right(parent(index)) : left(parent(index));
}

uint32_t rbtree_t::uncle(uint32_t index) const
{
	return sibling(parent(index));
}

bool rbtree_t::color(uint32_t index) const
{
	return index != invalid ? arr[index].color : false;
}

void rbtree_t::set_color(uint32_t index, bool color)
{
	if(index != invalid)
		arr[index].color = color;
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
		   tree->color(tree->left(index)) ||
		   tree->color(tree->right(index)) ||
		   tree->color(tree->parent(index))))
			return false;
		return check_color(tree, tree->left(index)) &&
			   check_color(tree, tree->right(index));
	};
	if(!check_color(this, root))
		return false;

	// verify if tree is balanced
	uint32_t target = 1;
	uint32_t left_index = root;
	while(left_index != invalid)
		if(!color(left_index = left(left_index))) // leaf is when left == invalid
			++target;
	static bool (*check_length)(const rbtree_t*, uint32_t, uint32_t, uint32_t) =
	[](const rbtree_t * tree, uint32_t index, uint32_t current, uint32_t target) -> bool
	{
		if(index == invalid)
			return current + 1 == target;
		if(!tree->color(index))
			++current;
		return check_length(tree, tree->left(index), current, target) &&
			   check_length(tree, tree->right(index), current, target);
	};
	return check_length(this, root, 0, target);
}

uint32_t rbtree_t::find_index(uint32_t key) const
{
	uint32_t index = root;
	while(index != invalid && arr[index].key != key)
		index = key < arr[index].key ? left(index) : right(index);
	return index;
}

void rbtree_t::swap(uint32_t old_node, uint32_t new_node)
{
	if(old_node == invalid || new_node == invalid)
		return;
	else if(root == old_node)
		root = new_node;
	else
	{
		assert(parent(old_node) != invalid);
		if(left(parent(old_node)) == old_node)
			arr[parent(old_node)].left = new_node;
		else
			arr[parent(old_node)].right = new_node;
	}
	arr[new_node].parent = parent(old_node);
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
	uint32_t index_a = right(index);
	if(index_a == invalid)
		return;
	swap(index_b, index_a);
	arr[index_b].right = left(index_a);
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
	uint32_t index_b = left(index);
	if(index_b == invalid)
		return;
	swap(index_a, index_b);
	arr[index_a].left = right(index_b);
	if(arr[index_a].left != invalid)
		arr[arr[index_a].left].parent = index_a;
	arr[index_b].right = index_a;
}

void rbtree_t::balance(uint32_t index)
{
	if(index == invalid)
		return;

	if(parent(index) == invalid)
	{
		set_color(index, false);
		return;
	}
	else
		set_color(index, true);

	if(color(parent(index)) == false)
		return;

	assert(grandparent(index) != invalid);

	if(color(uncle(index)) == true)
	{
		set_color(parent(index), false);
		set_color(uncle(index), false);
		set_color(grandparent(index), false);
		balance(grandparent(index));
		return;
	}

	if(right(parent(index)) == index && parent(index) == left(grandparent(index)))
	{
		rotate_left(parent(index));
		index = left(index);
	}
	else if(left(parent(index)) == index && parent(index) == right(grandparent(index)))
	{
		rotate_right(parent(index));
		index = right(index);
	}

	set_color(parent(index), false);
	set_color(grandparent(index), true);

	if(left(parent(index)) == index && parent(index) == left(grandparent(index)))
	{
		rotate_right(grandparent(index));
	}
	else
	{
		assert(right(parent(index)) == index && parent(index) == right(grandparent(index)));
		rotate_left(grandparent(index));
	}
}

void rbtree_t::rebalance(uint32_t index)
{
	if(parent(index) == invalid)
		return;
	if(color(sibling(index)) == true)
	{
		set_color(parent(index), true);
		set_color(sibling(index), false);
		if(arr[parent(index)].left == index)
			rotate_left(parent(index));
		else
			rotate_right(parent(index));
	}

	if(color(parent(index)) == false &&
	   color(sibling(index)) == false &&
	   color(left(sibling(index))) == false &&
	   color(right(sibling(index))) == false)
	{
		set_color(sibling(index), true);
		rebalance(parent(index));
		return;
	}

	if(color(parent(index)) == true &&
	   color(sibling(index)) == false &&
	   color(left(sibling(index))) == false &&
	   color(right(sibling(index))) == false)
	{
		set_color(sibling(index), true);
		set_color(parent(index), false);
		return;
	}

	if(left(parent(index)) == index &&
	   color(sibling(index)) == false &&
	   color(left(sibling(index))) == true &&
	   color(right(sibling(index))) == false)
	{
		set_color(sibling(index), true);
		set_color(left(sibling(index)), false);
		rotate_right(sibling(index));
	}
	else if(left(parent(index)) == index &&
			color(sibling(index)) == false &&
			color(left(sibling(index))) == true &&
			color(right(sibling(index))) == false)
	{
		set_color(sibling(index), true);
		set_color(right(sibling(index)), false);
		rotate_left(sibling(index));
	}

	set_color(sibling(index), color(parent(index)));
	set_color(parent(index), false);

	if(left(parent(index)) == index)
	{
		assert(color(right(sibling(index))) == true);
		set_color(right(sibling(index)), false);
		rotate_left(parent(index));
	}
	else
	{
		assert(color(left(sibling(index))) == true);
		set_color(left(sibling(index)), false);
		rotate_right(parent(index));
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
				if(tree->parent(index) != parent)
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
			pprint(tree, index != invalid ? tree->left(index) : invalid, deep + 1, target, total, index, true);
			pprint(tree, index != invalid ? tree->right(index) : invalid, deep + 1, target, total, index, space);
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
