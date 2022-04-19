#include "pch.h"
#include "ObjectPool.h"

template <typename T>
ObjectPool<T>::ObjectPool(size_t chunkSize)
{
	if (chunkSize == 0) {
		throw std::invalid_argument("Chunk size must be positive")
	}
	mChunkSize = chunkSize;
	//초기 풀로서 mChunkSize만큼의 객체를 생성한다.
	allocateChunk();
}

// mChunkSize마늠의 새로운 객체를 할당한다.
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

	//큐의 제일 위에 있는 가용 객체를 로컬 unique_ptr로 옮긴다.
	std::unique_ptr<T> obj(std::move(mFreeList.front()));
	mFreeList.pop();

	//객체 포인터를 Object 타입 으로 변환한다.
	Object smartObject(obj.release(), [this](T* t) {
		mFreeList.push(std::unique_ptr<T>(t));
		});

	return smartObject;
}