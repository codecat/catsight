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
	};

	s2::string m_stringBuffer;
	s2::list<LineDetails> m_lineDetails;

public:
	DataTab(Inspector* inspector, const s2::string& name, uintptr_t p);
	virtual ~DataTab();

	uint16_t RenderMember(uintptr_t offset, uint16_t relativeOffset, intptr_t displayOffset, int lineIndex);

	virtual s2::string GetLabel() override;

	virtual void RenderMenu() override;
	virtual void Render() override;
};
