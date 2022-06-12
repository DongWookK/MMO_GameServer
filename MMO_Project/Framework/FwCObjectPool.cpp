#include "pch.h"
#include "FwCObjectPool.h"

template <typename T>
template<typename TDerived, typename TInitializeFunction>
inline DWORD CObjectPool<T>::AllocateChunk(TInitializeFunction&& pInitfunc, size_t pInitSize, bool pIsExpandable)
{
	return AllocateChunk<TDerived>(std::forward<TInitializeFunction>(pInitfunc), nullptr, pInitSize, pIsExpandable);
}

template <typename T>
template<typename TDerived, typename TInitializeFunction>
inline DWORD CObjectPool<T>::AllocateChunk(TInitializeFunction&& pInitifunc, TUnacquireFunction&& pUnacFunc, size_t pInitSize, bool pIsExpandable)
{
	static_assert(std::is_base_of_v<T, TDerived>);
	ASSERT_CRASH(0 == __mAllocateSize);

	if (pInitSize <= 0)
	{
		throw std::invalid_argument("Init Size must be positive");
	}

	__mInitSize = pInitSize;
	__mChunkSize = DEFAULT_CHUNK_SIZE;
	__mIsExpandable = pIsExpandable;


	__mUnacquire = std::forward<TUnacquireFunction>(pUnacFunc);

	__mAllocateChunk = [this, aInitFunc = std::forward<TInitializeFunction>(pInitFunc)]() {
		const size_t aBegin = 0;
		const size_t aSize = (0 == __mFreeList.size()) ? __mInitSize : __mChunkSize;

		size_t aKey = 0;

		for (const auto& alt : __mFreeList)
		{
			if (DEFAULT_PARTITION_KEY == alt.first)
			{
				aKey += __mInitSize;
				continue;
			}

			if (aKey == alt.first)
			{
				continue;
			}
			break;
		}

		__mFreeList.emplace(aKey, TPartition{});

		for (size_t i = 0; i < aSize; ++i)
		{
			__mFreeList[aKey].emplace_back(std::make_unique<TDerived>());
			__mUsedCount++;
		}

		DWORD aErrorCode = 0;
		std::atomic<size_t> aIndex = aKey;

		concurrency::cancellation_token_source cts;
		concurrency::run_with_cancellation_token([&]() {
			concurrency::parallel_for_each(std::next(__mFreeList[aKey].begin(), aBegin), __mFreeList[aKey].end(), [&](auto& pObj) {
				DWORD aError = 0;
				const auto aCurrent = aIndex.fetch_add(1);
				aError = aInitFunc(pObj.get(), aCurrent);
				if (aError)
				{
					aErrorCode = aError;
					cts.cancel();
				}
				});
			}, cts.get_token());
		ASSERT_CRASH((aKey + aSize) == aIndex);
		__mAllocateSize == std::max(__mAllocateSize, (aKey + aSize));

		return aErrorCode;
	};
	std::lock_guard aLock(__mLocker);
	return __mAllocateChunk();
}

template <typename T>
typename CObjectPool<T>::Object CObjectPool<T>::AcquireObject()
{
	if (IsFull())
	{
		if (__mIsExpandable)
		{
			DWORD aErrorCode = __mAllocateChunk();
			if (aErrorCode)
			{
				return nullptr;
			}
		}
		else
		{
			//TODO - ErrorMsg 출력
			return nullptr;
		}
	}
	Object aSmartObject;
	/*
	//큐의 제일 위에 있는 가용 객체를 로컬 unique_ptr로 옮긴다.
	std::unique_ptr<T> obj(std::move(mFreeList.front()));
	mFreeList.pop();

	//객체 포인터를 Object 타입 으로 변환한다.
	Object smartObject(obj.release(), [this](T* t) {
		mFreeList.push(std::unique_ptr<T>(t));
		});
	*/

	return aSmartObject;
}