#pragma once

#include <stddef.h>

#include "vector.h"

typedef struct Node Node;
typedef struct Edge Edge;
typedef struct Railway Railway;
typedef struct RailwaySystemTopology RailwaySystemTopology;

struct Edge {
  usize Id;
  Node *A;
  Node *B;
};

struct Node {
  usize Id;
  Vector(Edge *) Edges;
};

typedef Vector(Node *) NodeVector;

struct Railway {
  usize Id;
  Node *ConnectedNode;
};

struct RailwaySystemTopology {
  NodeVector Nodes;
  Vector(Edge *) Edges;
  Vector(Railway *) Railways;
};

Node *FindNodeById(NodeVector Nodes, usize Id);

#define TO_STRING_IDENTIFIER(T) T##ToString
#define TO_STRING_DECL(T) const char *TO_STRING_IDENTIFIER(T)(const void *)

typedef const char *(*ElementToString)(const void *);

const char *NullToString();

TO_STRING_DECL(Node);
TO_STRING_DECL(Edge);
TO_STRING_DECL(Railway);
TO_STRING_DECL(RailwaySystemTopology);

#define RAII_New_IDENTIFIER(T) T##_New
#define RAII_Init_IDENTIFIER(T) T##_Init
#define RAII_Destroy_IDENTIFIER(T) T##_Destroy
#define RAII_Free_IDENTIFIER(T) T##_Free

#define RAII_New_DECL(T, ...) T *RAII_New_IDENTIFIER(T)(__VA_ARGS__)
#define RAII_Init_DECL(T, ...)                                                 \
  void RAII_Init_IDENTIFIER(T)(T * Self, ##__VA_ARGS__)
#define RAII_Destroy_DECL(T) void RAII_Destroy_IDENTIFIER(T)(T * Self)
#define RAII_Free_DECL(T) void RAII_Free_IDENTIFIER(T)(T * Self)

#define RAII_DECL(T, ...)                                                      \
  RAII_New_DECL(T, ##__VA_ARGS__);                                             \
  RAII_Init_DECL(T, ##__VA_ARGS__);                                            \
  RAII_Destroy_DECL(T);                                                        \
  RAII_Free_DECL(T);

RAII_DECL(Node, usize Id);
RAII_DECL(Edge, usize Id, Node *A, Node *B);
RAII_DECL(Railway, usize Id, Node *ConnectedNode);
RAII_DECL(RailwaySystemTopology);
