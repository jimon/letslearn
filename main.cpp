
#include <stdio.h>
#include "sorts.h"

bool sorts_test(void (*sort)(dataset_t&), size_t count = 10000)
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

int main()
{
	assert(sorts_test(&sorts_bubble));
	assert(sorts_test(&sorts_quicksort));

	for(size_t i = 0; i < 1000; ++i)
	{
		dataset_t a = {5,4,3,2,1};
		//dataset_t a = dataset_t::random();
		//a.print();
		sorts_quicksort(a);
		//a.print();
		//printf("valid : %i\n", a.validate());
		assert(a.validate());
	}


	printf("so wow\n");
	return 0;
}
