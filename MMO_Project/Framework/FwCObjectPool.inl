namespace FnlApi
{
	template<typename T>
	template<typename TDerived, typename TInitializeFunction>
	DWORD CObjectPool<T>::AllocateChunk(TInitializeFunction&& pInitfunc, size_t pInitSize, bool pIsExpandable)
	{
		return AllocateChunk<TDerived>(std::forward<TInitializeFunction>(pInitfunc), nullptr, pInitSize, pIsExpandable);
	}

	template <typename T>
	template<typename TDerived, typename TInitializeFunction>
	inline DWORD CObjectPool<T>::AllocateChunk(TInitializeFunction&& pInitfunc, TUnAcquire&& pUnAcqFunc, size_t pInitSize, bool pIsExpandable)
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

		__mUnAcquire = std::forward<TUnAcquire>(pUnAcqFunc);

		__mAllocateChunk = [this, aInitFunc = std::forward<TInitializeFunction>(pInitfunc)]() {
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
					aKey += __mChunkSize;
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
				//TODO - ErrorMsg ���
				return nullptr;
			}
		}

		//ť�� ���� ���� �ִ� ���� ��ü�� ���� unique_ptr�� �ű��.
		const size_t aKey = GetUsableKey();

		TUObject aUniqueObject(std::move(__mFreeList[aKey].front()));
		__mFreeList[aKey].pop_front();

		//��ü �����͸� Object Ÿ�� ���� ��ȯ�Ѵ�.
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
		if(__mFreeList.end() == aIt)
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