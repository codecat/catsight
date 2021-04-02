#include <Common.h>
#include <Tab.h>

Tab::Tab(Inspector* inspector, const s2::string& id)
{
	m_inspector = inspector;
	m_id = id;
}

Tab::~Tab()
{
}

s2::string Tab::GetLabel()
{
	const char* name = GetName();
	const char* id = GetID();
	const char* prefix = GetTitlePrefix();
	s2::string suffix = GetTitleSuffix();

	s2::string ret;

	// Prefix
	if (prefix != nullptr) {
		ret.append(prefix);
		ret.append(' ');
	}

	// Name
	ret += name;

	// Suffix
	if (suffix != "") {
		ret += " " + suffix;
	}

	// ID
	ret += "###";
	ret += id;

	return ret;
}

const char* Tab::GetName()
{
	return m_id;
}

const char* Tab::GetID()
{
	return m_id;
}

const char* Tab::GetTitlePrefix()
{
	return nullptr;
}

s2::string Tab::GetTitleSuffix()
{
	return "";
}

bool Tab::CanClose()
{
	return true;
}

void Tab::RenderMenu(float dt)
{
}

bool Tab::RenderBegin(float dt)
{
	return true;
}

void Tab::Render(float dt)
{
}

void Tab::RenderEnd(float dt)
{
}
