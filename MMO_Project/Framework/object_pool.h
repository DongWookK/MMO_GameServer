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