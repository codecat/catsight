#pragma once

class Tab
{
public:
	bool m_isOpen = true;

	class Inspector* m_inspector;
	s2::string m_name;

public:
	Tab(Inspector* inspector, const s2::string& name);
	virtual ~Tab();

	virtual s2::string GetLabel();
	virtual bool CanClose();

	virtual void Render() = 0;
};
