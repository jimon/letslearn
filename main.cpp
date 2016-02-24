
#include <stdio.h>
#include <assert.h>
#include "sorts.h"
#include "containers.h"

#include <stdlib.h>

bool sorts_test(void (*sort)(dataset_t&), size_t count = 1000)
{
	for(size_t i = 0; i < count; ++i)
	{
		dataset_t a = dataset_t::random();
		sort(a);
		if(!a.validate())
			return false;
	}
	return true;
}

bool linkedlist_test()
{
	linkedlist_t list;
	list.insert(0);
	for(uint32_t i = 0; i < list.count - 1; ++i)
		list.insert_before(i, 0);
	while(list.last() != list.invalid)
		list.remove(list.last());
	return true;
}

bool hashtable_test(float loadfactor = 1.0)
{
	hashtable_t table;
	uint32_t keys[hashtable_t::count] = {0};
	uint32_t values[hashtable_t::count] = {0};
	uint32_t payload = (uint32_t)((float)hashtable_t::count * loadfactor);

	for(uint32_t i = 0; i < payload; ++i)
		table.set(keys[i] = rand(), values[i] = rand());

	for(uint32_t i = 0; i < payload; ++i)
		if(values[i] != table.get(keys[i]))
			return false;

	return true;
}

bool rbtree_test()
{
	rbtree_t t;
	for(uint32_t i = 0; i < t.count; ++i)
		t.set(i, rand());
	while(t.root != t.invalid)
		t.remove(rand() % t.count);
	return true;
}

int main()
{
	#if 1
	assert(sorts_test(&sorts_bubble));
	assert(sorts_test(&sorts_quicksort));
	assert(sorts_test(&sorts_mergesort));
	assert(sorts_test(&sorts_radixsort));
	assert(sorts_test(&sorts_bitonicsort));

	assert(linkedlist_test());
	assert(hashtable_test());
	assert(rbtree_test());
	#endif

	//for(size_t i = 0; i < 1000; ++i)
	{
		//dataset_t a = {8,7,6,5,4,3,2,1};
		//dataset_t a = dataset_t::random(33);
		//a.print();
		//sorts_radixsort(a);
		//a.print();
		//printf("valid : %i\n", a.validate());
		//assert(a.validate());
	}

	binaryheap_t t;
	for(uint32_t i = 0; i <= 9; ++i)
		t.insert(i);
	t.print();
	t.remove();
	t.print();

	//t.rotate_left(t.root);
	//t.print(4);

	printf("so wow\n");
	return 0;
}
