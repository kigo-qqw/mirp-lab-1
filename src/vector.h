#pragma once

#include <stdlib.h>

#include "types.hpp"

#define VEC_INIT_CAPACITY 64

#define VECTOR_MAX_OF_2(A, B) (((A) < (B)) ? (B) : (A))

#define Vector(T)                                                              \
  struct {                                                                     \
    usize Size;                                                                \
    usize Capacity;                                                            \
    T *Data;                                                                   \
  }

#define Vector_InitWithCapacity(V, C)                                          \
  do {                                                                         \
    (V).Size = 0;                                                              \
    (V).Capacity = VECTOR_MAX_OF_2(C, VEC_INIT_CAPACITY);                      \
    (V).Data = calloc((V).Capacity, sizeof(*(V).Data));                        \
  } while (0)

#define Vector_Init(V) Vector_InitWithCapacity((V), VEC_INIT_CAPACITY)

#define Vector_Destroy(V)                                                      \
  do {                                                                         \
    (V).Size = 0;                                                              \
    (V).Capacity = 0;                                                          \
    free((V).Data);                                                            \
    (V).Data = NULL;                                                           \
  } while (0)

#define Vector_Size(V) ((const usize)(V).Size)

#define Vector_Capacity(V) ((const usize)(V).Capacity)

#define Vector_Empty(V) ((V).Size == 0)

#define Vector_At(V, I) ((V).Data[(I)])

#define Vector_Push(V, E)                                                      \
  do {                                                                         \
    if ((V).Size == (V).Capacity) {                                            \
      (V).Capacity = (V).Capacity * 2;                                         \
      (V).Data = realloc((V).Data, sizeof(*(V).Data) * (V).Capacity);          \
    }                                                                          \
    (V).Data[(V).Size++] = (E);                                                \
  } while (0)

char *Vector_ToString_Generic(const void *Data, usize Size, usize ElementSize,
                              const char *(*ToString)(const void *));

#define Vector_ToString(V, ETS)                                                \
  Vector_ToString_Generic((V).Data, (V).Size, sizeof(*(V).Data),               \
                          ((ToString)(ETS)))
