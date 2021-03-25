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
		if (ImGui::BeginTabBar("Tabs")) {
			for (size_t i = 0; i < m_tabs.len(); i++) {
				auto tab = m_tabs[i];

				if (ImGui::BeginTabItem(tab->GetLabel(), &tab->m_isOpen)) {
					tab->Render();
					ImGui::EndTabItem();
				}

				if (!tab->m_isOpen) {
					delete tab;
					m_tabs.remove(i);
					i--;
				}
			}

			ImGui::EndTabBar();
		}
	}
	ImGui::End();
}
