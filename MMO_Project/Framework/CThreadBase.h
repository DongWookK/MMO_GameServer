#pragma once
#include "CThreadMgr.h"
class CThreadBase
{

public:

	CThreadBase();
	virtual uint32 Open();
	virtual uint32 Close();
	static unsigned __stdcall Start(void *pThis);
	virtual void Main();

private:
	uint32 AllocateThreadId();
};

unsigned Start(void *pthis);