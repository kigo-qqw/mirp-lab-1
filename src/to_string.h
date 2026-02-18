#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define AUTO(FREE_METHOD) __attribute__((cleanup(FREE_METHOD)))

void free_const_string(const char *const *p);

#define String const char *AUTO(free_const_string)

#define TO_STRING(T) T##ToString

#define TO_STRING_DECL(T) const char *TO_STRING(T)(const void *)

typedef const char *(*ToString)(const void *);

const char *NullToString();
