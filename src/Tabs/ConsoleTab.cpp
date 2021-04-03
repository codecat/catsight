#include <Common.h>
#include <Tabs/ConsoleTab.h>
#include <Helpers/ImGuiString.h>
#include <Inspector.h>
#include <Resources.h>

#include <hello_imgui.h>

ConsoleTab::ConsoleTab(Inspector* inspector, const s2::string& id)
	: Tab(inspector, id)
{
}

ConsoleTab::~ConsoleTab()
{
}

const char* ConsoleTab::GetTitlePrefix()
{
	return ICON_FA_TERMINAL;
}

bool ConsoleTab::CanClose()
{
	return false;
}

void ConsoleTab::Render(float dt)
{
	auto& script = m_inspector->m_script;

	ImGui::BeginChild("Log", ImVec2(0, -30));
	ImGui::PushFont(Resources::FontMono);

	ImGui::TextUnformatted(script.m_log);
	if (script.m_logInvalidated) {
		ImGui::SetScrollHereY(1.0f);
		script.m_logInvalidated = false;
	}

	ImGui::PopFont();
	ImGui::EndChild();

	if (ImGui::IsWindowAppearing()) {
		ImGui::SetKeyboardFocusHere();
	}

	ImGui::PushItemWidth(-1);
	if (Helpers::InputText("##input", &m_input, ImGuiInputTextFlags_EnterReturnsTrue)) {
		script.ExecuteConsole(m_input);
		ImGui::SetKeyboardFocusHere(-1);
		m_input = "";
	}
	ImGui::PopItemWidth();
}
