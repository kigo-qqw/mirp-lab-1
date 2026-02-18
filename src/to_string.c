#include "to_string.h"

#include <string.h>

void free_const_string(const char *const *const p) { free((void *)*p); }

const char *NullToString() { return strdup("null"); }
