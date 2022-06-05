#include "pch.h"
#include "FwIocpEvent.h"

/*--------------
	IocpEvent
---------------*/

FwIocpEvent::FwIocpEvent(EventType type) : eventType(type)
{
	Init();
}

void FwIocpEvent::Init()
{
	OVERLAPPED::hEvent = 0;
	OVERLAPPED::Internal = 0;
	OVERLAPPED::InternalHigh = 0;
	OVERLAPPED::Offset = 0;
	OVERLAPPED::OffsetHigh = 0;
}
