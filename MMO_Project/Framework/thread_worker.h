#pragma once
#include "pch.h"

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