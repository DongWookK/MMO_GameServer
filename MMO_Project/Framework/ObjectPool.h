#pragma once
template <typename T>
class ObjectPool
{
public:
	//chunkSize 만큼의 객체 생성
	// 객체 풀의 객체가 모두 비게 되면 chunksize만큼의 객체를 추가로 생성하여 풀에 투입한다.
	// 풀은 커지기만 하고 줄어들지는 않는다. 객체들도 풀에서 삭제(memory해제) 되지 않는다.
	// 풀이 삭제되는 경우는 예외다.

	ObjectPool(size_t chunkSize = KDefaultChunkSize);

	//값에 의한 대입을 방지한다.
	ObjectPool(const ObjectPool<T>& src) = delete;
	ObjectPool<T>& operator=(const ObjectPool<T>& rhs) = delete;

	//acquireObject()가 리턴할 스마트 포인터이다.
	using Object = std::shared_ptr<T>;
	

	//객체를 클라이언트에 제공한다..
	Object acquireObject();

private:
		//mFreeList는 현재 가용한(클라이언트가 점유하지 않은) 객체들을 보관한다.
	std::queue<std::unique_ptr<T>> mFreeList;
	size_t mChunkSize;
	static const size_t KDefaultChunkSize = 10;
	//mChunkSize만큼의 새로운 객체를 생성하여 mFreeList에 추가한다. 
	void allocateChunk();

};

template<typename T>
const size_t ObjectPool<T>::KDefaultChunkSize;

