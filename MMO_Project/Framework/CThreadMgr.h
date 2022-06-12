#pragma once
#include <vector>
/*---------------------------------------------------------------------
	Thread Manager
desc : Managing Priamry, Worker, Db, Log Thread
warn :
----------------------------------------------------------------------*/

//�̱��� ���� ���� �ʿ�
enum class Flag {
	START,
	INITIAL,
	STOPPED,
	END
};


class CThreadMgr : public Singleton<CThreadMgr>
{
public:
	
	Atomic<Flag> _mFlag = Flag::START;

	int32				Open();
	int32				Close();


	int32				OpenWk();

private:

	uint32 _mThreadNo = 5;
	std::vector<HANDLE> _mThreads;

	
};

