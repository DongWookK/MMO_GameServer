#pragma once
#include "types.h"

/*
	TODO
	- ��ü ���μ����� ����, ���� ó��
	- ���ҽ� ������ �ʿ��� �ʼ� ������ �ε� (DB, File)
	- ���ҽ� ����
*/

class thread_pm : public singleton<thread_pm>
{
public:
	auto setup() -> fw::error;
	auto start() -> fw::error;
	auto stop() -> fw::error;
	auto teardown() -> void;

private:
	void		Load(void);
	void		LoadFromDB(void);
	void		LoadFromFile(void);
};

