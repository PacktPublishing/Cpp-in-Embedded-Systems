#ifndef UNITTEST_CHECKMACROS_H
#define UNITTEST_CHECKMACROS_H

#include "HelperMacros.h"
#include "ExceptionMacros.h"
#include "Checks.h"
#include "AssertException.h"
#include "RequiredCheckException.h"
#include "MemoryOutStream.h"
#include "TestDetails.h"
#include "CurrentTest.h"
#include "ReportAssertImpl.h"

#define UNITTEST_CHECK(value)                                      \
   UNITTEST_MULTILINE_MACRO_BEGIN                                                                                                     \
   UNITTEST_IMPL_TRY                                                                                                                             \
   ({                                                                                                                                 \
      if (!UnitTest::Check(value))                                                                                                    \
         UnitTest::CurrentTest::Results()->OnTestFailure(UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __LINE__), #value); \
   })                                                                                                                                 \
   UNITTEST_IMPL_RETHROW (UnitTest::RequiredCheckException)                                                                                                               \
   UNITTEST_IMPL_CATCH (std::exception, exc,                                                                                                       \
   {                                                                                                                                  \
      UnitTest::MemoryOutStream UnitTest_message;                                                                                     \
      UnitTest_message << "Unhandled exception (" << exc.what() << ") in CHECK(" #value ")";                                            \
      UnitTest::CurrentTest::Results()->OnTestFailure(UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __LINE__),             \
                                                      UnitTest_message.GetText());                                                    \
   })                                                                                                                                 \
   UNITTEST_IMPL_CATCH_ALL                                                                                                                       \
      ({                                                                                                                              \
      UnitTest::CurrentTest::Results()->OnTestFailure(UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __LINE__),             \
                                                      "Unhandled exception in CHECK(" #value ")");                                    \
   })                                                                                                                                 \
   UNITTEST_MULTILINE_MACRO_END

#define UNITTEST_CHECK_FALSE(value)                                      \
   UNITTEST_MULTILINE_MACRO_BEGIN                                                                                                     \
   UNITTEST_IMPL_TRY                                                                                                                             \
   ({                                                                                                                                 \
      if (!UnitTest::CheckFalse(value))                                                                                                    \
         UnitTest::CurrentTest::Results()->OnTestFailure(UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __LINE__), #value); \
   })                                                                                                                                 \
   UNITTEST_IMPL_RETHROW (UnitTest::RequiredCheckException)                                                                                                               \
   UNITTEST_IMPL_CATCH (std::exception, exc,                                                                                                       \
   {                                                                                                                                  \
      UnitTest::MemoryOutStream UnitTest_message;                                                                                     \
      UnitTest_message << "Unhandled exception (" << exc.what() << ") in CHECK(" #value ")";                                            \
      UnitTest::CurrentTest::Results()->OnTestFailure(UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __LINE__),             \
                                                      UnitTest_message.GetText());                                                    \
   })                                                                                                                                 \
   UNITTEST_IMPL_CATCH_ALL                                                                                                                       \
      ({                                                                                                                              \
      UnitTest::CurrentTest::Results()->OnTestFailure(UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __LINE__),             \
                                                      "Unhandled exception in CHECK(" #value ")");                                    \
   })                                                                                                                                 \
   UNITTEST_MULTILINE_MACRO_END

