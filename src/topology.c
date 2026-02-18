#include "topology.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "raii_impl.h"
#include "to_string_impl.h"

Node *FindNodeById(const NodeVector Nodes, const usize Id) {
  for (int i = 0; i < Vector_Size(Nodes); ++i) {
    if (Vector_At(Nodes, i)->Id == Id) {
      return Vector_At(Nodes, i);
    }
  }
  return NULL;
}

TO_STRING_DEFINITION(Node, {}, "Node{Id=%zu}", Node->Id)
TO_STRING_DEFINITION(Edge, String NodeA = NodeToString(Edge->A);
                     String NodeB = TO_STRING(Node)(Edge->B),
                     "Edge{Id=%zu, A=%s, B=%s}", Edge->Id, NodeA, NodeB);
TO_STRING_DEFINITION(
    Railway, String ConnectedNode = TO_STRING(Node)(Railway->ConnectedNode),
    "Railway{Id=%zu, ConnectedNode=%s}", Railway->Id, ConnectedNode);
TO_STRING_DEFINITION(
    RailwaySystemTopology,
    String NodesAsString = Vector_ToString(RailwaySystemTopology->Nodes,
                                           TO_STRING(Node));
    String EdgesAsString = Vector_ToString(RailwaySystemTopology->Edges,
                                           TO_STRING(Edge));
    String RailwaysAsString = Vector_ToString(RailwaySystemTopology->Railways,
                                              TO_STRING(Railway)),
    "RailwaySystemTopology{Nodes=%s, Edges=%s, Railways=%s}", NodesAsString,
    EdgesAsString, RailwaysAsString)

RAII_New_DECL(Node, const usize Id) {
  Node *Self = new(Node);
  RAII_Init(Node)(Self, Id);
  return Self;
}
RAII_Init_DECL(Node, const usize Id) {
  Self->Id = Id;
  Vector_Init(Self->Edges); // TODO
}
RAII_Destroy_DECL(Node) { Vector_Destroy(Self->Edges); }

RAII_New_DECL(Edge, const usize Id, Node *A, Node *B) {
  Edge *Self = new(Edge);
  RAII_Init(Edge)(Self, Id, A, B);
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
  RAII_Init(Railway)(Self, Id, ConnectedNode);
  return Self;
}
RAII_Init_DECL(Railway, const usize Id, Node *ConnectedNode) {
  Self->Id = Id;
  Self->ConnectedNode = ConnectedNode;
}
RAII_Destroy_DECL(Railway) {}

RAII_New_DECL(RailwaySystemTopology) {
  RailwaySystemTopology *Self = new(RailwaySystemTopology);
  RAII_Init(RailwaySystemTopology)(Self);
  return Self;
}
RAII_Init_DECL(RailwaySystemTopology) {}
RAII_Destroy_DECL(RailwaySystemTopology) {
  for (usize i = 0; i < Vector_Size(Self->Nodes); ++i)
    RAII_Free(Node)(Vector_At(Self->Nodes, i));
  Vector_Destroy(Self->Nodes);

  for (usize i = 0; i < Vector_Size(Self->Edges); ++i)
    RAII_Free(Edge)(Vector_At(Self->Edges, i));
  Vector_Destroy(Self->Edges);

  for (usize i = 0; i < Vector_Size(Self->Railways); ++i)
    RAII_Free(Railway)(Vector_At(Self->Railways, i));
  Vector_Destroy(Self->Railways);
}

RAII_Free_DEFINITION(Node);
RAII_Free_DEFINITION(Edge);
RAII_Free_DEFINITION(Railway);
RAII_Free_DEFINITION(RailwaySystemTopology);
