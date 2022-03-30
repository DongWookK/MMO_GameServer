#include "pch.h"

class a
{
	int b;
};
int main()
{
	a* b = new a;
	ASSERT_CRASH(b != nullptr);
	return 0;
}