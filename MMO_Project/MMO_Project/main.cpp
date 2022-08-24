#include "pch.h"
#include "CThreadMgr.h"
#include "CThreadbase.h"

int main()
{
	CThreadBase a;
	a.Open();

	CThreadMgr::This()->Open();
	system("pause");
	return 0;

}