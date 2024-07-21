#pragma once
#include <mutex>
#include <atomic>

namespace fw
{
	using error = int32_t;
}

using FwIocpRef = std::shared_ptr<class FwIocp>;
using FwIocpObjectRef = std::shared_ptr<class FwIocpObject>;
using SessionRef = std::shared_ptr<class Session>;
using PacketSessionRef = std::shared_ptr<class PacketSession>;
using ListenerRef = std::shared_ptr<class Listener>;


using SendBufferRef = std::shared_ptr<class SendBuffer>;
using SendBufferChunkRef = std::shared_ptr<class SendBufferChunk>;