#pragma once

#include "clogr.h"

#define URHI_ENABLE_VALIDATION // turn this off later

#if defined(URHI_ENABLE_VALIDATION)
#   define URHI_VALIDATE(condition, ...)                    \
do {                                                        \
clogr::ensure(condition, __VA_ARGS__);                      \
                                                            \
} while (0)

#   define URHI_WARNING(condition, ...)                     \
do {                                                        \
if(!(condition))                                            \
{                                                           \
clogr::warn(__VA_ARGS__);                                   \
}                                                           \
} while (0)

#else
#define URHI_WARNING(condition, ...) ((void)0)
#define URHI_VALIDATE(condition, ...) ((void)0)
#endif