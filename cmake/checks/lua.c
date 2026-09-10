#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <stdio.h>

static int double_value(lua_State *state)
{
	lua_pushnumber(state, 2 * luaL_checknumber(state, 1));
	return 1;
}

int main(void)
{
	lua_State *state = luaL_newstate();
	if (!state)
		return 1;
	luaL_openlibs(state);
	const luaL_Reg functions[] = {
		{"double_value", double_value},
		{NULL, NULL}
	};
	luaL_register(state, "host", functions);
	lua_pop(state, 1);
	const char *script =
		"assert(_VERSION == 'Lua 5.1')\n"
		"assert(host.double_value(21) == 42)\n"
		"assert(string.format('%04d', 7) == '0007')\n"
		"local t = {}; for i = 1, 10000 do t[i] = {i} end\n"
		"collectgarbage('collect'); assert(t[10000][1] == 10000)\n"
		"local ok = pcall(host.double_value, {})\n"
		"assert(not ok)\n";
	if (luaL_loadstring(state, script) || lua_pcall(state, 0, 0, 0)) {
		fprintf(stderr, "%s\n", lua_tostring(state, -1));
		lua_close(state);
		return 1;
	}
	int failed = luaL_loadstring(state, "error('expected failure')") ||
		lua_pcall(state, 0, 0, 0) != LUA_ERRRUN;
	lua_close(state);
	return failed;
}
