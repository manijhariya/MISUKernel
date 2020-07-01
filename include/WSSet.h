#ifndef __WS_SET
#define __WS_SET

#include "wisuStd.h"
#include "WSAssert.h"

#if((WS_MAX_Active < 1U) || (32U < WS_MAX_Active))
  #error "WS_MAX_Active is out of range. Valid range is 1U..32U"
#elif (WS_MAX_Active <= 8U)
  typedef uint8_t WSSetBits;
#elif (WS_MAX_Active <= 16U)
    typedef uint16_t WSSetBits;
#elif (WS_MAX_Active <= 32U)
     typedef uint32_t WSSetBits;
#endif
/*bitmask of 32bits long for different sets later*/
typedef struct{
  WSSetBits volatile bits;
}WSSet;

/*empty initilisation of the WSSet*/
#define WSSet_setEmpty(me) ((me)->bits = 0U)

/*checking wheather this is empty set*/
#define WSSet_isEmpty(me) ((me)->bits == 0U)

/*checking wheather this is not empty set*/
#define WSSet_notempty(me) ((me)->bits != 0U)

#define WSSet_hasBits(me,x) (((me)->bits & (WSSetBits)1 << ((x) - 1U)))

#define WSSet_insert(me,x) (((me)->bits |= (WSSetBits)1 << ((x) - 1U)))

#define WSSet_remove(me,x) ((me)->bits &= (WSSetBits)(~((WSSetBits)1 << ((x) - 1U))))

#define WSLog2(x) (32U - __CLZ(x))

#define WSSet_findMax(me,x) ((x) = WSLog2((me)->bits))

#endif