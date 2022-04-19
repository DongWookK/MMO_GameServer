#pragma once
template <typename T>
class ObjectPool
{
public:
	//chunkSize ��ŭ�� ��ü ����
	// ��ü Ǯ�� ��ü�� ��� ��� �Ǹ� chunksize��ŭ�� ��ü�� �߰��� �����Ͽ� Ǯ�� �����Ѵ�.
	// Ǯ�� Ŀ���⸸ �ϰ� �پ������ �ʴ´�. ��ü�鵵 Ǯ���� ����(memory����) ���� �ʴ´�.
	// Ǯ�� �����Ǵ� ���� ���ܴ�.

	ObjectPool(size_t chunkSize = KDefaultChunkSize);

	//���� ���� ������ �����Ѵ�.
	ObjectPool(const ObjectPool<T>& src) = delete;
	ObjectPool<T>& operator=(const ObjectPool<T>& rhs) = delete;

	//acquireObject()�� ������ ����Ʈ �������̴�.
	using Object = std::shared_ptr<T>;
	

	//��ü�� Ŭ���̾�Ʈ�� �����Ѵ�..
	Object acquireObject();

private:
		//mFreeList�� ���� ������(Ŭ���̾�Ʈ�� �������� ����) ��ü���� �����Ѵ�.
	std::queue<std::unique_ptr<T>> mFreeList;
	size_t mChunkSize;
	static const size_t KDefaultChunkSize = 10;
	//mChunkSize��ŭ�� ���ο� ��ü�� �����Ͽ� mFreeList�� �߰��Ѵ�. 
	void allocateChunk();

};

template<typename T>
const size_t ObjectPool<T>::KDefaultChunkSize;

