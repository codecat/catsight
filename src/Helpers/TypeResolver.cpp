#include <Common.h>
#include <Helpers/TypeResolver.h>
#include <Inspector.h>
#include <Helpers/MemoryValidator.h>
#include <Helpers/CodeButton.h>
#include <Helpers/DataButton.h>

#include <hello_imgui.h>

TypeResolver::TypeResolver(Inspector* inspector)
{
	m_inspector = inspector;
}

TypeResolver::~TypeResolver()
{
}

bool TypeResolver::RenderMenu()
{
	bool ret = false;

	if (ImGui::BeginMenu("Resolve")) {
		if (ImGui::MenuItem("Floats", nullptr, &m_resolveFloats)) {
			ret = true;
		}

#if defined(PLATFORM_64)
		if (ImGui::MenuItem("Pointers must align", nullptr, &m_resolvePointersIfAligned)) {
			ret = true;
		}
#endif

		ImGui::EndMenu();
	}

	return ret;
}

size_t TypeResolver::DetectAndRenderPointer(uintptr_t p, int depth)
{
	// NOTE: The order of which these are checked is important! We should test for the least common types first!

	//TODO: Allow plugins to detect stuff here (and in DetectAndRenderType?)

	auto handle = m_inspector->m_processHandle;
	if (handle->IsReadableMemory(p)) {
		uintptr_t value = handle->Read<uintptr_t>(p);
		if (value == p) {
			ImGui::TextDisabled("(recursive)");
			ImGui::SameLine();
			return sizeof(uintptr_t);
		} else {
			return DetectAndRenderType(value, sizeof(uintptr_t), depth);
		}
	}

	return 0;
}

size_t TypeResolver::DetectAndRenderType(uintptr_t value, size_t limitedSize, int depth)
{
	auto handle = m_inspector->m_processHandle;

	if (limitedSize >= sizeof(uintptr_t)) {
		s2::string str;
		if (MemoryValidator::String(handle, value, str)) {
			str = str.replace("\r", "\\r").replace("\n", "\\n").replace("\t", "\\t");

			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, .5f, 1));
			ImGui::Text("\"%s\"", str.c_str());
			ImGui::PopStyleColor();
			ImGui::SameLine();
			return sizeof(uintptr_t);
		}
	}

	if (m_resolveFloats && limitedSize >= sizeof(float)) {
		float f = *(float*)&value;
		if (MemoryValidator::Float(f)) {
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, .5f, 1));
			ImGui::Text("%f", f);
			ImGui::PopStyleColor();
			ImGui::SameLine();
			return sizeof(float);
		}
	}

	if (limitedSize >= sizeof(uintptr_t) && value != 0 && (value & 0xFFFFFF0) != 0 && handle->IsReadableMemory(value)) {
		if (handle->IsExecutableMemory(value)) {
			Helpers::CodeButton(m_inspector, value, depth);
			ImGui::SameLine();
			return sizeof(uintptr_t);
		}

#if defined(PLATFORM_64)
		// 64 bit pointers are typically aligned to 16 bytes
		bool pointerIsAligned = (value & 0xF) == 0;
#else
		// 32 bit targets don't care about alignment
		bool pointerIsAligned = true;
#endif

		if (!m_resolvePointersIfAligned || pointerIsAligned) {
			Helpers::DataButton(m_inspector, value, depth);
			ImGui::SameLine();
		}

		if (depth < m_resolvePointerMaxDepth) {
			DetectAndRenderPointer(value, depth + 1);
		}

		return sizeof(uintptr_t);
	}

	return 0;
}
