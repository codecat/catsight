#pragma once

#include <Common.h>
#include <Tab.h>
#include <System/ProcessMemoryRegion.h>

class MemoryTab : public Tab
{
public:
	bool m_hasValidRegion;
	ProcessMemoryRegion m_region;

	uintptr_t m_baseOffset = 0;
	uintptr_t m_baseSize = 0;

private:
	intptr_t m_scrollToOffset = -1;
	s2::string m_stringBuffer;

public:
	MemoryTab(Inspector* inspector, const s2::string& name, uintptr_t p);
	virtual ~MemoryTab();

	void SetRegion(const ProcessMemoryRegion& region, uintptr_t baseOffset = 0, uintptr_t baseSize = 0);
	void SetRegion(uintptr_t p, uintptr_t baseOffset = 0, uintptr_t baseSize = 0);

	void GoTo(uintptr_t p);

	void ScrollTo(uintptr_t p);
	void ScrollToOffset(uintptr_t offset);

	uint16_t RenderMember(uint16_t offset, uint16_t relativeOffset, intptr_t displayOffset);

	virtual s2::string GetLabel() override;

	virtual void RenderMenu() override;
	virtual void Render() override;
};
