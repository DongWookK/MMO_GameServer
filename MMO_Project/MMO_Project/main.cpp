#include "pch.h"
#include "ThreadBase.h"
class a
{
	int b;
};
int main()
{
	a* b = new a;
	ASSERT_CRASH(b != nullptr);

	GThreadBase.Open();
	
	return 0;
}