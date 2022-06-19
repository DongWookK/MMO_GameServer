#include "pch.h"
#include "FwCObjectPool.h"
#include <ppl.h>
#undef max

template <typename T>
template<typename TDerived, typename TInitializeFunction>
inline DWORD CObjectPool<T>::AllocateChunk(TInitializeFunction&& pInitFunc, size_t pInitSize, bool pIsExpandable)
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

		Concurrency::cancellation_token_source cts;
		Concurrency::run_with_cancellation_token([&]() {
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

		__mAllocateSize = std::max(__mAllocateSize, (aKey + aSize));

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
	
	//큐의 제일 위에 있는 가용 객체를 로컬 unique_ptr로 옮긴다.
	std::unique_ptr<T> obj(std::move(__mFreeList.front()));
	__mFreeList.pop();

	//객체 포인터를 Object 타입 으로 변환한다.
	Object aSmartObject(obj.release(), [this](T* t) {
		__mFreeList.push(std::unique_ptr<T>(t));
		});
	

	return aSmartObject;
}

template <typename T>
inline const bool CObjectPool<T>::IsFull() const
{
	auto alt = std::find_if(__mFreeList.cbegin(), __mFreeList.cend(), [](auto& pPartition)
		{
			return false == pPartition.second.empty();
		});
		return(__mFreeList.cend() == alt) ? true : false;
}

template <typename T>
inline const size_t CObjectPool<T>::GetUsableKey() const
{
	if (false == __mIsExpandable)
	{
		return 0;
	}

	auto alt = std::find_if(__mFreeList.cbegin(), __mFreeList.cend(), [](auto& pPartition)
		{
			return false == pPartition.second.empty();
	});
	return(__mFreeList.cend() != alt) ? alt->first : -1;
}