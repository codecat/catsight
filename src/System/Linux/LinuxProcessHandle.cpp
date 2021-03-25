#include <Common.h>
#include <System/Linux/LinuxProcessHandle.h>

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

size_t LinuxProcessHandle::ReadMemory(uintptr_t p, void* buffer, size_t size)
{
	fseeko(m_fhMemory, (off_t)p, SEEK_SET);
	return fread(buffer, 1, size, m_fhMemory);
}

s2::list<ProcessMemoryRegion> LinuxProcessHandle::GetMemoryRegions()
{
	s2::list<ProcessMemoryRegion> ret;

	FILE* fh = fopen(s2::strprintf("/proc/%d/maps", m_pid), "rb");
	while (!feof(fh)) {
		char line[512];
		if (fgets(line, 512, fh) == nullptr) {
			break;
		}

		line[strlen(line) - 1] = '\0';

		LinuxMemoryMapInfo mi;
		mi.mi_matches = sscanf(line, "%p-%p %c%c%c%c %p %02hhx:%02hhx %lu %s",
			&mi.mi_base, &mi.mi_end,
			&mi.mi_flags.read, &mi.mi_flags.write, &mi.mi_flags.execute, &mi.mi_flags.protection,
			&mi.mi_offset,
			&mi.mi_device.major, &mi.mi_device.minor,
			&mi.mi_inode,
			mi.mi_path
		);

		if (mi.mi_matches >= 10) {
			auto& region = ret.add();
			region.m_start = (uintptr_t)mi.mi_base;
			region.m_end = (uintptr_t)mi.mi_end;
		}
	}
	fclose(fh);

	return ret;
}
