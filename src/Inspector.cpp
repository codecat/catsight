#include <Common.h>
#include <Inspector.h>
#include <System.h>

#include <Tabs/ConsoleTab.h>
#include <Tabs/MapsTab.h>
#include <Tabs/ModulesTab.h>
#include <Tabs/DataTab.h>

#include <hello_imgui.h>

Inspector::Inspector(const ProcessInfo& info)
	: m_processInfo(info)
{
	m_title = m_processInfo.pathExe;
	m_processHandle = System::OpenProcessHandle(info);

	m_tabs.add(new ConsoleTab(this, "Console"));
	m_tabs.add(new MapsTab(this, "Maps"));
	m_tabs.add(new ModulesTab(this, "Modules"));
	m_tabs.top()->m_shouldFocus = true;

	m_processRegions = m_processHandle->GetMemoryRegions();
	if (m_processRegions.len() > 0) {
		m_tabs.add(new DataTab(this, "Data", m_processRegions[0].m_start));
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

bool Inspector::GetMemoryRegion(uintptr_t p, ProcessMemoryRegion& region)
{
	bool ret = false;
	m_processRegionsMutex.lock();

	for (auto& r : m_processRegions) {
		if (p >= r.m_start && p < r.m_end) {
			region = r;
			ret = true;
			break;
		}
	}

	m_processRegionsMutex.unlock();
	return ret;
}

void Inspector::Render(float dt)
{
	auto windowTitle = s2::strprintf("%s (%d, %s)###Inspector_%s", m_title.c_str(), m_processInfo.pid, m_processInfo.user.username.c_str(), m_processInfo.pathExe.c_str());

	ImGui::SetNextWindowSize(ImVec2(1000, 800), ImGuiCond_FirstUseEver);
	if (ImGui::Begin(windowTitle, &m_isOpen, ImGuiWindowFlags_MenuBar)) {
		if (!m_processHandle->IsOpen()) {
			ImGui::TextDisabled("Unable to open process handle! Your system user probably doesn't have permissions to access arbitrary processes.");
		} else {
			RenderMenu(dt);
			RenderTabs(dt);
		}
	}
	ImGui::End();

	// Don't allow closing window if there's still tasks running
	if (!m_isOpen && m_tasks.GetActiveWorkerCount() > 0) {
		m_isOpen = true;
	}
}

void Inspector::Update(float dt)
{
	m_tasks.Update();
}

void Inspector::RenderMenu(float dt)
{
	if (ImGui::BeginMenuBar()) {
		int activeTaskWorkers = m_tasks.GetActiveWorkerCount();
		if (activeTaskWorkers > 0) {
			ImGui::TextDisabled(ICON_FA_CLOCK " %d", activeTaskWorkers);
		}

		if (ImGui::BeginMenu("Tabs")) {
			if (ImGui::MenuItem("New data tab")) {
				if (m_processRegions.len() > 0) {
					m_tabs.add(new DataTab(this, "Data", m_processRegions[0].m_start));
				}
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
}

void Inspector::RenderTabs(float dt)
{
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
				if (tab->RenderBegin(dt)) {
					tab->Render(dt);
					tab->RenderEnd(dt);
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
			activeTab->RenderMenu(dt);
		}
		ImGui::EndMenuBar();
	}
}
