#pragma once
template <typename T>
class CObjectPool
{

private:
	using TUObject	= std::unique_ptr;
	using TLocker	= std::shared_mutex;

public:
	using TObject = T;
	
	using TUnacquireFunction = std::function<void(T* target)>;
	using TPartition = std::deque<TUObject>;
	using TFreeList  = std::map<size_t, TPartition>;

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
	DWORD	AllocateChunk(TInitializeFunction&& pInitifunc, size_t pInitSize = DEFAULT_CHUNK_SIZE, bool pIsExpandable = false);

	template<typename TDerived, typename TInitializeFunction>
	DWORD	AllocateChunk(TInitializeFunction&& pInitifunc, TUnacquireFunction&& pUnacqFunc, size_t pInitSIze = DEFAULT_CHUNK_SIZE, bool pIsExpandable = false);


	//acquireObject()가 리턴할 스마트 포인터이다.
	using Object = std::shared_ptr<T>;

	//객체를 클라이언트에 제공한다..
	Object AcquireObject(void);

	constexpr size_t FreeCount(void) const;
	constexpr size_t UseCount(void) const;
	constexpr size_t AllocateCount(void) const;
	
	constexpr bool	IsExpandable(void) const;

private:
	//mFreeList는 현재 가용한(클라이언트가 점유하지 않은) 객체들을 보관한다.
	mutable TLocker__mLocker;
	TFreeList __mFreeList;

	size_t __mInitSize;
	size_t __mChunkSize;
	bool __mIsExpandable;

	TUnacquireFunction __mUnacquire;
	std::function<void(T* target)> __mAllocateChunk;



	

};

template<typename T>
const size_t CObjectPool<T>::DEFAULT_CHUNK_SIZE;

