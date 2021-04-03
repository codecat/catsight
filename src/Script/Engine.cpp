#include <Common.h>
#include <Script/Engine.h>

#include <Script/Types/Pointer.h>

#include <sol/sol.hpp>

Script::Engine::Engine()
{
	m_state = new sol::state;

	m_state->open_libraries(
		sol::lib::base,
		sol::lib::package,
		sol::lib::string,
		sol::lib::os,
		sol::lib::math,
		sol::lib::table,
		sol::lib::debug,
		sol::lib::bit32,
		sol::lib::io
	);

	m_state->set("print", [this](sol::variadic_args va) {
		auto L = va.lua_state();
		for (size_t i = 0; i < va.size(); i++) {
			size_t len = 0;
			const char* str = luaL_tolstring(L, i + 2, &len);
			if (i > 0) {
				m_log.append('\t');
			}
			m_log.append(str);
			lua_pop(L, 1);
		}
		m_log.append('\n');
		m_logInvalidated = true;
	});

	m_state->new_usertype<Script::Types::Pointer>("ptr");
}

Script::Engine::~Engine()
{
	delete m_state;
}

void Script::Engine::ExecuteConsole(const char* sz)
{
	m_log.appendf("> %s\n", sz);

	// This kinda imitates what is happening in lua.c

	// Try with "return" in front first
	auto strExecReturn = s2::strprintf("return %s", sz);
	auto result = m_state->safe_script(strExecReturn.c_str(), sol::script_pass_on_error);
	if (!result.valid()) {
		// With "return" in front failed, try to run it normally
		result = m_state->safe_script(sz, sol::script_pass_on_error);
		if (!result.valid()) {
			// That also failed, this really is an error
			sol::error err = result;
			m_log.appendf("%s\n", err.what());
			m_logInvalidated = true;
			return;
		}
	}

	auto L = result.lua_state();
	for (auto r : result) {
		size_t len = 0;
		const char* str = luaL_tolstring(L, r.stack_index(), &len);
		m_log.append(str);
		m_log.append('\n');
		m_logInvalidated = true;
		lua_pop(L, 1);
	}
}
