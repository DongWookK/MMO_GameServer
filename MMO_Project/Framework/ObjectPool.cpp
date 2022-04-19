#include "pch.h"
#include "ObjectPool.h"

template <typename T>
ObjectPool<T>::ObjectPool(size_t chunkSize)
{
	if (chunkSize == 0) {
		throw std::invalid_argument("Chunk size must be positive")
	}
	mChunkSize = chunkSize;
	//�ʱ� Ǯ�μ� mChunkSize��ŭ�� ��ü�� �����Ѵ�.
	allocateChunk();
}

// mChunkSize������ ���ο� ��ü�� �Ҵ��Ѵ�.
template <typename T>
void ObjectPool<T>::allocateChunk()
{
	for (size_t i = 0; i < mChunkSize; ++i)
	{
		mFreeList.emplace(std::make_unique<T>());
	}
}

template <typename T>
typename ObjectPool<T>::Object ObjectPool<T>::acquireObject()
{
	if (mFreeList.empty()) {
		allocateChunk();
	}

	//ť�� ���� ���� �ִ� ���� ��ü�� ���� unique_ptr�� �ű��.
	std::unique_ptr<T> obj(std::move(mFreeList.front()));
	mFreeList.pop();

	//��ü �����͸� Object Ÿ�� ���� ��ȯ�Ѵ�.
	Object smartObject(obj.release(), [this](T* t) {
		mFreeList.push(std::unique_ptr<T>(t));
		});

	return smartObject;
}