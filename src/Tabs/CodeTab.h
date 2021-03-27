#pragma once

#include <Common.h>
#include <Tab.h>

class CodeTab : public Tab
{
private:
	uintptr_t m_address;

public:
	CodeTab(Inspector* inspector, const s2::string& name, uintptr_t p);
	virtual ~CodeTab();

	void GoTo(uintptr_t p);

	virtual void Render() override;
};
