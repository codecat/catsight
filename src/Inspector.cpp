#include <Common.h>
#include <Inspector.h>
#include <System.h>
#include <Random.h>

#include <Tabs/MapsTab.h>
#include <Tabs/DataTab.h>

#include <hello_imgui.h>

Inspector::Inspector(const ProcessInfo& info)
	: m_processInfo(info)
{
	m_title = m_processInfo.filename;
	m_processHandle = System::OpenProcessHandle(info);

	m_tabs.add(new MapsTab(this, "Maps"));

	auto regions = m_processHandle->GetMemoryRegions();
	if (regions.len() > 0) {
		m_tabs.add(new DataTab(this, "Memory", regions[0].m_start));
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
	auto windowTitle = s2::strprintf("%s (%d, %s)###Inspector_%s", m_title.c_str(), m_processInfo.pid, m_processInfo.user.username.c_str(), m_processInfo.filename.c_str());

	ImGui::SetNextWindowSize(ImVec2(1000, 800), ImGuiCond_FirstUseEver);
	if (ImGui::Begin(windowTitle, &m_isOpen, ImGuiWindowFlags_MenuBar)) {
		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("Tabs")) {
				if (ImGui::MenuItem("New memory tab")) {
					auto regions = m_processHandle->GetMemoryRegions();
					if (regions.len() > 0) {
						m_tabs.add(new DataTab(this, "Memory", regions[0].m_start));
					}
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		Tab* activeTab = nullptr;

		if (ImGui::BeginTabBar("Tabs", ImGuiTabBarFlags_AutoSelectNewTabs)) {
			for (size_t i = 0; i < m_tabs.len(); i++) {
				auto tab = m_tabs[i];
				ImGui::PushID(tab);

				ImGuiTabItemFlags tabFlags = ImGuiTabItemFlags_None;
				if (tab->m_shouldFocus) {
					tabFlags |= ImGuiTabItemFlags_SetSelected;
					tab->m_shouldFocus = false;
				}

				bool tabOpen = true;
				bool tabVisible = true;
				if (tab->CanClose()) {
					tabVisible = ImGui::BeginTabItem(tab->GetLabel(), &tabOpen, tabFlags);
				} else {
					tabVisible = ImGui::BeginTabItem(tab->GetLabel(), nullptr, tabFlags);
				}

				if (tabVisible) {
					activeTab = tab;

					ImGui::BeginChild("TabContent");
					if (tab->RenderBegin()) {
						tab->Render();
						tab->RenderEnd();
					}
					ImGui::EndChild();
					ImGui::EndTabItem();
				}

				if (!tabOpen) {
					activeTab = nullptr;
					delete tab;
					m_tabs.remove(i);
					i--;
				}

				ImGui::PopID();
			}

			ImGui::EndTabBar();
		}

		if (ImGui::BeginMenuBar()) {
			if (activeTab != nullptr) {
				activeTab->RenderMenu();
			}
			ImGui::EndMenuBar();
		}
	}
	ImGui::End();
}
