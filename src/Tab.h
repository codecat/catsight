#pragma once

class Tab
{
public:
	bool m_isOpen = true;

public:
	virtual ~Tab() = 0;

	virtual s2::string GetLabel() = 0;

	virtual void Render() = 0;
};
