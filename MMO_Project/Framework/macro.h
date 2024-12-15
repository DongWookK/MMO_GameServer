#pragma once

#define OUT

/*---------------------------------------------
Crash Define
---------------------------------------------*/
#define CRASH(cause)						\
{											\
	unsigned int* crash = nullptr;				\
	__analysis_assume(crash != nullptr);	\
	*crash = 0xDEADBEEF;					\
}

#define ASSERT_CRASH(expr)			\
{									\
	if (!(expr))					\
	{								\
		CRASH("ASSERT_CRASH");		\
		__analysis_assume(expr);	\
	}								\
}

/*---------------------------------------------
For Log
---------------------------------------------*/
#define __FILE _CRT_WIDE(__FILE__) 
#define FILELINE __FILE, __LINE__

/*---------------------------------------------
Simple API
---------------------------------------------*/
#define size16(val)		static_cast<int16_t>(sizeof(val))
#define size32(val)		static_cast<int32_t>(sizeof(val))
#define len16(arr)		static_cast<int16_t>(sizeof(arr)/sizeof(arr[0]))
#define len32(arr)		static_cast<int32_t>(sizeof(arr)/sizeof(arr[0]))