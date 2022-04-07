#pragma once

class ThreadBase
{

public:

	ThreadBase();
	virtual uint32 Open();
	virtual uint32 Close();
	static unsigned __stdcall Start(void *pThis);
	virtual void Main();
};

unsigned Start(void *pthis);