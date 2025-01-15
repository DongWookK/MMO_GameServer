#pragma once
#include "pch.h"
#include "object_pool.h"

namespace fw
{
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
		const size_t aKey = GetUsableKey();

		TUObject aUniqueObject(std::move(__mFreeList[aKey].front()));
		__mFreeList[aKey].pop_front();

		//객체 포인터를 Object 타입 으로 변환한다.
		Object aSmartObject(aUniqueObject.release(), [this, aKey](T* t) {

			if (__mUnAcquire)
			{
				__mUnAcquire(t);
			}

			std::lock_guard aLock(__mLocker);

			auto aIt = __mFreeList.find(aKey);
			if (__mFreeList.end() != aIt)
			{
				__mFreeList[aKey].emplace_back(TUObject{ t });
				Organize(aKey);
			}

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

	template<typename T>
	inline void CObjectPool<T>::Organize(const size_t pKey)
	{
		if (false == __mIsExpandable)
		{
			return;
		}
		if (DEFAULT_PARTITION_KEY == pKey)
		{
			return;
		}

		auto aIt = __mFreeList.find(pKey);
		if (__mFreeList.end() == aIt)
		{
			return;
		}

		if (aIt->second.size() == __mChunkSize)
		{
			while (aIt->second.size())
			{
				aIt->second.pop_front();
			}
			aIt->second.clear();

			__mUsedCount -= __mChunkSize;
			__mFreeList.erase(pKey);
		}
	}
}