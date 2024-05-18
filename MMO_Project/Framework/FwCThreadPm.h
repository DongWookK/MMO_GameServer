#pragma once

/*
	TODO
	- 전체 프로세스의 시작, 종료 처리
	- 리소스 생성에 필요한 필수 정보들 로드 (DB, File)
	- 리소스 생성
*/

class FwCThreadPm : public Singleton<FwCThreadPm>
{
public:
	void		Start(void);
	void		Terminate(void);
private:
	void		Load(void);
	void		LoadFromDB(void);
	void		LoadFromFile(void);
};

