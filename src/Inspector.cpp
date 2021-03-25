#include <Common.h>
#include <Inspector.h>

#include <hello_imgui.h>

Inspector::Inspector(const ProcessInfo& info)
	: m_processInfo(info)
{
	m_title = m_processInfo.filename;
}

const ProcessInfo& Inspector::GetProcessInfo()
{
	return m_processInfo;
}

void Inspector::Render()
{
	auto windowTitle = s2::strprintf("%s##Inspector_%s", m_title.c_str(), m_processInfo.filename.c_str());
	if (ImGui::Begin(windowTitle, &m_isOpen)) {
		ImGui::TextUnformatted("This is an inspector.");
	}
	ImGui::End();
}
