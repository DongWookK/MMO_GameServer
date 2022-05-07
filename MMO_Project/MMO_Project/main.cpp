#include "pch.h"
#include "CThreadMgr.h"
#include "CThreadBase.h"

int main()
{
	// ASSERT_CRASH(b != nullptr);


	//ThreadMgr를 통해서 5개 Thread 생성 
	CThreadMgr::This()->Open();


	while (1)
	{
		this_thread::sleep_for(5s);
		CThreadMgr::This()->Close();
	}
	return 0;
}