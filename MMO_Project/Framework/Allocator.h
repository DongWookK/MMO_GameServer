#pragma once
class Allocator
{
public:
	static void*	Alloc(int32 size);
	static void		Release(void* ptr);
};

