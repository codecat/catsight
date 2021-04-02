#include <Common.h>
#include <Helpers/ImGuiString.h>

template<typename T>
struct InputTextCallback_UserData
{
	T* Str;
	ImGuiInputTextCallback ChainCallback;
	void* ChainCallbackUserData;
};

static int InputTextCallback_s2(ImGuiInputTextCallbackData* data)
{
	auto userdata = (InputTextCallback_UserData<s2::string>*)data->UserData;

	if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
		auto str = userdata->Str;
		assert(data->Buf == str->c_str());
		str->ensure_memory((size_t)data->BufTextLen + 1);
		data->Buf = (char*)str->c_str();

	} else if (data->EventFlag == ImGuiInputTextFlags_CallbackEdit) {
		auto str = userdata->Str;
		size_t* plen = (size_t*)str + 1; // dirty hack but it works!
		*plen = data->BufTextLen;

	} else if (userdata->ChainCallback) {
		data->UserData = userdata->ChainCallbackUserData;
		return userdata->ChainCallback(data);
	}

	return 0;
}

bool Helpers::InputText(const char* label, s2::string* str, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
{
	assert((flags & (ImGuiInputTextFlags_CallbackResize | ImGuiInputTextFlags_CallbackEdit)) == 0);
	flags |= ImGuiInputTextFlags_CallbackResize;
	flags |= ImGuiInputTextFlags_CallbackEdit;

	// Since we make use of the internal buffer, we make sure it's not actually null before using it
	if (str->is_null()) {
		*str = "";
	}

	InputTextCallback_UserData<s2::string> userdata;
	userdata.Str = str;
	userdata.ChainCallback = callback;
	userdata.ChainCallbackUserData = user_data;
	return ImGui::InputText(label, (char*)str->c_str(), str->allocsize(), flags, InputTextCallback_s2, &userdata);
}
