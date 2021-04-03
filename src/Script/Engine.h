#pragma once

#include <Common.h>

namespace sol { class state; }

namespace Script
{
	class Engine
	{
	private:
		sol::state* m_state;

	public:
		s2::string m_log;
		bool m_logInvalidated = false;

	public:
		Engine();
		~Engine();

		void ExecuteConsole(const char* sz);
	};
}
