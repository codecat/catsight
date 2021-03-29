#include <cstdio>
#include <cstdint>
#include <cmath>
#include <cassert>

#include <s2list.h>
#include <s2string.h>
#include <s2file.h>
#include <s2func.h>

#if defined(PLATFORM_64)
#  define POINTER_FORMAT "%016llX"
#  define OFFSET_FORMAT "%llX"
#  define MAX_INSTRUCTION_SIZE 16
#else
#  define POINTER_FORMAT "%08X"
#  define OFFSET_FORMAT "%X"
#  define MAX_INSTRUCTION_SIZE 16
#endif
