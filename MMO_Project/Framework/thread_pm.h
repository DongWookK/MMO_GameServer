#pragma once

/*
	TODO
	- ��ü ���μ����� ����, ���� ó��
	- ���ҽ� ������ �ʿ��� �ʼ� ������ �ε� (DB, File)
	- ���ҽ� ����
*/

class thread_pm : public Singleton<thread_pm>
{
public:
	void		Start(void);
	void		Terminate(void);
private:
	void		Load(void);
	void		LoadFromDB(void);
	void		LoadFromFile(void);
};
