#include "pch.h"
#include "FwCThreadPm.h"
#include "CThreadMgr.h"

void FwCThreadPm::Start(void)
{
	uint32 aErrorCode = 0;

	aErrorCode = CThreadMgr::This()->Open();
	if (0 < aErrorCode)
	{
		// CONSOLE_SYS
	}

	return;
}