#define UNITTEST_CHECK_EQUAL(expected, actual)                                                                                                                \
   UNITTEST_MULTILINE_MACRO_BEGIN                                                                                                                    \
   UNITTEST_IMPL_TRY                                                                                                                                            \
   ({                                                                                                                                                \
      UnitTest::CheckEqual(*UnitTest::CurrentTest::Results(), expected, actual, UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __LINE__)); \
   })                                                                                                                                                \
   UNITTEST_IMPL_RETHROW (UnitTest::RequiredCheckException)                                                                                                               \
   UNITTEST_IMPL_CATCH (std::exception, exc,                                                                                                                      \
   {                                                                                                                                                 \
      UnitTest::MemoryOutStream UnitTest_message;                                                                                                    \
      UnitTest_message << "Unhandled exception (" << exc.what() << ") in CHECK_EQUAL(" #expected ", " #actual ")";                                     \
      UnitTest::CurrentTest::Results()->OnTestFailure(UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __LINE__),                            \
                                                      UnitTest_message.GetText());                                                                   \
   })                                                                                                                                                \
   UNITTEST_IMPL_CATCH_ALL                                                                                                                                      \
   ({                                                                                                                                                \
      UnitTest::CurrentTest::Results()->OnTestFailure(UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __LINE__),                            \
                                                      "Unhandled exception in CHECK_EQUAL(" #expected ", " #actual ")");                             \
   })                                                                                                                                                \
   UNITTEST_MULTILINE_MACRO_END

#define UNITTEST_CHECK_EQUAL_HEX(expected, actual)                                                                                                                \
   UNITTEST_MULTILINE_MACRO_BEGIN                                                                                                                    \
   UNITTEST_IMPL_TRY                                                                                                                                            \
   ({                                                                                                                                                \
      UnitTest::CheckEqualHex(*UnitTest::CurrentTest::Results(), expected, actual, UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __LINE__)); \
   })                                                                                                                                                \
   UNITTEST_IMPL_RETHROW (UnitTest::RequiredCheckException)                                                                                                               \
   UNITTEST_IMPL_CATCH (std::exception, exc,                                                                                                                      \
   {                                                                                                                                                 \
      UnitTest::MemoryOutStream UnitTest_message;                                                                                                    \
      UnitTest_message << "Unhandled exception (" << exc.what() << ") in CHECK_EQUAL(" #expected ", " #actual ")";                                     \
      UnitTest::CurrentTest::Results()->OnTestFailure(UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __LINE__),                            \
                                                      UnitTest_message.GetText());                                                                   \
   })                                                                                                                                                \
   UNITTEST_IMPL_CATCH_ALL                                                                                                                                      \
   ({                                                                                                                                                \
      UnitTest::CurrentTest::Results()->OnTestFailure(UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __LINE__),                            \
                                                      "Unhandled exception in CHECK_EQUAL(" #expected ", " #actual ")");                             \
   })                                                                                                                                                \
   UNITTEST_MULTILINE_MACRO_END

#define UNITTEST_CHECK_NOT_EQUAL(expected, actual)                                                                                                                \
   UNITTEST_MULTILINE_MACRO_BEGIN                                                                                                                    \
   UNITTEST_IMPL_TRY                                                                                                                                            \
   ({                                                                                                                                                \
      UnitTest::CheckNotEqual(*UnitTest::CurrentTest::Results(), expected, actual, UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __LINE__)); \
   })                                                                                                                                                \
   UNITTEST_IMPL_RETHROW (UnitTest::RequiredCheckException)                                                                                                               \
   UNITTEST_IMPL_CATCH (std::exception, exc,                                                                                                                      \
   {                                                                                                                                                 \
      UnitTest::MemoryOutStream UnitTest_message;                                                                                                    \
      UnitTest_message << "Unhandled exception (" << exc.what() << ") in CHECK_EQUAL(" #expected ", " #actual ")";                                     \
      UnitTest::CurrentTest::Results()->OnTestFailure(UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __LINE__),                            \
                                                      UnitTest_message.GetText());                                                                   \
   })                                                                                                                                                \
   UNITTEST_IMPL_CATCH_ALL                                                                                                                                      \
   ({                                                                                                                                                \
      UnitTest::CurrentTest::Results()->OnTestFailure(UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __LINE__),                            \
                                                      "Unhandled exception in CHECK_EQUAL(" #expected ", " #actual ")");                             \
   })                                                                                                                                                \
   UNITTEST_MULTILINE_MACRO_END

