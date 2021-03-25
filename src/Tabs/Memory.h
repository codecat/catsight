#pragma once

#include <Common.h>
#include <Tab.h>

class MemoryTab : public Tab
{
public:
	MemoryTab();
	virtual ~MemoryTab();

	virtual s2::string GetLabel() override;

	virtual void Render() override;
};
