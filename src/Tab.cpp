#include <Common.h>
#include <Tab.h>

Tab::Tab(Inspector* inspector, const s2::string& name)
{
	m_inspector = inspector;
	m_name = name;
}

Tab::~Tab()
{
}

s2::string Tab::GetLabel()
{
	return m_name;
}

bool Tab::CanClose()
{
	return true;
}