#define UNITTEST_CHECK_FALSE_EQUAL_HEX(expected, actual)                                                                                                                \
   UNITTEST_MULTILINE_MACRO_BEGIN                                                                                                                    \
   UNITTEST_IMPL_TRY                                                                                                                                            \
   ({                                                                                                                                                \
      UnitTest::CheckNotEqualHex(*UnitTest::CurrentTest::Results(), expected, actual, UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __LINE__)); \
   })                                                                                                                                                \
   UNITTEST_IMPL_RETHROW (UnitTest::RequiredCheckException)                                                                                                               \
   UNITTEST_IMPL_CATCH (std::exception, exc,                                                                                                                      \
   {                                                                                                                                                 \
      UnitTest::MemoryOutStream UnitTest_message;                                                                                                    \
      UnitTest_message << "Unhandled exception (" << exc.what() << ") in CHECK_EQUAL(" #expected ", " #actual ")";                                     \
      UnitTest::CurrentTest::Results()->OnTestFailure(UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __LINE__),                            \
                                                      UnitTest_message.GetText());                                                                   \
   })                                                                                                                                                \
   UNITTEST_IMPL_CATCH_ALL                                                                                                                                      \
   ({                                                                                                                                                \
      UnitTest::CurrentTest::Results()->OnTestFailure(UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __LINE__),                            \
                                                      "Unhandled exception in CHECK_EQUAL(" #expected ", " #actual ")");                             \
   })                                                                                                                                                \
   UNITTEST_MULTILINE_MACRO_END

#define UNITTEST_CHECK_CLOSE(expected, actual, tolerance)                                                                                                                \
   UNITTEST_MULTILINE_MACRO_BEGIN                                                                                                                               \
   UNITTEST_IMPL_TRY                                                                                                                                                       \
   ({                                                                                                                                                           \
      UnitTest::CheckClose(*UnitTest::CurrentTest::Results(), expected, actual, tolerance, UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __LINE__)); \
   })                                                                                                                                                           \
   UNITTEST_IMPL_RETHROW (UnitTest::RequiredCheckException)                                                                                                               \
   UNITTEST_IMPL_CATCH (std::exception, exc,                                                                                                                                 \
   {                                                                                                                                                            \
      UnitTest::MemoryOutStream UnitTest_message;                                                                                                               \
      UnitTest_message << "Unhandled exception (" << exc.what() << ") in CHECK_CLOSE(" #expected ", " #actual ")";                                                \
      UnitTest::CurrentTest::Results()->OnTestFailure(UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __LINE__),                                       \
                                                      UnitTest_message.GetText());                                                                              \
   })                                                                                                                                                           \
   UNITTEST_IMPL_CATCH_ALL                                                                                                                                                 \
   ({                                                                                                                                                           \
      UnitTest::CurrentTest::Results()->OnTestFailure(UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __LINE__),                                       \
                                                      "Unhandled exception in CHECK_CLOSE(" #expected ", " #actual ")");                                        \
   })                                                                                                                                                           \
   UNITTEST_MULTILINE_MACRO_END

#define UNITTEST_CHECK_FLOAT_SAME(expected, actual)                                                                                                                \
   UNITTEST_MULTILINE_MACRO_BEGIN                                                                                                                               \
   UNITTEST_IMPL_TRY                                                                                                                                                       \
   ({                                                                                                                                                           \
      UnitTest::CheckClose(*UnitTest::CurrentTest::Results(), expected, actual, 0, UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __LINE__)); \
   })                                                                                                                                                           \
   UNITTEST_IMPL_RETHROW (UnitTest::RequiredCheckException)                                                                                                               \
   UNITTEST_IMPL_CATCH (std::exception, exc,                                                                                                                                 \
   {                                                                                                                                                            \
      UnitTest::MemoryOutStream UnitTest_message;                                                                                                               \
      UnitTest_message << "Unhandled exception (" << exc.what() << ") in CHECK_FLOAT_SAME(" #expected ", " #actual ")";                                                \
      UnitTest::CurrentTest::Results()->OnTestFailure(UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __LINE__),                                       \
                                                      UnitTest_message.GetText());                                                                              \
   })                                                                                                                                                           \
   UNITTEST_IMPL_CATCH_ALL                                                                                                                                                 \
   ({                                                                                                                                                           \
      UnitTest::CurrentTest::Results()->OnTestFailure(UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __LINE__),                                       \
                                                      "Unhandled exception in CHECK_FLOAT_SAME(" #expected ", " #actual ")");                                        \
   })                                                                                                                                                           \
   UNITTEST_MULTILINE_MACRO_END

