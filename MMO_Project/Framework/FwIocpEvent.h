#pragma once

class Session;

enum class EventType : uint8
{
	Connect,
	Disconnect,
	Accept,
	//PreRecv,
	Recv,
	Send
};

/*--------------
	IocpEvent
---------------*/

class FwIocpEvent : public OVERLAPPED
{
public:
	FwIocpEvent(EventType type);

	void			Init();

public:
	EventType		eventType;
	FwIocpObjectRef	owner;
};

/*----------------
	ConnectEvent
-----------------*/

class ConnectEvent : public FwIocpEvent
{
public:
	ConnectEvent() : FwIocpEvent(EventType::Connect) { }
};

/*--------------------
	DisconnectEvent
----------------------*/

class DisconnectEvent : public FwIocpEvent
{
public:
	DisconnectEvent() : FwIocpEvent(EventType::Disconnect) { }
};

/*----------------
	AcceptEvent
-----------------*/

class AcceptEvent : public FwIocpEvent
{
public:
	AcceptEvent() : FwIocpEvent(EventType::Accept) { }

public:
	SessionRef	session = nullptr;
};

/*----------------
	RecvEvent
-----------------*/

class RecvEvent : public FwIocpEvent
{
public:
	RecvEvent() : FwIocpEvent(EventType::Recv) { }
};

/*----------------
	SendEvent
-----------------*/

class SendEvent : public FwIocpEvent
{
public:
	SendEvent() : FwIocpEvent(EventType::Send) { }

	Vector<SendBufferRef> sendBuffers;
};