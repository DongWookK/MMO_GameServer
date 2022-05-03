#pragma once
/*---------------------------------------------------------------------
	Thread Manager
desc : Managing Priamry, Worker, Db, Log Thread
warn :
----------------------------------------------------------------------*/

class CThreadMgr
{
public:
	int32				open();
	int32				openWk();

private:

	//CPool ���� ��� �����ʿ�
	uint32 _mThreadNo = 5;
	std::vector<HANDLE> _mThreads;

	
};

