#include "pch.h"
#include "Thread.h"

void Thread::main()
{
	this_thread::sleep_for(chrono::seconds(1));
	cout << this_thread::get_id << endl;

	//��� �ݺ����� ������?
}
