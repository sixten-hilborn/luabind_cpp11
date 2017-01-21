#include <iostream>
#include <luabind/luabind.hpp>

void greet()
{
	std::cout << "Hello world from Lua!" << std::endl;
}

int main(int argc, char *argv[])
{
	lua_State* const L = luaL_newstate();
	luabind::open(L);
	luaL_openlibs(L);
	luabind::module(L)
	[
		luabind::def("greet", &greet)
	];
	if (luaL_dofile(L, "greet.lua") != 0)
	{
		std::cerr << "Could not load greet.lua" << std::endl;
		return 1;
	}
	
	return 0;
}