
#include "CThreadMgr.h"
#include "pch.h"

int32 CThreadMgr::open()
{
	InitTLS();
	//1)������Ǯ ����
	//2)������������ �迭 �ϳ��� loop���鼭 launch
	return int32();
}

int32 CThreadMgr::openWk()
{
	uint32 aRv;
	//DB���� �����尹�� �������� (���� �ӽ÷� 5��)
	for (int i = 0; i < WkThreadNo; i++)
	{
		//������Ǯ���� acquire   (�ӽ÷� New)
		
	}
	
}

int32 CThreadMgr::InitTLS()
{
	SThreadId = 0;
}

