#include <Common.h>
#include <Inspector.h>
#include <System.h>

#include <Tabs/MemoryTab.h>

#include <hello_imgui.h>

Inspector::Inspector(const ProcessInfo& info)
	: m_processInfo(info)
{
	m_title = m_processInfo.filename;
	m_processHandle = System::OpenProcessHandle(info);

	auto regions = m_processHandle->GetMemoryRegions();
	if (regions.len() > 0) {
		for (int i = 0; i < 5; i++) {
			m_tabs.add(new MemoryTab(this, s2::strprintf("Memory %d", i + 1), regions[0].m_start));
		}
	}
}

Inspector::~Inspector()
{
	for (auto tab : m_tabs) {
		delete tab;
	}

	if (m_processHandle != nullptr) {
		delete m_processHandle;
	}
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
					ImGui::BeginChild("TabContent");
					tab->Render();
					ImGui::EndChild();
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
