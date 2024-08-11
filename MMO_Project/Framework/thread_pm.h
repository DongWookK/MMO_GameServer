#pragma once
#include "pch_fw.h"

/*
	TODO
	- 전체 프로세스의 시작, 종료 처리
	- 리소스 생성에 필요한 필수 정보들 로드 (DB, File)
	- 리소스 생성
*/

class worker
{
public:
	worker();

public:
	auto allocate_job() -> const uint32_t;
	static auto job() -> const uint32_t;

private:
	std::thread thread_; // 단순히 멤버로 thread를 가지고 있는상태에서는 아무기능을 하지 않는다.
	// jthread로 stop 을 지원하게한다?
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