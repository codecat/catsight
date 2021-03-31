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
