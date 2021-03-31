#include <Common.h>
#include <System/Windows/WindowsProcessHandle.h>

#if defined(PLATFORM_WINDOWS)

#include <Windows.h>
#include <TlHelp32.h>

WindowsProcessHandle::WindowsProcessHandle(const ProcessInfo& info)
{
	m_proc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, info.pid);
	if (m_proc == nullptr) {
		printf("ERROR: Unable to open process for pid %d!\n", info.pid);
	}
}

WindowsProcessHandle::~WindowsProcessHandle()
{
	if (m_proc != nullptr) {
		CloseHandle(m_proc);
	}
}

bool WindowsProcessHandle::IsOpen()
{
	return m_proc != nullptr;
}

size_t WindowsProcessHandle::ReadMemory(uintptr_t p, void* buffer, size_t size)
{
	assert(m_proc != nullptr);
	if (m_proc == nullptr) {
		return 0;
	}

	size_t ret = 0;
	if (!ReadProcessMemory(m_proc, (LPCVOID)p, buffer, size, &ret)) {
		return 0;
	}
	return ret;
}

bool WindowsProcessHandle::IsReadableMemory(uintptr_t p)
{
	assert(m_proc != nullptr);
	if (m_proc == nullptr) {
		return false;
	}

	uint8_t b;
	return ReadMemory(p, &b, 1) == 1;
}

bool WindowsProcessHandle::IsExecutableMemory(uintptr_t p)
{
	assert(m_proc != nullptr);
	if (m_proc == nullptr) {
		return false;
	}

	MEMORY_BASIC_INFORMATION mbi;
	if (VirtualQueryEx(m_proc, (LPCVOID)p, &mbi, sizeof(mbi)) == 0) {
		return false;
	}

	return (mbi.Protect == PAGE_EXECUTE_READ || mbi.Protect == PAGE_EXECUTE_READWRITE || mbi.Protect == PAGE_EXECUTE_WRITECOPY);
}

s2::list<ProcessMemoryRegion> WindowsProcessHandle::GetMemoryRegions()
{
	s2::list<ProcessMemoryRegion> ret;

	// Make a list of all modules
	struct SectionInfo
	{
		uintptr_t m_base;
		s2::string m_name;
	};

	struct ModuleInfo
	{
		uintptr_t m_base;
		MODULEENTRY32 m_mod;
		s2::list<SectionInfo> m_sections;
	};

	s2::list<ModuleInfo> modules;
	modules.ensure_memory(50);

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetProcessId(m_proc));
	MODULEENTRY32 mod;
	if (Module32First(hSnapshot, &mod)) {
		do {
			auto& newModule = modules.add();
			newModule.m_base = (uintptr_t)mod.modBaseAddr;
			newModule.m_mod = mod;

			// Get DOS header
			auto headerDOS = Read<IMAGE_DOS_HEADER>(newModule.m_base);
			if (headerDOS.e_magic != 0x5A4D) { // "MZ"
				continue;
			}

			// Get NT header
			auto headerNT = Read<IMAGE_NT_HEADERS>(newModule.m_base + headerDOS.e_lfanew);
			if (headerNT.Signature != 0x4550) { // "PE\0\0"
				continue;
			}

			//TODO: Do something with headerNT.OptionalHeader.AddressOfEntryPoint (for the attached process)

			// Get sections
			newModule.m_sections.ensure_memory(headerNT.FileHeader.NumberOfSections);
			uintptr_t sectionArrayAddress = newModule.m_base + headerDOS.e_lfanew + FIELD_OFFSET(IMAGE_NT_HEADERS, OptionalHeader) + headerNT.FileHeader.SizeOfOptionalHeader;
			for (WORD i = 0; i < headerNT.FileHeader.NumberOfSections; i++) {
				auto headerSection = Read<IMAGE_SECTION_HEADER>(sectionArrayAddress + i * sizeof(IMAGE_SECTION_HEADER));
				auto& newSection = newModule.m_sections.add();
				newSection.m_base = newModule.m_base + headerSection.VirtualAddress;
				newSection.m_name = (const char*)headerSection.Name;
			}

		} while (Module32Next(hSnapshot, &mod));
	}
	CloseHandle(hSnapshot);

	// Go through every single page in memory
	uintptr_t pageStart = 0;
	while (true) {
		MEMORY_BASIC_INFORMATION mbi;
		if (VirtualQueryEx(m_proc, (LPCVOID)pageStart, &mbi, sizeof(mbi)) == 0) {
			break;
		}

		// Already advance to the next page
		pageStart += mbi.RegionSize;

		// Skip free memory
		if (mbi.State == MEM_FREE) {
			// Go to next page
			continue;
		}

		// See if this is a module with a section
		ModuleInfo* pageModule = nullptr;
		SectionInfo* pageSection = nullptr;

		for (auto& mod : modules) {
			auto base = (uintptr_t)mbi.BaseAddress;
			if (base >= mod.m_base && base < mod.m_base + mod.m_mod.modBaseSize) {
				pageModule = &mod;
				for (auto& section : mod.m_sections) {
					if (base == section.m_base) {
						pageSection = &section;
					}
				}
				break;
			}
		}

		// Add new region
		auto& region = ret.add();
		region.m_start = (uintptr_t)mbi.BaseAddress;
		region.m_end = (uintptr_t)mbi.BaseAddress + mbi.RegionSize;
		if (pageModule != nullptr) {
			region.m_path = pageModule->m_mod.szExePath;
		}
		if (pageSection != nullptr) {
			region.m_section = pageSection->m_name;
		}

		if (mbi.Protect == PAGE_READONLY) {
			region.m_flags = pmrf_Read;
		} else if (mbi.Protect == PAGE_READWRITE || mbi.Protect == PAGE_WRITECOPY) {
			region.m_flags = pmrf_Read | pmrf_Write;
		} else if (mbi.Protect == PAGE_EXECUTE_READ) {
			region.m_flags = pmrf_Read | pmrf_Execute;
		} else if (mbi.Protect == PAGE_EXECUTE_READWRITE || mbi.Protect == PAGE_EXECUTE_WRITECOPY) {
			region.m_flags = pmrf_Read | pmrf_Write | pmrf_Execute;
		}
	}

	return ret;
}

#endif
