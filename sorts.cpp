#include "sorts.h"
#include "containers.h"
#include <malloc.h>
#include <assert.h>

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
		auto pivot_val = data.items[pivot];
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

void sorts_heapsort(dataset_t & data)
{
	binaryheap_t heap;
	heap.build(data.items, (uint32_t)data.count);
	for(size_t i = data.count; i > 0; --i)
		data.items[i - 1] = heap.remove();
}

void sorts_treesort(dataset_t & data)
{
	rbtree_t tree;
	for(size_t i = 0; i < data.count; ++i)
		tree.set(data.items[i], 0, true);

	static void (*inorder)(dataset_t&, const rbtree_t&, uint32_t, uint32_t*) =
	[](dataset_t & data, const rbtree_t & tree, uint32_t node_index, uint32_t * data_index)
	{
		if(node_index == tree.invalid)
			return;
		assert(data_index);
		inorder(data, tree, tree.left(node_index), data_index);
		data.items[(*data_index)++] = tree.arr[node_index].key;
		inorder(data, tree, tree.right(node_index), data_index);
	};
	uint32_t data_index = 0;
	inorder(data, tree, tree.root, &data_index);
}

void sorts_mergesort(dataset_t & data)
{
	static void (*merge)(dataset_t&, size_t, size_t, size_t) =
	[](dataset_t & data, size_t left, size_t right, size_t middle)
	{
		auto temp = (decltype(&dataset_t::items[0]))alloca(
						(right - left + 1) * sizeof(dataset_t::items[0]));
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

void sorts_radixsort(dataset_t & data)
{
	static void (*sort)(dataset_t&, size_t, size_t, uint32_t) =
	[](dataset_t & data, size_t left, size_t right, uint32_t bit)
	{
		if(left >= right || !bit)
			return;

		size_t i = left, j = right;
		while(i < j)
		{
			if(!(data.items[i] & bit))
				++i;
			else if(data.items[j] & bit)
				--j;
			else
				data.swap(i, j);
		}

		if(i > left && (data.items[i] & bit))
			--i;
		if(j < right && !(data.items[j] & bit))
			++j;

		sort(data, left, i, bit >> 1);
		sort(data, j, right, bit >> 1);
	};

	if(data.count)
	{
		auto max = data.items[0];
		for(size_t i = 1; i < data.count; ++i)
			if(max < data.items[i])
				max = data.items[i];
		uint32_t bit = 1;
		while(max >>= 1)
			bit <<= 1;
		sort(data, 0, data.count - 1, bit);
	}
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
