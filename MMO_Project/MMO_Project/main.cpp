#include "pch.h"
#include "CThreadMgr.h"
#include "CThreadBase.h"
class a
{
	int b;
};
int main()
{
	a* b = new a;
	ASSERT_CRASH(b != nullptr);

	CThreadBase* aNew = new CThreadBase;
	HANDLE aRv = aNew->Open();
	//joinÇØÁÖ±â
	return 0;
}