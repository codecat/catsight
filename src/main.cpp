#include <Common.h>

#include <hello_imgui/hello_imgui.h>

#include <Folder.h>

#include <unistd.h>

struct ProcessInfo
{
	s2::string exe;
	int pid = -1;
};
static s2::list<ProcessInfo> g_processes;

void render_main_menu_file()
{
	if (ImGui::BeginMenu("File")) {
		if (ImGui::BeginMenu("Attach")) {
			if (ImGui::IsWindowAppearing()) {
				g_processes.clear();

				printf("Reading procs list\n");

				char linkBuffer[256];

				FolderIndex fi("/proc/", false);
				for (int i = 0; i < fi.GetDirCount(); i++) {
					s2::string procPath = fi.GetDirPath(i);

					int pid = -1;
					sscanf(procPath.c_str(), "/proc/%d/", &pid);
					if (pid == -1) {
						continue;
					}

					ssize_t s = readlink(procPath + "exe", linkBuffer, sizeof(linkBuffer));
					if (s == -1) {
						continue;
					}
					linkBuffer[s] = '\0';

					auto& newProcess = g_processes.push();
					newProcess.exe = linkBuffer;
					newProcess.pid = pid;
				}
			}

			for (auto& proc : g_processes) {
				if (ImGui::MenuItem(proc.exe)) {
					//
				}
			}

			ImGui::EndMenu();
		}
		ImGui::EndMenu();
	}
}

void render_main_menu()
{
	if (ImGui::BeginMainMenuBar()) {
		render_main_menu_file();
		ImGui::EndMainMenuBar();
	}
}

void render_interface()
{
	render_main_menu();
}

int main()
{
	HelloImGui::Run(render_interface, ImVec2(800, 600), "Catsight");
	return 0;
}
