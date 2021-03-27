#pragma once

#include <Common.h>
#include <Tab.h>
#include <System/ProcessMemoryRegion.h>

class MemoryTab : public Tab
{
public:
	bool m_hasValidRegion;
	ProcessMemoryRegion m_region;

protected:
	float m_itemHeight = 22.0f;
	uintptr_t m_addressMask = 0;
	bool m_showScrollBar = true;

protected:
	bool m_invalidated = false;
	intptr_t m_topOffset = 0;
	intptr_t m_topOffsetMax = 0;
	int m_itemsPerPage = 0;

private:
	bool m_ui_gotoPopupShow = false;
	uintptr_t m_ui_gotoAddress = 0;
	s2::string m_ui_gotoAddressString;

public:
	MemoryTab(Inspector* inspector, const s2::string& name, uintptr_t p);
	virtual ~MemoryTab();

	virtual void SetRegion(const ProcessMemoryRegion& region);
	virtual void SetRegion(uintptr_t p);

	virtual void GoTo(uintptr_t p);

	virtual void ScrollTo(uintptr_t p);
	virtual void ScrollToOffset(uintptr_t offset);

	virtual void RenderMenu() override;
	virtual bool RenderBegin() override;
	virtual void RenderEnd() override;

protected:
	virtual intptr_t GetScrollAmount(int wheel);

	virtual const char* DetectString(uintptr_t p);
};
