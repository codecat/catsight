#include <Common.h>
#include <Explorer.h>

int main()
{
	Explorer::Instance = new Explorer();
	Explorer::Instance->Run();
	delete Explorer::Instance;
	Explorer::Instance = nullptr;
	return 0;
}
