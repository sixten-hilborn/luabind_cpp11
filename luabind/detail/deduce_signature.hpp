// Copyright Daniel Wallin 2008. Use, modification and distribution is
// subject to the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef LUABIND_DEDUCE_SIGNATURE_080911_HPP
#define LUABIND_DEDUCE_SIGNATURE_080911_HPP

#  include <luabind/detail/most_derived.hpp>
#  include <luabind/vector.hpp>

namespace luabind { namespace detail {


template <class R, class... Args>
auto deduce_signature(R(*)(Args...), ...)
{
    return vector<R, Args...>();
}

template <class R, class T, class... Args>
auto deduce_signature(R(T::*)(Args...))
{
    return vector<R, Args...>();
}

template <class R, class T, class Wrapped, class... Args>
auto deduce_signature(R(T::*)(Args...), Wrapped*)
{
    return vector<R, typename most_derived<T,Wrapped>::type&, Args...>();
}

template <class R, class T, class... Args>
auto deduce_signature(R(T::*)(Args...) const)
{
    return vector<R, T const&, Args...>();
}

template <class R, class T, class Wrapped, class... Args>
auto deduce_signature(R(T::*)(Args...) const, Wrapped*)
{
    return vector<R, typename most_derived<T,Wrapped>::type const&, Args...>();
}

// This is primarily intended to catch C++0x lambda closures. It figures out
// the signature of a function object, and strips the object type from the
// resulting signature:
//
//   vector<void, unspecified const&, ...>
//
//     into
//
//   vector<void, ...>
//
// This overload is all luabind needs to correctly handle monomorphic function
// objects with a fixed signature such as C++0x lambdas. The standard doesn't
// explicitly say that an implementation isn't allowed to add additional
// overloads of operator() to the closure type, in practice however, noone
// seems to.

template <class Signature>
struct strip_this_argument;

template <class R, class T, class... Args>
struct strip_this_argument<vector<R, T, Args...> >
{
    typedef vector<R, Args...> type;
};

template <class F>
auto deduce_signature(F const&, ...)
{
    return typename strip_this_argument<
        decltype(deduce_signature(&F::operator()))>::type();
}

}} // namespace luabind::detail

#endif // LUABIND_DEDUCE_SIGNATURE_080911_HPP