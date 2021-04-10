#pragma once

#include <Common.h>
#include <Tab.h>
#include <System/ProcessMemoryRegion.h>
#include <Helpers/TypeRenderer.h>

class MemoryTab : public Tab
{
public:
	bool m_hasValidRegion = false;
	ProcessMemoryRegion m_region;

protected:
	TypeRenderer m_typeRenderer;

	float m_itemHeight = 22.0f;
	size_t m_maxHistoryItems = 25;
	uintptr_t m_addressMask = 0;
	bool m_showScrollBar = true;

protected:
	bool m_invalidated = false; // Can be used for particularly expensive operations mid-rendering
	intptr_t m_topOffset = 0;
	intptr_t m_topOffsetMax = 0;
	int m_itemsPerPage = 0;

	s2::list<uintptr_t> m_history;
	int m_historyIndex = -1;

private:
	bool m_ui_gotoPopupShow = false;
	uintptr_t m_ui_gotoAddress = 0;
	s2::string m_ui_gotoAddressString;

	bool m_ui_setLabelPopupShow = false;
	uintptr_t m_ui_setLabelAddress = 0;
	s2::string m_ui_setLabelString;

	bool m_wasInvalidatedAtBegin = false;

public:
	MemoryTab(Inspector* inspector, const s2::string& id, uintptr_t p);
	virtual ~MemoryTab();

	virtual void SetRegion(const ProcessMemoryRegion& region);
	virtual void SetRegion(uintptr_t p);

	virtual void GoTo(uintptr_t p, bool addToHistory = true);

	virtual void AddHistory();
	virtual void LimitHistorySize();
	virtual void GoToHistory(int offset);

	virtual void ScrollTo(uintptr_t p);
	virtual void ScrollToOffset(uintptr_t offset);

	virtual s2::string GetTitleSuffix() override;

	virtual void RenderMenu(float dt) override;
	virtual bool RenderBegin(float dt) override;
	virtual void RenderEnd(float dt) override;

protected:
	virtual intptr_t GetScrollAmount(int wheel);

	virtual void RenderAddress(uintptr_t p);
	virtual void RenderAddressContextMenu(uintptr_t p);
};