#define UNITTEST_CHECK_FLOAT_DIFFERENT(expected, actual)                                                                                                                \
   UNITTEST_MULTILINE_MACRO_BEGIN                                                                                                                               \
   UNITTEST_IMPL_TRY                                                                                                                                                       \
   ({                                                                                                                                                           \
      UnitTest::CheckNotClose(*UnitTest::CurrentTest::Results(), expected, actual, 0, UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __LINE__)); \
   })                                                                                                                                                           \
   UNITTEST_IMPL_RETHROW (UnitTest::RequiredCheckException)                                                                                                               \
   UNITTEST_IMPL_CATCH (std::exception, exc,                                                                                                                                 \
   {                                                                                                                                                            \
      UnitTest::MemoryOutStream UnitTest_message;                                                                                                               \
      UnitTest_message << "Unhandled exception (" << exc.what() << ") in CHECK_FLOAT_DIFFERENT(" #expected ", " #actual ")";                                                \
      UnitTest::CurrentTest::Results()->OnTestFailure(UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __LINE__),                                       \
                                                      UnitTest_message.GetText());                                                                              \
   })                                                                                                                                                           \
   UNITTEST_IMPL_CATCH_ALL                                                                                                                                                 \
   ({                                                                                                                                                           \
      UnitTest::CurrentTest::Results()->OnTestFailure(UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __LINE__),                                       \
                                                      "Unhandled exception in CHECK_FLOAT_DIFFERENT(" #expected ", " #actual ")");                                        \
   })                                                                                                                                                           \
   UNITTEST_MULTILINE_MACRO_END

#define UNITTEST_CHECK_ARRAY_EQUAL(expected, actual, count)                                                                                                               \
   UNITTEST_MULTILINE_MACRO_BEGIN                                                                                                                                \
   UNITTEST_IMPL_TRY                                                                                                                                                        \
      ({                                                                                                                                                         \
      UnitTest::CheckArrayEqual(*UnitTest::CurrentTest::Results(), expected, actual, count, UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __LINE__)); \
   })                                                                                                                                                            \
   UNITTEST_IMPL_RETHROW (UnitTest::RequiredCheckException)                                                                                                               \
   UNITTEST_IMPL_CATCH (std::exception, exc,                                                                                                                                  \
   {                                                                                                                                                             \
      UnitTest::MemoryOutStream UnitTest_message;                                                                                                                \
      UnitTest_message << "Unhandled exception (" << exc.what() << ") in CHECK_ARRAY_EQUAL(" #expected ", " #actual ")";                                           \
      UnitTest::CurrentTest::Results()->OnTestFailure(UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __LINE__),                                        \
                                                      UnitTest_message.GetText());                                                                               \
   })                                                                                                                                                            \
   UNITTEST_IMPL_CATCH_ALL                                                                                                                                                  \
      ({                                                                                                                                                         \
      UnitTest::CurrentTest::Results()->OnTestFailure(UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __LINE__),                                        \
                                                      "Unhandled exception in CHECK_ARRAY_EQUAL(" #expected ", " #actual ")");                                   \
   })                                                                                                                                                            \
   UNITTEST_MULTILINE_MACRO_END

