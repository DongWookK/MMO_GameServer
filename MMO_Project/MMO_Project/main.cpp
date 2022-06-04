#include "pch.h"
#include "CThreadMgr.h"
#include "CThreadBase.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include <variant>

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


	struct Msg{
		wchar_t* msg[10];

	};
	struct Number{
		uint64 no;
		float prime;
	};
	
	variant<struct Msg, struct Number> var{};
	copy_n(L"Hello", 6, var);
	
	
	cout << "content of variant : " << get<string>(var) << endl;
	


	


	system("pause");
	return 0;

}