#include <cstdio>
#include <cstdint>
#include <cmath>

#include <s2list.h>
#include <s2string.h>
#include <s2file.h>

#if defined(PLATFORM_64)
#  define POINTER_FORMAT "%016llX"
#  define OFFSET_FORMAT "%llX"
#else
#  define POINTER_FORMAT "%08X"
#  define OFFSET_FORMAT "%X"
#endif