#define UNITTEST_CHECK_ARRAY_CLOSE(expected, actual, count, tolerance)                                                                                                               \
   UNITTEST_MULTILINE_MACRO_BEGIN                                                                                                                                           \
   UNITTEST_IMPL_TRY                                                                                                                                                                   \
      ({                                                                                                                                                                    \
      UnitTest::CheckArrayClose(*UnitTest::CurrentTest::Results(), expected, actual, count, tolerance, UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __LINE__)); \
   })                                                                                                                                                                       \
   UNITTEST_IMPL_RETHROW (UnitTest::RequiredCheckException)                                                                                                               \
   UNITTEST_IMPL_CATCH (std::exception, exc,                                                                                                                                             \
   {                                                                                                                                                                        \
      UnitTest::MemoryOutStream UnitTest_message;                                                                                                                           \
      UnitTest_message << "Unhandled exception (" << exc.what() << ") in CHECK_ARRAY_CLOSE(" #expected ", " #actual ")";                                                      \
      UnitTest::CurrentTest::Results()->OnTestFailure(UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __LINE__),                                                   \
                                                      UnitTest_message.GetText());                                                                                          \
   })                                                                                                                                                                       \
   UNITTEST_IMPL_CATCH_ALL                                                                                                                                                             \
      ({                                                                                                                                                                    \
      UnitTest::CurrentTest::Results()->OnTestFailure(UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __LINE__),                                                   \
                                                      "Unhandled exception in CHECK_ARRAY_CLOSE(" #expected ", " #actual ")");                                              \
   })                                                                                                                                                                       \
   UNITTEST_MULTILINE_MACRO_END

#define UNITTEST_CHECK_ARRAY2D_CLOSE(expected, actual, rows, columns, tolerance)                                                                                                               \
   UNITTEST_MULTILINE_MACRO_BEGIN                                                                                                                                                     \
   UNITTEST_IMPL_TRY                                                                                                                                                                             \
      ({                                                                                                                                                                              \
      UnitTest::CheckArray2DClose(*UnitTest::CurrentTest::Results(), expected, actual, rows, columns, tolerance, UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __LINE__)); \
   })                                                                                                                                                                                 \
   UNITTEST_IMPL_RETHROW (UnitTest::RequiredCheckException)                                                                                                               \
   UNITTEST_IMPL_CATCH (std::exception, exc,                                                                                                                                                       \
   {                                                                                                                                                                                  \
      UnitTest::MemoryOutStream UnitTest_message;                                                                                                                                     \
      UnitTest_message << "Unhandled exception (" << exc.what() << ") in CHECK_ARRAY2D_CLOSE(" #expected ", " #actual ")";                                                              \
      UnitTest::CurrentTest::Results()->OnTestFailure(UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __LINE__),                                                             \
                                                      UnitTest_message.GetText());                                                                                                    \
   })                                                                                                                                                                                 \
   UNITTEST_IMPL_CATCH_ALL                                                                                                                                                                       \
      ({                                                                                                                                                                              \
      UnitTest::CurrentTest::Results()->OnTestFailure(UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __LINE__),                                                             \
                                                      "Unhandled exception in CHECK_ARRAY2D_CLOSE(" #expected ", " #actual ")");                                                      \
   })                                                                                                                                                                                 \
   UNITTEST_MULTILINE_MACRO_END

