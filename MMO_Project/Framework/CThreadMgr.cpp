#include "pch.h"
#include "CThreadMgr.h"
#include "CThreadBase.h"

int32 CThreadMgr::Open()
{
	// threadwk ����
	const auto& aInit = [this]()->DWORD {return this->Open(); };

	return 0;
}

int32 CThreadMgr::Close()
{
	//flag ������ loop���� thread���� �ߴ��ϰԲ�. ( ���� Pool����̾ƴ� vector ����Ʈ�̹Ƿ� ���߿� freelist�� ���ư��Բ�?)
	_mFlag = Flag::STOPPED;
	return 0;
}

int32 CThreadMgr::OpenWk()
{
	uint32 aRv;
	return 0;
	//DB���� �����尹�� �������� (���� �ӽ÷� 5��)
	
	
}
