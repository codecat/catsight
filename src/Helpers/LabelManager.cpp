#include <Common.h>
#include <Helpers/LabelManager.h>
#include <Inspector.h>

LabelManager::LabelManager(Inspector* inspector)
	: m_inspector(inspector)
{
}

LabelManager::~LabelManager()
{
	for (size_t i = 0; i < m_modules.len(); i++) {
		delete m_modules.at(i);
	}
}

void LabelManager::Set(uintptr_t p, const char* label)
{
	ProcessMemoryRegion region;
	if (!m_inspector->GetMemoryRegion(p, region)) {
		printf("Error: Unable to set label at " POINTER_FORMAT " because it's not within a valid region!\n", p);
		return;
	}

	Set(p, label, region);
}

void LabelManager::Set(uintptr_t p, const char* label, const ProcessMemoryRegion& region)
{
	auto mod = GetModule(region);
	if (mod == nullptr) {
		printf("Error: Unable to set label at " POINTER_FORMAT " because it's not within a module!\n", p);
		return;
	}

	if (mod->m_addresses.contains(label)) {
		printf("Error: Unable to set label at " POINTER_FORMAT " because '%s' already exists!\n", p, label);
		return;
	}

	uintptr_t rva = p - region.m_start;

	s2::string existingName;
	if (mod->m_labels.get(rva, existingName)) {
		mod->m_labels.set(rva, label);
		mod->m_addresses.remove(existingName);
		mod->m_addresses.add(label);
	} else {
		mod->m_addresses.add(label, rva);
		mod->m_labels.add(rva, label);
	}
}

void LabelManager::Remove(uintptr_t p)
{
	ProcessMemoryRegion region;
	if (!m_inspector->GetMemoryRegion(p, region)) {
		printf("Error: Unable to remove label at " POINTER_FORMAT " because it's not within a valid region!\n", p);
		return;
	}

	Remove(p, region);
}

void LabelManager::Remove(uintptr_t p, const ProcessMemoryRegion& region)
{
	auto mod = GetModule(region);
	if (mod == nullptr) {
		printf("Error: Unable to remove label at " POINTER_FORMAT " because it's not within a module!\n", p);
		return;
	}

	uintptr_t rva = p - region.m_start;

	s2::string name;
	if (!mod->m_labels.get(rva, name)) {
		assert(false);
		return;
	}

	mod->m_labels.remove(rva);
	mod->m_addresses.remove(name);
}

bool LabelManager::GetLabel(uintptr_t p, s2::string& label)
{
	ProcessMemoryRegion region;
	if (!m_inspector->GetMemoryRegion(p, region)) {
		return false;
	}
	return GetLabel(p, label, region);
}

bool LabelManager::GetLabel(uintptr_t p, s2::string& label, const ProcessMemoryRegion& region)
{
	auto mod = GetModule(region);
	if (mod == nullptr) {
		return false;
	}

	uintptr_t rva = p - region.m_start;

	return mod->m_labels.get(rva, label);
}

bool LabelManager::GetAddress(const char* label, uintptr_t& p)
{
	ProcessMemoryRegion region;
	if (!m_inspector->GetMemoryRegion(p, region)) {
		return false;
	}
	return GetAddress(label, p, region);
}

bool LabelManager::GetAddress(const char* label, uintptr_t& p, const ProcessMemoryRegion& region)
{
	auto mod = GetModule(region);
	if (mod == nullptr) {
		return false;
	}

	uintptr_t ret;
	if (!mod->m_addresses.get(label, ret)) {
		return false;
	}

	p = region.m_start + ret;

	return true;
}

LabelManager::Module* LabelManager::GetModule(const ProcessMemoryRegion& region)
{
	if (region.m_path == "") {
		return nullptr;
	}

	Module* mod = nullptr;
	if (!m_modules.get(region.m_path, mod)) {
		mod = new Module;
		mod->m_path = region.m_path;
		m_modules.add(region.m_path, mod);
	}
	return mod;
}
