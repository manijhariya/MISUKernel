#ifndef WS_ASSERT_
#define WS_ASSERT_

#define WS_DEFINE_FILE \
  static char const WS_THIS_FILE[] = __FILE__;
#define WS_ASSERT(var) ((var) \
  ? (void)0 : WS_onAssert(&WS_THIS_FILE[0],__LINE__))
#define WS_ERRROR() \
  WS_onAssert(&WS_THIS_FILE[0],__LINE__)
#define WS_Require(var) WS_ASSERT(var)

#endif