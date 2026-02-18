#pragma once

#include <stdlib.h>

#include "raii.h"

#define RAII_Free_DEFINITION(T)                                                \
  RAII_Free_DECL(T) {                                                          \
    RAII_Destroy(T)(Self);                                                     \
    free(Self);                                                                \
  }
