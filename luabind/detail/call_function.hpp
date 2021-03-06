// Copyright (c) 2003 Daniel Wallin and Arvid Norberg

// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF
// ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
// TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT
// SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
// ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
// OR OTHER DEALINGS IN THE SOFTWARE.


#ifndef LUABIND_CALL_FUNCTION_HPP_INCLUDED
#define LUABIND_CALL_FUNCTION_HPP_INCLUDED

#include <luabind/config.hpp>

#include <boost/mpl/if.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/mpl/or.hpp>

#include <luabind/error.hpp>
#include <luabind/detail/convert_to_lua.hpp>
#include <luabind/detail/pcall.hpp>

# ifdef BOOST_MSVC
// Disable warning about assert+throw
#  pragma warning(push)
#  pragma warning(disable:4297)
# endif // BOOST_MSVC


namespace luabind
{
	namespace detail
	{

		// if the proxy_function_caller returns non-void
			template<class Ret, class Tuple>
			class proxy_function_caller
			{
//			friend class luabind::object;
			public:

				typedef int(*function_t)(lua_State*, int, int);

				proxy_function_caller(
					lua_State* L
					, int params
					, function_t fun
					, const Tuple args)
					: m_state(L)
					, m_params(params)
					, m_fun(fun)
					, m_args(args)
					, m_called(false)
				{
				}

				proxy_function_caller(const proxy_function_caller& rhs)
					: m_state(rhs.m_state)
					, m_params(rhs.m_params)
					, m_fun(rhs.m_fun)
					, m_args(rhs.m_args)
					, m_called(rhs.m_called)
				{
					rhs.m_called = true;
				}

				~proxy_function_caller()
				{
					if (m_called) return;

					m_called = true;
					lua_State* L = m_state;

					int top = lua_gettop(L);

					push_args_from_tuple<1>::apply(L, m_args);
					if (m_fun(L, std::tuple_size<Tuple>::value, 0))
					{
						assert(lua_gettop(L) == top - m_params + 1);
#ifndef LUABIND_NO_EXCEPTIONS
						throw luabind::error(L);
#else
						error_callback_fun e = get_error_callback();
						if (e) e(L);

						assert(0 && "the lua function threw an error and exceptions are disabled."
									" If you want to handle the error you can use luabind::set_error_callback()");
						std::terminate();

#endif
					}

					// pops the return values from the function call
					stack_pop pop(L, lua_gettop(L) - top + m_params);
				}

				operator Ret()
				{
					typename mpl::apply_wrap2<default_policy,Ret,lua_to_cpp>::type converter;

					m_called = true;
					lua_State* L = m_state;

					int top = lua_gettop(L);

					push_args_from_tuple<1>::apply(L, m_args);
					if (m_fun(L, std::tuple_size<Tuple>::value, 1))
					{
						assert(lua_gettop(L) == top - m_params + 1);
#ifndef LUABIND_NO_EXCEPTIONS
						throw luabind::error(L);
#else
						error_callback_fun e = get_error_callback();
						if (e) e(L);

						assert(0 && "the lua function threw an error and exceptions are disabled."
								" If you want to handle the error you can use luabind::set_error_callback()");
						std::terminate();
#endif
					}

					// pops the return values from the function call
					stack_pop pop(L, lua_gettop(L) - top + m_params);

#ifndef LUABIND_NO_ERROR_CHECKING

					if (converter.match(L, LUABIND_DECORATE_TYPE(Ret), -1) < 0)
					{
#ifndef LUABIND_NO_EXCEPTIONS
						throw cast_failed(L, typeid(Ret));
#else
						cast_failed_callback_fun e = get_cast_failed_callback();
						if (e) e(L, typeid(Ret));

						assert(0 && "the lua function's return value could not be converted."
									" If you want to handle the error you can use luabind::set_error_callback()");
						std::terminate();

#endif
					}
#endif
					return converter.apply(L, LUABIND_DECORATE_TYPE(Ret), -1);
				}

				template<class Policies>
				Ret operator[](const Policies& p)
				{
					typedef typename detail::find_conversion_policy<0, Policies>::type converter_policy;
					typename mpl::apply_wrap2<converter_policy,Ret,lua_to_cpp>::type converter;

					m_called = true;
					lua_State* L = m_state;

					int top = lua_gettop(L);

					detail::push_args_from_tuple<1>::apply(L, m_args, p);
					if (m_fun(L, std::tuple_size<Tuple>::value, 1))
					{
						assert(lua_gettop(L) == top - m_params + 1);
#ifndef LUABIND_NO_EXCEPTIONS
						throw error(L);
#else
						error_callback_fun e = get_error_callback();
						if (e) e(L);

						assert(0 && "the lua function threw an error and exceptions are disabled."
								" If you want to handle the error you can use luabind::set_error_callback()");
						std::terminate();
#endif
					}

					// pops the return values from the function call
					stack_pop pop(L, lua_gettop(L) - top + m_params);

#ifndef LUABIND_NO_ERROR_CHECKING

					if (converter.match(L, LUABIND_DECORATE_TYPE(Ret), -1) < 0)
					{
#ifndef LUABIND_NO_EXCEPTIONS
						throw cast_failed(L, typeid(Ret));
#else
						cast_failed_callback_fun e = get_cast_failed_callback();
						if (e) e(L, typeid(Ret));

						assert(0 && "the lua function's return value could not be converted."
									" If you want to handle the error you can use luabind::set_error_callback()");
						std::terminate();

#endif
					}
#endif
					return converter.apply(L, LUABIND_DECORATE_TYPE(Ret), -1);
				}

