#pragma once

#include <stddef.h>

#include "raii.h"
#include "to_string.h"
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

TO_STRING_DECL(Node);
TO_STRING_DECL(Edge);
TO_STRING_DECL(Railway);
TO_STRING_DECL(RailwaySystemTopology);

RAII_DECL(Node, usize Id);
RAII_DECL(Edge, usize Id, Node *A, Node *B);
RAII_DECL(Railway, usize Id, Node *ConnectedNode);
RAII_DECL(RailwaySystemTopology);