#ifndef UNITTEST_DISABLE_SHORT_MACROS
   #ifdef CHECK
      #error CHECK already defined, re-configure with UNITTEST_ENABLE_SHORT_MACROS set to 0 and use UNITTEST_CHECK instead
   #else
      #define CHECK(value) UNITTEST_CHECK((value))
   #endif

   #ifdef CHECK_TRUE
      #error CHECK_TRUE already defined, re-configure with UNITTEST_ENABLE_SHORT_MACROS set to 0 and use UNITTEST_CHECK_TRUE instead
   #else
      #define CHECK_TRUE(value) UNITTEST_CHECK((value))
   #endif

   #ifdef CHECK_FALSE
      #error CHECK_FALSE already defined, re-configure with UNITTEST_ENABLE_SHORT_MACROS set to 0 and use UNITTEST_CHECK_FALSE instead
   #else
      #define CHECK_FALSE(value) UNITTEST_CHECK_FALSE((value))
   #endif

   #ifdef CHECK_EQUAL
      #error CHECK_EQUAL already defined, re-configure with UNITTEST_ENABLE_SHORT_MACROS set to 0 and use UNITTEST_CHECK_EQUAL instead
   #else
      #define CHECK_EQUAL(expected, actual) UNITTEST_CHECK_EQUAL((expected), (actual))
   #endif

   #ifdef CHECK_EQUAL_HEX
      #error CHECK_EQUAL_HEX already defined, re-configure with UNITTEST_ENABLE_SHORT_MACROS set to 0 and use UNITTEST_CHECK_EQUAL_HEX instead
   #else
      #define CHECK_EQUAL_HEX(expected, actual) UNITTEST_CHECK_EQUAL_HEX((expected), (actual))
   #endif

   #ifdef CHECK_NOT_EQUAL
      #error CHECK_NOT_EQUAL already defined, re-configure with UNITTEST_ENABLE_SHORT_MACROS set to 0 and use UNITTEST_CHECK_NOT_EQUAL instead
   #else
      #define CHECK_NOT_EQUAL(expected, actual) UNITTEST_CHECK_NOT_EQUAL((expected), (actual))
   #endif

   #ifdef CHECK_NOT_EQUAL_HEX
      #error CHECK_NOT_EQUAL_HEX already defined, re-configure with UNITTEST_ENABLE_SHORT_MACROS set to 0 and use UNITTEST_CHECK_NOT_EQUAL_HEX instead
   #else
      #define CHECK_NOT_EQUAL_HEX(expected, actual) UNITTEST_CHECK_NOT_EQUAL_HEX((expected), (actual))
   #endif

   #ifdef CHECK_CLOSE
      #error CHECK_CLOSE already defined, re-configure with UNITTEST_ENABLE_SHORT_MACROS set to 0 and use UNITTEST_CHECK_CLOSE instead
   #else
      #define CHECK_CLOSE(expected, actual, tolerance) UNITTEST_CHECK_CLOSE((expected), (actual), (tolerance))
   #endif

  #ifdef CHECK_FLOAT_SAME
      #error CHECK_FLOAT_SAME already defined, re-configure with UNITTEST_ENABLE_SHORT_MACROS set to 0 and use UNITTEST_CHECK_FLOAT_SAME instead
  #else
      #define CHECK_FLOAT_SAME(expected, actual) UNITTEST_CHECK_FLOAT_SAME((expected), (actual))
  #endif

  #ifdef CHECK_FLOAT_DIFFERENT
      #error CHECK_FLOAT_DIFFERENT already defined, re-configure with UNITTEST_ENABLE_SHORT_MACROS set to 0 and use UNITTEST_CHECK_FLOAT_DIFFERENT instead
  #else
      #define CHECK_FLOAT_DIFFERENT(expected, actual) UNITTEST_CHECK_FLOAT_DIFFERENT((expected), (actual))
  #endif

   #ifdef CHECK_ARRAY_EQUAL
      #error CHECK_ARRAY_EQUAL already defined, re-configure with UNITTEST_ENABLE_SHORT_MACROS set to 0 and use UNITTEST_CHECK_ARRAY_EQUAL instead
   #else
      #define CHECK_ARRAY_EQUAL(expected, actual, count) UNITTEST_CHECK_ARRAY_EQUAL((expected), (actual), (count))
   #endif

   #ifdef CHECK_ARRAY_CLOSE
      #error CHECK_ARRAY_CLOSE already defined, re-configure with UNITTEST_ENABLE_SHORT_MACROS set to 0 and use UNITTEST_CHECK_ARRAY_CLOSE instead
   #else
      #define CHECK_ARRAY_CLOSE(expected, actual, count, tolerance) UNITTEST_CHECK_ARRAY_CLOSE((expected), (actual), (count), (tolerance))
   #endif

   #ifdef CHECK_ARRAY2D_CLOSE
      #error CHECK_ARRAY2D_CLOSE already defined, re-configure with UNITTEST_ENABLE_SHORT_MACROS set to 0 and use UNITTEST_CHECK_ARRAY2D_CLOSE instead
   #else
      #define CHECK_ARRAY2D_CLOSE(expected, actual, rows, columns, tolerance) UNITTEST_CHECK_ARRAY2D_CLOSE((expected), (actual), (rows), (columns), (tolerance))
   #endif
#endif

