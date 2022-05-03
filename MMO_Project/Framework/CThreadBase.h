#pragma once

class CThreadBase
{

public:
	CThreadBase();
	virtual HANDLE Open();
	virtual uint32 Close();
	static unsigned __stdcall Start(void *pThis);
	virtual void Main();

private:

	HANDLE _mThdHandler;
};

unsigned Start(void *pthis);