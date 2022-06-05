#include "pch.h"
#include "Allocator.h"
#include "Memory.h"


void* Allocator::Alloc(int32 size)
{
	return ::malloc(size);
}

void Allocator::Release(void* ptr)
{
	::free(ptr);
}

/*-------------------
	PoolAllocator
-------------------*/

void* PoolAllocator::Alloc(int32 size)
{
	return GMemory->Allocate(size);
}

void PoolAllocator::Release(void* ptr)
{
	GMemory->Release(ptr);
}