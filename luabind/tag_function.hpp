// Copyright Daniel Wallin 2008. Use, modification and distribution is
// subject to the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

# ifndef LUABIND_TAG_FUNCTION_081129_HPP
#  define LUABIND_TAG_FUNCTION_081129_HPP

namespace luabind {

namespace detail
{

  template <class Signature, class F>
  struct tagged_function
  {
      tagged_function(F f)
        : f(f)
      {}

      F f;
  };

  template <class Signature, class F>
  Signature deduce_signature(tagged_function<Signature, F> const&, ...)
  {
      return Signature();
  }

  template <class Function>
  struct signature_from_function;

  template <class... Args, class F, class Policies>
  int invoke(
      lua_State* L, function_object const& self, invoke_context& ctx
    , tagged_function<vector<Args...>, F> const& tagged
    , vector<Args...>, Policies const& policies)
  {
      return invoke(L, self, ctx, tagged.f, vector<Args...>(), policies);
  }

  template <class R, class... Args>
  struct signature_from_function<R(Args...)>
  {
      typedef vector<R, Args...> type;
  };

} // namespace detail

template <class Signature, class F>
detail::tagged_function<
    typename detail::signature_from_function<Signature>::type
  , F
>
tag_function(F f)
{
    return f;
}

} // namespace luabind

# endif // LUABIND_TAG_FUNCTION_081129_HPP
