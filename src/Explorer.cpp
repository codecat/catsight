#include <Common.h>
#include <Explorer.h>
#include <Resources.h>
#include <System.h>

#include <imgui.h>

Explorer* Explorer::Instance = nullptr;

void Explorer::Run()
{
	m_currentUser = System::GetCurrentUser();

	m_params.callbacks.ShowGui = [this]() { Render(); };
	m_params.callbacks.SetupImGuiStyle = [this]() { SetStyle(); };
	m_params.callbacks.LoadAdditionalFonts = [this]() { LoadFonts(); };

	m_params.appWindowParams.windowSize = ImVec2(1200, 900);
	m_params.appWindowParams.windowTitle = "Catsight";

	HelloImGui::Run(m_params);
}

void Explorer::SetStyle()
{
	auto& style = ImGui::GetStyle();
	style.AntiAliasedLines = true;
	style.AntiAliasedFill = true;

	style.WindowRounding = 2;
	style.ChildRounding = 2;

	style.ScrollbarSize = 16;
	style.ScrollbarRounding = 3;

	style.GrabRounding = 2;

	style.ItemSpacing.x = 10;
	style.ItemSpacing.y = 6;

	style.IndentSpacing = 22;

	style.FramePadding.x = 6;
	style.FramePadding.y = 4;

	style.Alpha = 1.0f;
	style.FrameRounding = 3.0f;
	style.TabRounding = 3.0f;

	style.Colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.94f, 0.94f, 0.99f);
	style.Colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.86f, 0.86f, 0.86f, 0.99f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.33f, 0.33f, 0.33f, 0.00f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.69f, 0.69f, 0.69f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_Separator] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
	style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.65f);
	style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.9f);

	// Comment this for light theme
	for (int i = 0; i <= ImGuiCol_COUNT; i++) {
		ImVec4& col = style.Colors[i];
		float H, S, V;
		ImGui::ColorConvertRGBtoHSV(col.x, col.y, col.z, H, S, V);

		if (S < 0.1f) {
			V = 1.0f - V;
		}
		ImGui::ColorConvertHSVtoRGB(H, S, V, col.x, col.y, col.z);
		if (col.w < 1.00f) {
			col.w *= 0.98f;
		}
	}
}

void Explorer::LoadFonts()
{
	auto& io = ImGui::GetIO();
	io.FontDefault = HelloImGui::LoadFontTTF_WithFontAwesomeIcons("fonts/DroidSans.ttf", 16);
	Resources::FontMono = HelloImGui::LoadFontTTF_WithFontAwesomeIcons("fonts/DroidSansMono.ttf", 16);
	Resources::FontBold = HelloImGui::LoadFontTTF_WithFontAwesomeIcons("fonts/DroidSans-Bold.ttf", 16);
}

Inspector* Explorer::GetInspector(const ProcessInfo& info)
{
	return GetInspector(info.pid);
}

Inspector* Explorer::GetInspector(int pid)
{
	for (size_t i = 0; i < m_inspectors.len(); i++) {
		auto inspector = m_inspectors[i];
		if (inspector->GetProcessInfo().pid == pid) {
			return inspector;
		}
	}
	return nullptr;
}

void Explorer::RenderMenu()
{
	if (!System::IsCurrentUserRoot()) {
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 0, 1));
		ImGui::TextUnformatted(ICON_FA_EXCLAMATION_TRIANGLE);
		ImGui::PopStyleColor();
		if (ImGui::IsItemHovered()) {
#if defined(PLATFORM_WINDOWS)
			ImGui::SetTooltip("Not running as administrator!");
#else
			ImGui::SetTooltip("Not running as root user!");
#endif
		}
	}
	ImGui::TextDisabled(ICON_FA_USER " %s", m_currentUser.username.c_str());

	if (ImGui::BeginMenu("File")) {
		if (ImGui::BeginMenu(ICON_FA_FOLDER_OPEN " Open")) {
			if (ImGui::IsWindowAppearing()) {
				printf("Reading procs list\n");
				m_processes = System::GetProcesses();
				m_processes.sort([](const void* pa, const void* pb) {
					auto& a = *(ProcessInfo*)pa;
					auto& b = *(ProcessInfo*)pb;

					// If the process start time is available, sort by that
					if (a.startTime != 0 && b.startTime != 0) {
						if (a.startTime < b.startTime) {
							return 1;
						} else if (a.startTime > b.startTime) {
							return -1;
						}
					}

					// As a fallback, sort by process ID
					if (a.pid < b.pid) {
						return 1;
					} else if (a.pid > b.pid) {
						return -1;
					}

					return 0;
				});
			}

			for (auto& proc : m_processes) {
				const char* icon = "";
				if (m_currentUser == proc.user) {
					icon = ICON_FA_USER " ";
				}

				auto menuText = s2::strprintf("%s%s (%d, %s)", icon, proc.exe.c_str(), proc.pid, proc.user.username.c_str());
				auto existingInspector = GetInspector(proc);

				if (ImGui::MenuItem(menuText, nullptr, nullptr, existingInspector == nullptr)) {
					m_inspectors.add(new Inspector(proc));
				}
			}

			ImGui::EndMenu();
		}

		ImGui::Separator();
		if (ImGui::MenuItem("Quit")) {
			m_params.appShallExit = true;
		}

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu(ICON_FA_BUG " Debug")) {
		if (ImGui::IsWindowAppearing()) {
			printf("Test window appearing\n");
		}
		ImGui::MenuItem("UI metrics", nullptr, &m_metricsVisible);
		ImGui::EndMenu();
	}
}

void Explorer::Render()
{
	if (ImGui::BeginMainMenuBar()) {
		RenderMenu();
		ImGui::EndMainMenuBar();
	}

	if (m_metricsVisible) {
		ImGui::ShowMetricsWindow(&m_metricsVisible);
	}

	for (size_t i = 0; i < m_inspectors.len(); i++) {
		auto inspector = m_inspectors[i];

		inspector->Render();

		if (!inspector->m_isOpen) {
			delete inspector;
			m_inspectors.remove(i);
			i--;
		}
	}
}
