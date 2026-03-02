#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "types.h"

#define AUTO(FREE_METHOD) __attribute__((cleanup(FREE_METHOD)))

void FreeConstString(const c8 *const *p);

#define String const c8 *AUTO(FreeConstString)

#define TO_STRING(T) T##ToString

#define TO_STRING_DECL(T) const c8 *TO_STRING(T)(const void *)

typedef const c8 *(*ToString)(const void *);

const c8 *NullToString(void);
