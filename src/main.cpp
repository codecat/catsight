#include <Common.h>
#include <Explorer.h>

#if defined(PLATFORM_LINUX)
#include <sys/mman.h>
#endif

void do_something()
{
	printf("Something!\n");
}

int main()
{
	srand(time(nullptr));

#if defined(PLATFORM_LINUX)
	const size_t testMemSize = 0x1000000;
	void* testMem = mmap((void*)0x1000000, testMemSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE, -1, 0);
	memset(testMem, 0x00, testMemSize);
	for (auto p = (uintptr_t*)testMem; p < (uintptr_t*)((uintptr_t)testMem + testMemSize); p++) {
		if (((uintptr_t)p & 0xF) == 0) {
			*p = (uintptr_t)p;
		}
	}
	((void**)testMem)[1] = (void*)&do_something;
	((void**)testMem)[3] = (void*)&main;
#endif

	Explorer::Instance = new Explorer();
	Explorer::Instance->Run();
	delete Explorer::Instance;
	Explorer::Instance = nullptr;
	return 0;
}
