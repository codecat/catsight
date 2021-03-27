#pragma once

class Tab
{
public:
	bool m_shouldFocus = false;

	class Inspector* m_inspector;
	s2::string m_name;

public:
	Tab(Inspector* inspector, const s2::string& name);
	virtual ~Tab();

	virtual s2::string GetLabel();
	virtual bool CanClose();

	virtual void RenderMenu();
	virtual bool RenderBegin();
	virtual void Render();
	virtual void RenderEnd();
};
