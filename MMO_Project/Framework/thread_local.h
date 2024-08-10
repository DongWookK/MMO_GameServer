#pragma once
/*--------------------------------
	Thread Local Storage
desc :  각 스레드별 개별저장 변수
--------------------------------*/

#include <stack>

extern thread_local uint32_t			LThreadId;
extern thread_local std::stack<int32_t>	LLockStack;
extern thread_local SendBufferChunkRef	LSendBufferChunk;