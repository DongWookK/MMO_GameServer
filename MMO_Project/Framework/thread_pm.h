#pragma once
#include "pch_fw.h"

/*
	TODO
	- ��ü ���μ����� ����, ���� ó��
	- ���ҽ� ������ �ʿ��� �ʼ� ������ �ε� (DB, File)
	- ���ҽ� ����
*/

class worker
{
public:
	worker();

public:
	auto allocate_job() -> const uint32_t;
	static auto job() -> const uint32_t;

private:
	std::thread thread_; // �ܼ��� ����� thread�� ������ �ִ»��¿����� �ƹ������ ���� �ʴ´�.
	// jthread�� stop �� �����ϰ��Ѵ�?
};

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