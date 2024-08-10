#pragma once

#include "pch_fw.h"
namespace FnlApi
{

template <typename T>
class CObjectPool
{

private:
	using TUObject = std::unique_ptr<T>;
	using TLocker = std::shared_mutex;

public:
	using TObject = T;
	using TUnAcquire = std::function<void(T* Target)>;
	using TPartition = std::deque<TUObject>;
	using TFreeList = std::map<size_t, TPartition>;

private:
	static constexpr size_t DEFAULT_PARTITION_KEY = 0;
	static constexpr size_t DEFAULT_CHUNK_SIZE = 30;

public:
	//chunkSize ��ŭ�� ��ü ����
	// ��ü Ǯ�� ��ü�� ��� ��� �Ǹ� chunksize��ŭ�� ��ü�� �߰��� �����Ͽ� Ǯ�� �����Ѵ�.
	// Ǯ�� Ŀ���⸸ �ϰ� �پ������ �ʴ´�. ��ü�鵵 Ǯ���� ����(memory����) ���� �ʴ´�.
	// Ǯ�� �����Ǵ� ���� ���ܴ�.

	CObjectPool(void) = default;
	//CObjectPool(size_t chunkSize = KDefaultChunkSize);

	//���� ���� ������ �����Ѵ�.
	CObjectPool(const CObjectPool<T>& src) = delete;
	CObjectPool<T>& operator=(const CObjectPool<T>& rhs) = delete;

	//mChunkSize��ŭ�� ���ο� ��ü�� �����Ͽ� mFreeList�� �߰��Ѵ�. 
	//initializefunction�� ���� open���� ����

	template<typename TDerived, typename TInitializeFunction>
	DWORD	AllocateChunk(TInitializeFunction&& pInitfunc, size_t pInitSize = DEFAULT_CHUNK_SIZE, bool pIsExpandable = false);

	template<typename TDerived, typename TInitializeFunction>
	DWORD	AllocateChunk(TInitializeFunction&& pInitfunc, TUnAcquire&& pUnAcqFunc, size_t pInitSize = DEFAULT_CHUNK_SIZE, bool pIsExpandable = false);


	//acquireObject()�� ������ ����Ʈ �������̴�.
	using Object = std::shared_ptr<T>;

	//��ü�� Ŭ���̾�Ʈ�� �����Ѵ�..
	Object AcquireObject(void);

	constexpr size_t FreeCount(void) const;
	constexpr size_t UseCount(void) const;
	constexpr size_t AllocateCount(void) const;
	constexpr bool	IsExpandable(void) const;

private:
	const bool IsFull() const;
	const size_t GetUsableKey() const;
	void	Organize(const size_t pKey);

private:
	//mFreeList�� ���� ������(Ŭ���̾�Ʈ�� �������� ����) ��ü���� �����Ѵ�.
	mutable TLocker __mLocker;
	TFreeList __mFreeList;

	size_t __mInitSize;
	size_t __mChunkSize;
	size_t __mAllocateSize;
	size_t __mUsedCount;
	bool __mIsExpandable;

	std::function<DWORD(void)> __mAllocateChunk;
	TUnAcquire __mUnAcquire;

};
}
#include "FwCObjectPool.inl"
