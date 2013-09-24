#ifndef INDEXALLOCATOR_H
#define INDEXALLOCATOR_H

#include <cstdint>
#include <vector>
#include <algorithm>

class IndexAllocator
{
public:
	typedef uint32_t Index;

	inline IndexAllocator();

	inline Index Allocate();
	inline void AllocateBlock(Index *indexArray, const size_t count);

	inline void Free(const Index index);
	inline void FreeBlock(const Index *indexArray, const size_t count);

private:
	std::vector<Index> _freeIndices;
	Index _nextFreeIndex;
};

IndexAllocator::IndexAllocator() :
	_nextFreeIndex(1)
{
}

IndexAllocator::Index IndexAllocator::Allocate()
{
	if(_freeIndices.size() > 0)
	{
		Index index = _freeIndices.back();
		_freeIndices.pop_back();
		return index;
	}
	else
	{
		return _nextFreeIndex++;
	}
}

void IndexAllocator::AllocateBlock(Index *indexArray, const size_t count)
{
	for(size_t i = 0; i < count; i++)
	{
		indexArray[i] = Allocate();
	}
}

void IndexAllocator::Free(const Index index)
{
	auto location = std::lower_bound(std::begin(_freeIndices), std::end(_freeIndices), index);
	_freeIndices.insert(location, index);
}

void IndexAllocator::FreeBlock(const Index *indexArray, const size_t count)
{
	for(size_t i = 0; i < count; i++)
	{
		_freeIndices.push_back(indexArray[i]);
	}

	std::sort(std::begin(_freeIndices), std::end(_freeIndices));
}

#endif // INDEXALLOCATOR_H