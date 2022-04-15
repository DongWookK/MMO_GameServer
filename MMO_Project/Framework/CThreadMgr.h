#pragma once
#include "CThreadBase.h"
#include "CThreadWk.h"
#include "FwTLS.h"
#include "pch.h"
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
	int32				InitTLS();
private:

	//CPool ���� ��� �����ʿ�
	uint32 WkThreadNo = 5;
	std::vector<CThreadBase*> ThreadPool;
	
};

