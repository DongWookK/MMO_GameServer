#pragma once


class CThreadBase
{

public:
	CThreadBase();
	void ResetAttr();
	virtual HANDLE Open();
	virtual uint32 Close();
	static unsigned __stdcall Start(void *pThis);
	virtual void Main();

private:

	HANDLE _mThdHandler;
	CThreadMgr* _mThreadMgr;
};

unsigned Start(void *pthis);