// CHECK_THROW and CHECK_ASSERT only exist when UNITTEST_NO_EXCEPTIONS isn't defined (see config.h)
#ifndef UNITTEST_NO_EXCEPTIONS

#define UNITTEST_CHECK_THROW(expression, ExpectedExceptionType)                                                                                                                                      \
   UNITTEST_MULTILINE_MACRO_BEGIN                                                                                                                                                           \
   bool caught_ = false;                                                                                                                                                                    \
   try { expression; }                                                                                                                                                                      \
   catch (ExpectedExceptionType const&) { caught_ = true; }                                                                                                                                 \
   catch (...) {}                                                                                                                                                                           \
   if (!caught_)                                                                                                                                                                            \
      UnitTest::CurrentTest::Results()->OnTestFailure(UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __LINE__), "Expected exception: \"" #ExpectedExceptionType "\" not thrown"); \
   UNITTEST_MULTILINE_MACRO_END

#define UNITTEST_CHECK_NO_THROW(expression) \
  UNITTEST_MULTILINE_MACRO_BEGIN \
  bool caught_ = false; \
  try { expression; } \
  catch (...) { caught_ = true; } \
if (caught_) \
  UnitTest::CurrentTest::Results()->OnTestFailure(UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __LINE__), "Unexpected exception thrown"); \
  UNITTEST_MULTILINE_MACRO_END

#define UNITTEST_CHECK_ASSERT(expression)                       \
   UNITTEST_MULTILINE_MACRO_BEGIN                      \
   UnitTest::Detail::ExpectAssert(true);               \
   CHECK_THROW(expression, UnitTest::AssertException); \
   UnitTest::Detail::ExpectAssert(false);              \
   UNITTEST_MULTILINE_MACRO_END
#endif

#ifndef UNITTEST_DISABLE_SHORT_MACROS
   #ifdef CHECK_THROW
      #error CHECK_THROW already defined, re-configure with UNITTEST_ENABLE_SHORT_MACROS set to 0 and use UNITTEST_CHECK_THROW instead
   #else
      #define CHECK_THROW UNITTEST_CHECK_THROW
   #endif

   #ifdef CHECK_NO_THROW
      #error CHECK_NO_THROW already defined, re-configure with UNITTEST_ENABLE_SHORT_MACROS set to 0 and use UNITTEST_CHECK_NO_THROW instead
   #else
      #define CHECK_NO_THROW UNITTEST_CHECK_NO_THROW
   #endif

   #ifdef CHECK_ASSERT
      #error CHECK_ASSERT already defined, re-configure with UNITTEST_ENABLE_SHORT_MACROS set to 0 and use UNITTEST_CHECK_ASSERT instead
   #else
      #define CHECK_ASSERT UNITTEST_CHECK_ASSERT
   #endif
#endif

#define CHECK_MESSAGE(m1)              std::cerr << (m1) << "\n";
#define CHECK_MESSAGE1(m1)             std::cerr << (m1) << "\n";
#define CHECK_MESSAGE2(m1, m2)         std::cerr << (m1) << (m2) << "\n";
#define CHECK_MESSAGE3(m1, m2, m3)     std::cerr << (m1) << (m2) << (m3) << "\n";
#define CHECK_MESSAGE4(m1, m2, m3, m4) std::cerr << (m1) << (m2) << (m3) << (m4) << "\n";

#define CHECK_MESSAGE_IF(b, m1)           { if (b) std::cerr << (m1) << "\n"; }
#define CHECK_MESSAGE1_IF(m1)             { if (b) std::cerr << (m1) << "\n"; }
#define CHECK_MESSAGE2_IF(m1, m2)         { if (b) std::cerr << (m1) << (m2) << "\n"; }
#define CHECK_MESSAGE3_IF(m1, m2, m3)     { if (b) std::cerr << (m1) << (m2) << (m3) << "\n"; }
#define CHECK_MESSAGE4_IF(m1, m2, m3, m4) { if (b) std::cerr << (m1) << (m2) << (m3) << (m4) << "\n"; }
#endif

