// MyEnum.h
#ifndef MYENUM_H
#define MYENUM_H

#define ENUM_DEF(NAME, ENUM_MACRO) \
enum NAME { \
    ENUM_MACRO(ENUM_ENTRY) \
    NAME##_COUNT \
}; \
inline const char* NAME##Str(NAME value) { \
    static const char* strings[] = { \
        ENUM_MACRO(ENUM_STRING) \
    }; \
    return strings[value]; \
} \

#define ENUM_ENTRY(name, str) name,
#define ENUM_STRING(name, str) str,

#define TRADER_RESULT(XX) \
    XX(TRADER_RESULT_SUCCESS, "success") \
    XX(TRADER_RESULT_FAILURE, "failure") \
    XX(TRADER_RESULT_INVALID_PARAMETER, "invalid parameter") \
    XX(TRADER_RESULT_INVALID_STATE, "invalid state") \
    XX(TRADER_RESULT_NOT_IMPLEMENTED, "not implemented")
ENUM_DEF(TraderResult, TRADER_RESULT)
#undef TRADER_RESULT



#undef ENUM_DEF
#undef ENUM_ENTRY
#undef ENUM_STRING

#endif // MYENUM_H