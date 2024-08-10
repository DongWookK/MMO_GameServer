#pragma once
/*--------------------------------
	Thread Local Storage
desc :  �� �����庰 �������� ����
--------------------------------*/

#include <stack>

extern thread_local uint32_t			LThreadId;
extern thread_local std::stack<int32_t>	LLockStack;
extern thread_local SendBufferChunkRef	LSendBufferChunk;