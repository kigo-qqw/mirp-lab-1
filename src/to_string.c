#include "to_string.h"

#include <string.h>

void FreeConstString(const c8 *const *const p) { free((void *)*p); }

const c8 *NullToString(void) { return strdup("null"); }
