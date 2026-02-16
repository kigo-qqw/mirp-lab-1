#include "topology.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

Node *FindNodeById(const NodeVector Nodes, const usize Id) {
  for (int i = 0; i < Vector_Size(Nodes); ++i) {
    if (Vector_At(Nodes, i)->Id == Id) {
      return Vector_At(Nodes, i);
    }
  }
  return NULL;
}

const char *NullToString() { return strdup("null"); }

#define TO_STRING_DEFINITION(T, FMT, ...)                                      \
  const char *TO_STRING_IDENTIFIER(T)(const void *const Self) {                \
    const T *const T = Self;                                                  \
    if (!T)                                                                    \
      return NullToString();                                                   \
    char *Buffer = NULL;                                                       \
    usize BufferSize = 0;                                                      \
                                                                               \
    while (true) {                                                             \
      BufferSize = snprintf(Buffer, BufferSize, FMT, ##__VA_ARGS__) + 1;       \
      if (Buffer)                                                              \
        return Buffer;                                                         \
      Buffer = malloc(BufferSize);                                             \
    }                                                                          \
  }

TO_STRING_DEFINITION(Node, "Node{Id=%zu}", Node->Id)
TO_STRING_DEFINITION(Edge, "Edge{Id=%zu, A=%s, B=%s}", Edge->Id,
                     NodeToString(Edge->A), NodeToString(Edge->B))
TO_STRING_DEFINITION(Railway, "Railway{Id=%zu, ConnectedNode=%s}", Railway->Id,
                     NodeToString(Railway->ConnectedNode))
TO_STRING_DEFINITION(RailwaySystemTopology,
                     "RailwaySystemTopology{Nodes=%s, Edges=%s, Railways=%s}",
                     Vector_ToString(RailwaySystemTopology->Nodes,
                                     TO_STRING_IDENTIFIER(Node)),
                     Vector_ToString(RailwaySystemTopology->Edges,
                                     TO_STRING_IDENTIFIER(Edge)),
                     Vector_ToString(RailwaySystemTopology->Railways,
                                     TO_STRING_IDENTIFIER(Railway)))

#define new(T) ((T *)malloc(sizeof(T)))

#define RAII_Free_DEFINITION(T)                                                \
  RAII_Free_DECL(T) {                                                          \
    RAII_Destroy_IDENTIFIER(T)(Self);                                          \
    free(Self);                                                                \
  }

RAII_New_DECL(Node, const usize Id) {
  Node *Self = new(Node);
  RAII_Init_IDENTIFIER(Node)(Self, Id);
  return Self;
}
RAII_Init_DECL(Node, const usize Id) {
  Self->Id = Id;
  Vector_Init(Self->Edges); // TODO
}
RAII_Destroy_DECL(Node) { Vector_Destroy(Self->Edges); }

RAII_New_DECL(Edge, const usize Id, Node *A, Node *B) {
  Edge *Self = new(Edge);
  RAII_Init_IDENTIFIER(Edge)(Self, Id, A, B);
  return Self;
}
RAII_Init_DECL(Edge, const usize Id, Node *A, Node *B) {
  Self->Id = Id;
  Self->A = A;
  Self->B = B;
}
RAII_Destroy_DECL(Edge) {}

RAII_New_DECL(Railway, const usize Id, Node *ConnectedNode) {
  Railway *Self = new(Railway);
  RAII_Init_IDENTIFIER(Railway)(Self, Id, ConnectedNode);
  return Self;
}
RAII_Init_DECL(Railway, const usize Id, Node *ConnectedNode) {
  Self->Id = Id;
  Self->ConnectedNode = ConnectedNode;
}
RAII_Destroy_DECL(Railway) {}

RAII_New_DECL(RailwaySystemTopology) {
  RailwaySystemTopology *Self = new(RailwaySystemTopology);
  RAII_Init_IDENTIFIER(RailwaySystemTopology)(Self);
  return Self;
}
RAII_Init_DECL(RailwaySystemTopology) {}
RAII_Destroy_DECL(RailwaySystemTopology) {
  for (usize i = 0; i < Vector_Size(Self->Nodes); ++i)
    RAII_Free_IDENTIFIER(Node)(Vector_At(Self->Nodes, i));
  Vector_Destroy(Self->Nodes);

  for (usize i = 0; i < Vector_Size(Self->Edges); ++i)
    RAII_Free_IDENTIFIER(Edge)(Vector_At(Self->Edges, i));
  Vector_Destroy(Self->Edges);

  for (usize i = 0; i < Vector_Size(Self->Railways); ++i)
    RAII_Free_IDENTIFIER(Railway)(Vector_At(Self->Railways, i));
  Vector_Destroy(Self->Railways);
}

RAII_Free_DEFINITION(Node);
RAII_Free_DEFINITION(Edge);
RAII_Free_DEFINITION(Railway);
RAII_Free_DEFINITION(RailwaySystemTopology);
