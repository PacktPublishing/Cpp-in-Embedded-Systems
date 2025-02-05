#pragma once

// NOLINTBEGIN(cppcoreguidelines-macro-usage)

#ifndef CONSTINIT
#ifndef __cpp_constinit
#if defined(__clang__)
#define CONSTINIT [[clang::require_constant_initialization]]
#else
#define CONSTINIT
#endif
#else
#define CONSTINIT constinit
#endif
#endif

#ifndef CONSTEVAL
#ifndef __cpp_consteval
#define CONSTEVAL constexpr
#else
#define CONSTEVAL consteval
#endif
#endif

#ifndef USING_ATTR_NS
#if defined(__clang__)
#define USING_ATTR_NS using clang:
#else
#define USING_ATTR_NS using gnu:
#endif
#endif

#ifndef ALWAYS_INLINE
#define ALWAYS_INLINE inline __attribute__((always_inline))
#endif

#ifndef NEVER_INLINE
#define NEVER_INLINE __attribute__((noinline))
#endif

#ifndef FLATTEN
#define FLATTEN __attribute__((flatten))
#endif

#ifndef MUSTTAIL
#if defined(__clang__)
#define MUSTTAIL [[clang::musttail]]
#else
#define MUSTTAIL
#endif
#endif

#ifndef LIFETIMEBOUND
#if defined(__clang__)
#define LIFETIMEBOUND [[clang::lifetimebound]]
#else
#define LIFETIMEBOUND
#endif
#endif

#define STDX_DO_PRAGMA(X) _Pragma(#X)
#ifdef __clang__
#define STDX_PRAGMA(X) STDX_DO_PRAGMA(clang X)
#else
#define STDX_PRAGMA(X) STDX_DO_PRAGMA(GCC X)
#endif

// NOLINTEND(cppcoreguidelines-macro-usage)
