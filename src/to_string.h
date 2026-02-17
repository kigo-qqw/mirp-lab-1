#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define AUTO(FREE_METHOD) __attribute__((cleanup(FREE_METHOD)))

static inline void free_const_string(const char *const *const p) {
  free((void *)*p);
}

#define String const char *AUTO(free_const_string)

#define TO_STRING(T) T##ToString

#define TO_STRING_DECL(T) const char *TO_STRING(T)(const void *)

typedef const char *(*ToString)(const void *);

#define TO_STRING_DEFINITION(T, DEFER_CLEANUP, FMT, ...)                      \
  const char *TO_STRING(T)(const void *const Self) {                           \
    const T *const T = Self;                                                   \
    if (!T)                                                                    \
      return NullToString();                                                   \
    char *Buffer = NULL;                                                       \
    usize BufferSize = 0;                                                      \
                                                                               \
    DEFER_CLEANUP;                                                             \
                                                                               \
    while (true) {                                                             \
      BufferSize = snprintf(Buffer, BufferSize, FMT, ##__VA_ARGS__) + 1;       \
      if (Buffer)                                                              \
        return Buffer;                                                         \
      Buffer = malloc(BufferSize);                                             \
    }                                                                          \
  }