			private:

				lua_State* m_state;
				int m_params;
				function_t m_fun;
				Tuple m_args;
				mutable bool m_called;

			};

		// if the proxy_member_caller returns void
			template<class Tuple>
			class proxy_function_void_caller
			{
			friend class luabind::object;
			public:

				typedef int(*function_t)(lua_State*, int, int);

				proxy_function_void_caller(
					lua_State* L
					, int params
					, function_t fun
					, const Tuple args)
					: m_state(L)
					, m_params(params)
					, m_fun(fun)
					, m_args(args)
					, m_called(false)
				{
				}

				proxy_function_void_caller(const proxy_function_void_caller& rhs)
					: m_state(rhs.m_state)
					, m_params(rhs.m_params)
					, m_fun(rhs.m_fun)
					, m_args(rhs.m_args)
					, m_called(rhs.m_called)
				{
					rhs.m_called = true;
				}

				~proxy_function_void_caller()
				{
					if (m_called) return;

					m_called = true;
					lua_State* L = m_state;

					int top = lua_gettop(L);

					push_args_from_tuple<1>::apply(L, m_args);
					if (m_fun(L, std::tuple_size<Tuple>::value, 0))
					{
						assert(lua_gettop(L) == top - m_params + 1);
#ifndef LUABIND_NO_EXCEPTIONS
						throw luabind::error(L);
#else
						error_callback_fun e = get_error_callback();
						if (e) e(L);

						assert(0 && "the lua function threw an error and exceptions are disabled."
								" If you want to handle the error you can use luabind::set_error_callback()");
						std::terminate();
#endif
					}
					// pops the return values from the function call
					stack_pop pop(L, lua_gettop(L) - top + m_params);
				}

				template<class Policies>
				void operator[](const Policies& p)
				{
					m_called = true;
					lua_State* L = m_state;

					int top = lua_gettop(L);

					detail::push_args_from_tuple<1>::apply(L, m_args, p);
					if (m_fun(L, std::tuple_size<Tuple>::value, 0))
					{
						assert(lua_gettop(L) == top - m_params + 1);
#ifndef LUABIND_NO_EXCEPTIONS
						throw error(L);
#else
						error_callback_fun e = get_error_callback();
						if (e) e(L);

						assert(0 && "the lua function threw an error and exceptions are disabled."
							" If you want to handle the error you can use luabind::set_error_callback()");
						std::terminate();
#endif
					}
					// pops the return values from the function call
					stack_pop pop(L, lua_gettop(L) - top + m_params);
				}

			private:

				lua_State* m_state;
				int m_params;
				function_t m_fun;
				Tuple m_args;
				mutable bool m_called;

			};

	}

	template<class Ret, class... Args>
	using proxy_type = typename boost::mpl::if_<boost::is_void<Ret>
			, luabind::detail::proxy_function_void_caller<std::tuple<Args...>>
			, luabind::detail::proxy_function_caller<Ret, std::tuple<Args...>> >::type;

	template<class Ret, class... Args>
	auto call_function(lua_State* L, const char* name, Args&&... a)
	{
		assert(name && "luabind::call_function() expects a function name");
		using tuple_t = std::tuple<Args...>;
		tuple_t args{ std::forward<Args>(a)... };

		lua_pushstring(L, name);
		lua_gettable(L, LUA_GLOBALSINDEX);

		return proxy_type<Ret, Args...>(L, 1, &detail::pcall, args);
	}

	template<class Ret, class... Args>
	auto call_function(luabind::object const& obj, Args&&... a)
	{
		using tuple_t = std::tuple<Args...>;
		tuple_t args{ std::forward<Args>(a)... };

		obj.push(obj.interpreter());
		return proxy_type<Ret, Args...>(obj.interpreter(), 1, &detail::pcall, args);
	}

	template<class Ret, class... Args>
	auto resume_function(lua_State* L, const char* name, Args&&... a)
	{
		assert(name && "luabind::resume_function() expects a function name");
		typedef std::tuple<Args...> tuple_t;
		tuple_t args{ std::forward<Args>(a)... };

		lua_pushstring(L, name);
		lua_gettable(L, LUA_GLOBALSINDEX);

		return proxy_type<Ret, Args...>(L, 1, &detail::resume_impl, args);
	}

	template<class Ret, class... Args>
	auto resume_function(luabind::object const& obj, Args&&... a)
	{
		typedef std::tuple<Args...> tuple_t;
		tuple_t args{ std::forward<Args>(a)... };

		obj.push(obj.interpreter());
		return proxy_type<Ret, Args...>(obj.interpreter(), 1, &detail::resume_impl, args);
	}

	template<class Ret, class... Args>
	auto resume(lua_State* L, Args&&... a)
	{
		typedef std::tuple<Args...> tuple_t;
		tuple_t args{ std::forward<Args>(a)... };

		return proxy_type<Ret, Args...>(L, 0, &detail::resume_impl, args);
	}

}

# ifdef BOOST_MSVC
#  pragma warning(pop)
# endif // BOOST_MSVC

#endif // LUABIND_CALL_FUNCTION_HPP_INCLUDED
