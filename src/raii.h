#pragma once

#define new(T) ((T *)malloc(sizeof(T)))

#define RAII_New(T) T##_New
#define RAII_Init(T) T##_Init
#define RAII_Destroy(T) T##_Destroy
#define RAII_Free(T) T##_Free

#define RAII_New_DECL(T, ...) T *RAII_New(T)(__VA_ARGS__)
#define RAII_Init_DECL(T, ...) void RAII_Init(T)(T * Self, ##__VA_ARGS__)
#define RAII_Destroy_DECL(T) void RAII_Destroy(T)(T * Self)
#define RAII_Free_DECL(T) void RAII_Free(T)(T * Self)

#define RAII_DECL(T, ...)                                                      \
  RAII_New_DECL(T, ##__VA_ARGS__);                                             \
  RAII_Init_DECL(T, ##__VA_ARGS__);                                            \
  RAII_Destroy_DECL(T);                                                        \
  RAII_Free_DECL(T);
