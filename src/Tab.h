#pragma once

class Tab
{
public:
	bool m_shouldFocus = false;

	class Inspector* m_inspector;
	s2::string m_id;

public:
	Tab(Inspector* inspector, const s2::string& id);
	virtual ~Tab();

	virtual s2::string GetLabel();

	virtual const char* GetName();
	virtual const char* GetID();
	virtual const char* GetTitlePrefix();
	virtual s2::string GetTitleSuffix();

	virtual bool CanClose();

	virtual void RenderMenu(float dt);
	virtual bool RenderBegin(float dt);
	virtual void Render(float dt);
	virtual void RenderEnd(float dt);
};
