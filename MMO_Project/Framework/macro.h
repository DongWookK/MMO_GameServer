#pragma once
#include "stdint.h"

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
	if (!(expr))						\
	{								\
		CRASH("ASSERT_CRASH");		\
		__analysis_assume(expr);	\
	}								\
}

/*---------------------------------------------
For Code
---------------------------------------------*/
#ifdef NDEBUG	// NDEBUG is typically defined in release mode
#define RETURN_VALUE(error_code)			\
{ if (error_code != 0) {					\
		return error_code;					\
	}										\
}
#else			// Debug mode
#define RETURN_VALUE(error_code)			\
{ if (error_code != 0) {					\
		ASSERT_CRASH(!(error_code));		\
		return error_code;					\
	}										\
}
#endif

/*---------------------------------------------
For Log
---------------------------------------------*/
#define __FILE _CRT_WIDE(__FILE__) 
#define FILELINE __FILE, __LINE__


namespace fw
{
	using error = int32_t;
}