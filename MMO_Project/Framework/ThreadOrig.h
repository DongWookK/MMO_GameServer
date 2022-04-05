#pragma once

class ThreadOrig
{
	virtual uint32 Open();
	static unsigned __stdcall Start(void *pThis);
	virtual uint32 Close();
	virtual void Main();
};

