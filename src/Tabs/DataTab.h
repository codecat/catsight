#pragma once

#include <Common.h>
#include <Tabs/MemoryTab.h>

class DataTab : public MemoryTab
{
public:
	uintptr_t m_baseOffset = 0;
	uintptr_t m_baseSize = 0;

private:
	struct LineDetails
	{
		bool m_memoryExecutable = false;
		bool m_pointsToExecutable = false;
		uintptr_t m_pointsToExecutableValue = 0;
	};
	s2::list<LineDetails> m_lineDetails;

public:
	DataTab(Inspector* inspector, const s2::string& id, uintptr_t p);
	virtual ~DataTab();

	uint16_t RenderMember(uintptr_t offset, uint16_t relativeOffset, intptr_t displayOffset, int lineIndex);

	virtual const char* GetTitlePrefix() override;

	virtual void RenderMenu(float dt) override;
	virtual void Render(float dt) override;
};
