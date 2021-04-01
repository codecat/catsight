#include <Common.h>
#include <Explorer.h>

#if defined(PLATFORM_LINUX)
#  include <sys/mman.h>
#elif defined(PLATFORM_WINDOWS)
#  include <Windows.h>
#endif

void do_something()
{
	printf("Something!\n");
}

int main()
{
	srand(time(nullptr));

	// Create test memory page
	const size_t testMemSize = 0x1000000;
#if defined(PLATFORM_LINUX)
	uintptr_t* testMem = (uintptr_t*)mmap((void*)0x1000000, testMemSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE, -1, 0);
#elif defined(PLATFORM_WINDOWS)
	uintptr_t* testMem = (uintptr_t*)VirtualAlloc((void*)0x1000000, testMemSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
#endif

	memset(testMem, 0, testMemSize);

	testMem[1] = (uintptr_t)"Pointer chain to code";
	for (int i = 2; i <= 6; i++) {
		testMem[i] = (uintptr_t)(testMem + i + 1);
	}
	testMem[6] = (uintptr_t)&main;

	testMem[10] = (uintptr_t)"Pointer chain to float";
	for (int i = 10; i <= 14; i++) {
		testMem[i] = (uintptr_t)(testMem + i + 1);
	}
	*(float*)(testMem + 14) = 1.23f;

	testMem[17] = (uintptr_t)"Pointer to itself";
	testMem[18] = (uintptr_t)(testMem + 18);

	testMem[20] = (uintptr_t)"Floats";
	auto floatArray = (float*)(testMem + 21);
	floatArray[0] = 1.0f;
	for (int i = 1; i < 5; i++) {
		floatArray[i] = 1.0f * (i / 2.0f);
	}

	// Initialize and run application
	Explorer::Instance = new Explorer();
	Explorer::Instance->Run();
	delete Explorer::Instance;
	Explorer::Instance = nullptr;
	return 0;
}
