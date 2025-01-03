#include "config.h"
#ifdef USE_FOREGROUND
.zero

_ptrReadForegroundLeft .dsb 2
_ptrReadForegroundRight .dsb 2

.text

#endif
