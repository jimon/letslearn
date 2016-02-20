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

void sorts_quicksort(dataset_t & data)
{
	// TODO try more optimized versions
	static void (*sort)(dataset_t&, size_t, size_t) = [](dataset_t & data, size_t left, size_t right)
	{
		if(left >= right)
			return;
		size_t pivot = left; // TODO figure out why only this pivot point works
		int32_t pivot_val = data.items[pivot];
		size_t i = left + 1;
		size_t j = right;

		while(i < j)
		{
			if(data.items[i] <= pivot_val)
				i++;
			else if(pivot_val <= data.items[j])
				j--;
			else
				data.swap(i, j);
		}

		if(data.items[i] < pivot_val) // TODO what exactly does this do ?
			data.swap(i--, pivot);
		else
			data.swap(--i, pivot);

		sort(data, left, i);
		sort(data, j, right);
	};

	if(data.count)
		sort(data, 0, data.count - 1);
}
