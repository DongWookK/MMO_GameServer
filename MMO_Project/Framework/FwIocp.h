#pragma once

/*----------------
	IocpObject
-----------------*/

class FwIocpObject : public enable_shared_from_this<FwIocpObject>
{
public:
	virtual HANDLE GetHandle() abstract;
	virtual void Dispatch(class FwIocpEvent* iocpEvent, int32 numOfBytes = 0) abstract;
};

/*--------------
	IocpCore
---------------*/

class FwIocp
{
public:
	FwIocp();
	~FwIocp();

	HANDLE		GetHandle() { return _iocpHandle; }

	bool		Register(FwIocpObjectRef iocpObject);
	bool		Dispatch(uint32 timeoutMs = INFINITE);

private:
	HANDLE		_iocpHandle;
};