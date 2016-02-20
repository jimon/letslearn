
#include <stdio.h>
#include "sorts.h"

int main()
{
	for(size_t i = 0; i < 10000; ++i)
	{
		dataset_t a = dataset_t::random();
		sorts_bubble(a);
		a.validate();
	}


	printf("so wow\n");
	return 0;
}
