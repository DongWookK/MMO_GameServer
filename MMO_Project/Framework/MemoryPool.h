#pragma once



/* 
	Aligned_alloc을 위한 enum 선언
	alignment 값에 의해 특정된 alignment 를 가진, 초기화 되지 않은 공간에 대해 size 크기 만큼 메모리를 할당하는 함수이다.
	size 값은 alignment의 배수여야 한다.aligned_alloc은 thread - safe 하다.함수 실행에 성공하면, 새롭게 할당된 메모리의 첫 주소를 반환한다
*/
enum
{
	SLIST_ALIGNMENT = 16
};

/*-----------------
	MemoryHeader
------------------*/

struct MemoryHeader
{
	// [MemoryHeader][Data]
	MemoryHeader(int32 size) : allocSize(size) { }

	static void* AttachHeader(MemoryHeader* header, int32 size)
	{
		new(header)MemoryHeader(size); // placement new
		return reinterpret_cast<void*>(++header);
	}

	static MemoryHeader* DetachHeader(void* ptr)
	{
		MemoryHeader* header = reinterpret_cast<MemoryHeader*>(ptr) - 1;
		return header;
	}

	int32 allocSize;
};

/*-----------------
	MemoryPool
------------------*/

class MemoryPool
{
public:
	MemoryPool(int32 allocSize);
	~MemoryPool();

	void			Push(MemoryHeader* ptr);
	MemoryHeader* Pop();

private:
	int32 _allocSize = 0;
	atomic<int32> _allocCount = 0;

	USE_LOCK;
	queue<MemoryHeader*> _queue;
};

