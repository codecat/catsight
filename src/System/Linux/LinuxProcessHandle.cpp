#include <Common.h>
#include <System/Linux/LinuxProcessHandle.h>

#include <elfio/elfio.hpp>

struct LinuxMemoryMapInfo
{
	int mi_matches;
	void* mi_base;
	void* mi_end;
	struct
	{
		char read;
		char write;
		char execute;
		char protection;
	} mi_flags;
	void* mi_offset;
	struct
	{
		uint8_t major;
		uint8_t minor;
	} mi_device;
	uint64_t mi_inode;
	char mi_path[1024];
};

LinuxProcessHandle::LinuxProcessHandle(const ProcessInfo& info)
{
	m_pid = info.pid;
	m_fhMemory = fopen(s2::strprintf("/proc/%d/mem", m_pid), "rb");
	if (m_fhMemory == nullptr) {
		printf("ERROR: Unable to open process memory for pid %d!\n", m_pid);
		return;
	}
}

LinuxProcessHandle::~LinuxProcessHandle()
{
	if (m_fhMemory != nullptr) {
		fclose(m_fhMemory);
	}
}

bool LinuxProcessHandle::IsOpen()
{
	return m_fhMemory != nullptr;
}

size_t LinuxProcessHandle::ReadMemory(uintptr_t p, void* buffer, size_t size)
{
	assert(m_fhMemory != nullptr);
	if (m_fhMemory == nullptr) {
		return 0;
	}

	m_readLock.lock();

	fseeko(m_fhMemory, (off_t)p, SEEK_SET);
	size_t ret = fread(buffer, 1, size, m_fhMemory);

	m_readLock.unlock();

	return ret;
}

bool LinuxProcessHandle::IsReadableMemory(uintptr_t p)
{
	assert(m_fhMemory != nullptr);
	if (m_fhMemory == nullptr) {
		return false;
	}

	uint8_t b;
	return ReadMemory(p, &b, 1) == 1;
}

bool LinuxProcessHandle::IsExecutableMemory(uintptr_t p)
{
	FILE* fh = fopen(s2::strprintf("/proc/%d/maps", m_pid), "rb");
	while (!feof(fh)) {
		void* base;
		void* end;

		fscanf(fh, "%p-%p ", &base, &end);
		if (p >= (uintptr_t)base && p < (uintptr_t)end) {
			fseek(fh, 2, SEEK_CUR);
			char c = fgetc(fh);
			fclose(fh);
			return (c != '-');
		}

		while (!feof(fh) && fgetc(fh) != '\n');
	}
	fclose(fh);
	return false;
}

bool LinuxProcessHandle::GetSymbolName(uintptr_t p, s2::string& name)
{
	//TODO
	return false;
}

bool LinuxProcessHandle::GetSymbolAddress(const char* name, uintptr_t& p)
{
	//TODO
	return false;
}

s2::list<ProcessMemoryRegion> LinuxProcessHandle::GetMemoryRegions()
{
	s2::list<ProcessMemoryRegion> ret;

	FILE* fh = fopen(s2::strprintf("/proc/%d/maps", m_pid), "rb");
	while (!feof(fh)) {
		LinuxMemoryMapInfo mi;
		mi.mi_matches = fscanf(fh, "%p-%p %c%c%c%c %p %02hhx:%02hhx %lu",
			&mi.mi_base, &mi.mi_end,
			&mi.mi_flags.read, &mi.mi_flags.write, &mi.mi_flags.execute, &mi.mi_flags.protection,
			&mi.mi_offset,
			&mi.mi_device.major, &mi.mi_device.minor,
			&mi.mi_inode
		);

		for (int i = 0; i < 1024; i++) {
			if (feof(fh)) {
				mi.mi_path[i] = '\0';
				break;
			}

			char c;
			if (i == 0) {
				do {
					c = fgetc(fh);
				} while (c == ' ');
			} else {
				c = fgetc(fh);
			}
			if (c == '\n') {
				mi.mi_path[i] = '\0';
				break;
			}
			mi.mi_path[i] = c;
		}

		if (mi.mi_matches < 10) {
			continue;
		}

		ModuleInfo* pageModule = nullptr;
		if (mi.mi_inode > 0 && s2::file_exists(mi.mi_path)) {
			for (auto& module : m_modules) {
				if (module.m_path == mi.mi_path) {
					pageModule = &module;
					break;
				}
			}
			if (pageModule == nullptr) {
				ELFIO::elfio elf;
				if (elf.load(mi.mi_path)) {
					auto& newModule = m_modules.add();
					newModule.m_path = mi.mi_path;
					newModule.m_start = (uintptr_t)mi.mi_base;

					//TODO: Entry point offset might need to be relative to a section rather than file? Need to check..
					newModule.m_entryPoint = newModule.m_start + elf.get_entry();

					for (auto section : elf.sections) {
						auto& newSection = newModule.m_sections.add();
						newSection.m_offset = section->get_offset();
						newSection.m_size = section->get_size();
						newSection.m_name = section->get_name().c_str();
					}

					pageModule = &m_modules.top();
				}
			}
		}

		SectionInfo* pageSection = nullptr;
		if (pageModule != nullptr) {
			for (auto& section : pageModule->m_sections) {
				if (section.m_offset == (uintptr_t)mi.mi_offset) {
					pageSection = &section;
					break;
				}
			}
		}

		auto& region = ret.add();
		region.m_start = (uintptr_t)mi.mi_base;
		region.m_end = (uintptr_t)mi.mi_end;

		if (mi.mi_flags.read != '-') { region.m_flags |= pmrf_Read; }
		if (mi.mi_flags.write != '-') { region.m_flags |= pmrf_Write; }
		if (mi.mi_flags.execute != '-') { region.m_flags |= pmrf_Execute; }
		if (mi.mi_flags.protection != '-') { region.m_flags |= pmrf_Protect; }

		region.m_path = mi.mi_path;

		if (pageModule != nullptr) {
			region.m_flags |= pmrf_Image;
			region.m_entryPoint = pageModule->m_entryPoint;

			/*
			for (auto& section : pageModule->m_sections) {
				if (section.m_offset >= (uintptr_t)mi.mi_offset && section.m_offset < (uintptr_t)mi.mi_offset + ((uintptr_t)mi.mi_end - (uintptr_t)mi.mi_base)) {
					if (region.m_section.len() > 0) {
						region.m_section += ", ";
					}
					region.m_section += section.m_name;
				}
			}
			*/
		}
		if (pageSection != nullptr) {
			region.m_section = pageSection->m_name;
		}
	}
	fclose(fh);

	return ret;
}
