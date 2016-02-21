#include "sorts.h"
#include <malloc.h>

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
	static void (*sort)(dataset_t&, size_t, size_t) =
	[](dataset_t & data, size_t left, size_t right)
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

void sorts_mergesort(dataset_t & data)
{
	static void (*merge)(dataset_t&, size_t, size_t, size_t) =
	[](dataset_t & data, size_t left, size_t right, size_t middle)
	{
		int32_t * temp = (int32_t*)alloca((right - left + 1) * sizeof(int32_t));
		for(size_t i = left, j = middle + 1, k = 0; k + left <= right; ++k)
			if(i > middle || j <= right && data.items[j] < data.items[i])
				temp[k] = data.items[j++];
			else
				temp[k] = data.items[i++];
		for(size_t k = 0; k + left <= right; ++k)
			data.items[k + left] = temp[k];
	};

	static void (*sort)(dataset_t&, size_t, size_t) =
	[](dataset_t & data, size_t left, size_t right)
	{
		if(left >= right)
			return;
		size_t middle = (left + right) / 2;
		sort(data, left, middle);
		sort(data, middle + 1, right);
		merge(data, left, right, middle);
	};

	if(data.count)
		sort(data, 0, data.count - 1);
}

void sorts_bitonicsort(dataset_t & data)
{
	static void (*merge)(dataset_t&, size_t, size_t, bool) =
	[](dataset_t & data, size_t left, size_t right, bool ascending)
	{
		if(left >= right)
			return;

		size_t middle = 1;
		while(middle + left < right + 1)
			middle <<= 1;
		middle >>= 1;

		for(size_t i = left, j = left + middle; i + middle < right + 1; ++i, ++j)
			if(ascending == (data.items[i] > data.items[j]))
				data.swap(i, j);

		merge(data, left, middle + left - 1, ascending);
		merge(data, middle + left, right, ascending);
	};

	static void (*sort)(dataset_t&, size_t, size_t, bool) =
	[](dataset_t & data, size_t left, size_t right, bool ascending)
	{
		if(left >= right)
			return;

		size_t middle = (left + right) / 2;
		sort(data, left, middle, !ascending);
		sort(data, middle + 1, right, ascending);
		merge(data, left, right, ascending);
	};

	if(data.count)
		sort(data, 0, data.count - 1, true);
}
