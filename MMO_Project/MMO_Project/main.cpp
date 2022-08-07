#include "pch.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

int main()
{
	// ASSERT_CRASH(b != nullptr);


	//ThreadMgr를 통해서 5개 Thread 생성 
	//CThreadMgr::This()->Open();


	//while (1)
	//{
	//	this_thread::sleep_for(5s);
	//	CThreadMgr::This()->Close();
	//}


	
	CThreadMgr::This()->Open();
	

	system("pause");
	return 0;

}