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

	//CPool 积己 规过 利侩鞘夸
	uint32 _mThreadNo = 5;
	std::vector<HANDLE> _mThreads;

	
};

