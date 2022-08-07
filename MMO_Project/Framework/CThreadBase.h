#pragma once


class CThreadBase
{

public:
	CThreadBase();
	void ResetAttr();
	virtual DWORD Open();
	virtual DWORD Close();
	static unsigned __stdcall Start(void *pThis);
	virtual void Main();

private:

	DWORD _mThdHandler;
	CThreadMgr* _mThreadMgr;
};

unsigned Start(void *pthis);