#include <Common.h>
#include <Tabs/Memory.h>

#include <hello_imgui.h>

MemoryTab::MemoryTab()
{
}

MemoryTab::~MemoryTab()
{
}

s2::string MemoryTab::GetLabel()
{
	return "Memory";
}

void MemoryTab::Render()
{
	ImGui::TextUnformatted("This is a memory tab");
}
