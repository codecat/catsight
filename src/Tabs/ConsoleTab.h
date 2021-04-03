#pragma once

#include <Common.h>
#include <Tab.h>

class ConsoleTab : public Tab
{
private:
	s2::string m_input;

public:
	ConsoleTab(Inspector* inspector, const s2::string& id);
	~ConsoleTab();

	virtual const char* GetTitlePrefix() override;

	virtual bool CanClose() override;

	virtual void Render(float dt) override;
};
