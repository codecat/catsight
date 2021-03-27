#pragma once

#include <Common.h>

#include <imgui.h>

namespace Helpers
{
	bool InputText(const char* label, s2::string* str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = nullptr, void* user_data = nullptr);
}
