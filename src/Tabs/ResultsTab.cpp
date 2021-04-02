#include <Common.h>
#include <Tabs/ResultsTab.h>
#include <Inspector.h>
#include <Resources.h>

#include <hello_imgui.h>

ResultsTab::ResultsTab(Inspector* inspector, const s2::string& name)
	: TaskWaitTab(inspector, name)
{
	m_results.ensure_memory(1000);
}

ResultsTab::~ResultsTab()
{
}

void ResultsTab::Render(float dt)
{
	TaskWaitTab::Render(dt);

	ImGui::BeginChild("Items");
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 0));

	ImGuiListClipper clipper;
	clipper.Begin((int)GetNumResults());
	while (clipper.Step()) {
		for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {
			ImGui::PushID(i);
			RenderResult(GetResult(i));
			ImGui::PopID();
		}
	}

	ImGui::PopStyleVar();
	ImGui::EndChild();
}

void ResultsTab::RenderResult(const Result& result)
{
	ImGui::PushFont(Resources::FontMono);
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(.5f, .5f, .5f, 1));
	ImGui::Text(POINTER_FORMAT, result.m_address);
	ImGui::PopStyleColor();
	ImGui::PopFont();
}

size_t ResultsTab::GetNumResults()
{
	return m_results.len();
}

const ResultsTab::Result& ResultsTab::GetResult(size_t i)
{
	return m_results[i];
}
