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


	struct A{
		int a;

	};
	struct B{
		long long b;
		long long c;
	};
	A a;
	B b;

	cout << "size of a:" << sizeof(a) << endl;
	cout << "size of b:" << sizeof(b) << endl;
	
	variant<struct A, struct B> v, w;
	v = a; // v contains int
	
	cout << "size of variant v:" << sizeof(v) << endl;


	


	system("pause");
	return 0;

}