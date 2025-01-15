#pragma once
#include "pch.h"
namespace fw
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
		//chunkSize 만큼의 객체 생성
		// 객체 풀의 객체가 모두 비게 되면 chunksize만큼의 객체를 추가로 생성하여 풀에 투입한다.
		// 풀은 커지기만 하고 줄어들지는 않는다. 객체들도 풀에서 삭제(memory해제) 되지 않는다.
		// 풀이 삭제되는 경우는 예외다.

		CObjectPool(void) = default;
		//CObjectPool(size_t chunkSize = KDefaultChunkSize);

		//값에 의한 대입을 방지한다.
		CObjectPool(const CObjectPool<T>& src) = delete;
		CObjectPool<T>& operator=(const CObjectPool<T>& rhs) = delete;

		//mChunkSize만큼의 새로운 객체를 생성하여 mFreeList에 추가한다. 
		//initializefunction을 통해 open까지 수행

		template<typename TDerived, typename TInitializeFunction>
		DWORD AllocateChunk(TInitializeFunction&& pInitfunc, size_t pInitSize, bool pIsExpandable)
		{
			return AllocateChunk<TDerived>(std::forward<TInitializeFunction>(pInitfunc), nullptr, pInitSize, pIsExpandable);
		};

		template<typename TDerived, typename TInitializeFunction>
		DWORD AllocateChunk(TInitializeFunction&& pInitfunc, TUnAcquire&& pUnAcqFunc, size_t pInitSize, bool pIsExpandable)
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

		//acquireObject()가 리턴할 스마트 포인터이다.
		using Object = std::shared_ptr<T>;

		//객체를 클라이언트에 제공한다..
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
		//mFreeList는 현재 가용한(클라이언트가 점유하지 않은) 객체들을 보관한다.
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