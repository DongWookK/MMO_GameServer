#pragma once
/*--------------------------------
	Thread Local Storage
desc :  각 스레드별 개별저장 변수
--------------------------------*/

thread_local uint32 LThreadId = 0;
thread_local std::stack<int32>	LLockStack;
//extern thread_local SendBufferChunkRef	LSendBufferChunk;


static Atomic<uint32> SThreadId = 0;