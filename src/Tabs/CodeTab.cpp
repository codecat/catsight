#include <Common.h>
#include <Tabs/CodeTab.h>

#include <hello_imgui.h>

CodeTab::CodeTab(Inspector* inspector, const s2::string& name, uintptr_t p)
	: Tab(inspector, name)
{
}

CodeTab::~CodeTab()
{
}

void CodeTab::GoTo(uintptr_t p)
{
}

void CodeTab::Render()
{
	if (ImGui::IsWindowAppearing()) {
		printf("Hello code tab\n");
	}
}
