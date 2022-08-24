#pragma once


class CThreadBase
{

public:
	void ResetAttr();
	DWORD Open();
	DWORD Close();
	static unsigned __stdcall Start(void *pThis);
	virtual void Main();

private:

	DWORD _mThdHandler;
	CThreadMgr* _mThreadMgr = nullptr;
};

unsigned Start(void *pthis);