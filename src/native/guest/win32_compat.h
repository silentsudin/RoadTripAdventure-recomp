#pragma once
// Ghidra's gen TUs spell ssize_t; mingw-w64 only declares it in sys/types.h.
#if defined(_WIN32)
#include <sys/types.h>
#endif
