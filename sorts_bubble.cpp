#include "sorts.h"

void sorts_bubble(dataset_t & data)
{
	bool swap = true;
	size_t count = data.count;
	while(swap)
	{
		swap = false;
		for(size_t i = 1; i < count; ++i)
		{
			if(data.items[i - 1] > data.items[i])
			{
				data.swap(i - 1, i);
				swap = true;
			}
		}
		count--;
	}
}
