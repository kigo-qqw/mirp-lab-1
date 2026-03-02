#pragma once

#include <stddef.h>

#include "raii.h"
#include "to_string.h"
#include "types.h"
#include "vector.h"

typedef struct Node Node;
typedef struct Edge Edge;
typedef struct Railway Railway;
typedef struct RailwaySystemTopology RailwaySystemTopology;

struct Edge {
  u64 Id;
  Node *A;
  Node *B;
};

struct Node {
  u64 Id;
  Vector(Edge *) Edges;
};

typedef Vector(Node *) NodeVector;

struct Railway {
  u64 Id;
  Node *ConnectedNode;
};

struct RailwaySystemTopology {
  NodeVector Nodes;
  Vector(Edge *) Edges;
  Vector(Railway *) Railways;
};

Node *FindNodeById(NodeVector Nodes, u64 Id);

TO_STRING_DECL(Node);
TO_STRING_DECL(Edge);
TO_STRING_DECL(Railway);
TO_STRING_DECL(RailwaySystemTopology);

RAII_DECL(Node, u64 Id);
RAII_DECL(Edge, u64 Id, Node *A, Node *B);
RAII_DECL(Railway, u64 Id, Node *ConnectedNode);
RAII_DECL(RailwaySystemTopology);

bool RailwaySystemTopology_IsFullyConnected(const RailwaySystemTopology *Self);
