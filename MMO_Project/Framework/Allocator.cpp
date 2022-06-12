#include "pch.h"
#include "Allocator.h"


void* Allocator::Alloc(int32 size)
{
	return ::malloc(size);
}

void Allocator::Release(void* ptr)
{
	::free(ptr);
}
