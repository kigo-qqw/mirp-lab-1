#include "parser.h"

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "topology.h"

const char *SkipSpaces(const char *Str) {
  while (*Str == ' ' || *Str == '\t')
    Str++;
  return Str;
}

typedef struct ParseUnsignedIntegerResult {
  unsigned long long Value;
  bool Success;
} ParseUnsignedIntegerResult;

static ParseUnsignedIntegerResult
ParseUnsignedInteger(const char *Str, const char **const StrEnd) {
  Str = SkipSpaces(Str);
  char *EndPtr = NULL;

  ParseUnsignedIntegerResult Result = {strtoull(Str, &EndPtr, 10), true};

  if (errno == ERANGE) {
    errno = 0;
    printf("Failed to parse %.*s", (int)(EndPtr - Str), Str);
    Result.Success = false;
    return Result;
  }

  if (StrEnd) {
    *StrEnd = EndPtr;
  }

  return Result;
}

// Parsing FSM
typedef enum ParseRailwaySystemTopologyStateType {
  ParseRailwaySystemTopologyStateType_Error = -1,
  ParseRailwaySystemTopologyStateType_Complete = 0,
  ParseRailwaySystemTopologyStateType_ReadNodesLabel,
  ParseRailwaySystemTopologyStateType_ReadNode,
  ParseRailwaySystemTopologyStateType_ReadRailwaysLabel,
  ParseRailwaySystemTopologyStateType_ReadRailway,
  ParseRailwaySystemTopologyStateType_ReadEdgesLabel,
  ParseRailwaySystemTopologyStateType_ReadEdge,
  ParseRailwaySystemTopologyStateType_Start =
      ParseRailwaySystemTopologyStateType_ReadNodesLabel
} ParseRailwaySystemTopologyStateType;

#define ParseRailwaySystemTopologyStateData_Read(T)                            \
  struct {                                                                     \
    usize Count;                                                               \
  }

typedef struct ParseRailwaySystemTopologyState {
  ParseRailwaySystemTopologyStateType Type;

  union {
    ParseRailwaySystemTopologyStateData_Read(Node) NodeData;
    ParseRailwaySystemTopologyStateData_Read(Edge) EdgeData;
    ParseRailwaySystemTopologyStateData_Read(Railway) RailwayData;
  };
} ParseRailwaySystemTopologyState;

#define PARSE_UNSIGNED_INTEGER(RESULT_DECL)                                    \
  StrStart = SkipSpaces(StrStart);                                             \
  ParseUnsignedIntegerResult RESULT_DECL =                                     \
      ParseUnsignedInteger(StrStart, &StrEnd);                                 \
  if (!RESULT_DECL.Success) {                                                  \
    State.Type = ParseRailwaySystemTopologyStateType_Error;                    \
    break;                                                                     \
  }                                                                            \
  StrStart = StrEnd

ParseRailwaySystemTopologyResult
ParseRailwaySystemTopology(const char *const TopologyFilename,
                           RailwaySystemTopology *Topology) {
  FILE *F = fopen(TopologyFilename, "r");
  if (!F) {
    fprintf(stderr, "Error opening file %s\n", TopologyFilename);
    exit(EXIT_FAILURE);
  }

  char Line[256];

  ParseRailwaySystemTopologyState State = {
      .Type = ParseRailwaySystemTopologyStateType_Start};
  ParseRailwaySystemTopologyResult Result =
      ParseRailwaySystemTopologyResult_Success;

  while (fgets(Line, sizeof(Line), F)) {
    const char *StrStart = SkipSpaces(Line);
    const char *StrEnd = NULL;

    switch (State.Type) {
    case ParseRailwaySystemTopologyStateType_Error: {
      Result = ParseRailwaySystemTopologyResult_Failure;
      goto Failure;
    }
    case ParseRailwaySystemTopologyStateType_Complete: {
      Result = ParseRailwaySystemTopologyResult_Success;
      goto Success;
    }
    case ParseRailwaySystemTopologyStateType_ReadNodesLabel: {
      static const char NODES[] = "NODES";
      static const size_t NODES_LENGTH = sizeof(NODES) - 1;
      if (strncmp(NODES, StrStart, NODES_LENGTH) == 0) {
        StrStart = StrStart + NODES_LENGTH;
        PARSE_UNSIGNED_INTEGER(R);
        State.NodeData.Count = R.Value;
        Vector_InitWithCapacity(Topology->Nodes, State.NodeData.Count);

        State.Type = ParseRailwaySystemTopologyStateType_ReadNode;
      } else {
        State.Type = ParseRailwaySystemTopologyStateType_Error;
      }
      break;
    }
    case ParseRailwaySystemTopologyStateType_ReadNode: {
      PARSE_UNSIGNED_INTEGER(NodeIdResult);

      Node *N = RAII_New_IDENTIFIER(Node)(NodeIdResult.Value);
      Vector_Push(Topology->Nodes, N);

#ifndef NDEBUG
      puts(TO_STRING_IDENTIFIER(Node)(
          Vector_At(Topology->Nodes, Vector_Size(Topology->Nodes) - 1)));
      puts(Vector_ToString(Topology->Nodes, TO_STRING_IDENTIFIER(Node)));
#endif

      State.Type = Vector_Size(Topology->Nodes) >= State.NodeData.Count
                       ? ParseRailwaySystemTopologyStateType_ReadRailwaysLabel
                       : ParseRailwaySystemTopologyStateType_ReadNode;

      break;
    }
    case ParseRailwaySystemTopologyStateType_ReadRailwaysLabel: {
      static const char ROADS[] = "ROADS";
      static const size_t ROADS_LENGTH = sizeof(ROADS) - 1;
      if (strncmp(ROADS, StrStart, ROADS_LENGTH) == 0) {
        StrStart = StrStart + ROADS_LENGTH;
        PARSE_UNSIGNED_INTEGER(R);
        State.RailwayData.Count = R.Value;
        Vector_InitWithCapacity(Topology->Railways, State.RailwayData.Count);

        State.Type = ParseRailwaySystemTopologyStateType_ReadRailway;
      } else {
        State.Type = ParseRailwaySystemTopologyStateType_Error;
      }
      break;
    }
    case ParseRailwaySystemTopologyStateType_ReadRailway: {
      PARSE_UNSIGNED_INTEGER(RailwayIdResult);
      PARSE_UNSIGNED_INTEGER(RailwayNodeResult);

      Node *ConnectedNode =
          FindNodeById(Topology->Nodes, RailwayNodeResult.Value);
      if (!ConnectedNode) {
        State.Type = ParseRailwaySystemTopologyStateType_Error;
        printf("Error: Node %llu is not defined\n", RailwayNodeResult.Value);
        break;
      }
      Railway *R =
          RAII_New_IDENTIFIER(Railway)(RailwayIdResult.Value, ConnectedNode);
      Vector_Push(Topology->Railways, R);

#ifndef NDEBUG
      puts(TO_STRING_IDENTIFIER(Railway)(
          Vector_At(Topology->Railways, Vector_Size(Topology->Railways) - 1)));
#endif

      State.Type = Vector_Size(Topology->Railways) >= State.RailwayData.Count
                       ? ParseRailwaySystemTopologyStateType_ReadEdgesLabel
                       : ParseRailwaySystemTopologyStateType_ReadRailway;

      break;
    }
    case ParseRailwaySystemTopologyStateType_ReadEdgesLabel: {
      static const char EDGES[] = "EDGES";
      static const size_t EDGES_LENGTH = sizeof(EDGES) - 1;
      if (strncmp(EDGES, StrStart, EDGES_LENGTH) == 0) {
        StrStart = StrStart + EDGES_LENGTH;
        PARSE_UNSIGNED_INTEGER(R);
        State.EdgeData.Count = R.Value;
        Vector_InitWithCapacity(Topology->Edges, State.EdgeData.Count);

        State.Type = ParseRailwaySystemTopologyStateType_ReadEdge;
      } else {
        State.Type = ParseRailwaySystemTopologyStateType_Error;
      }
      break;
    }
    case ParseRailwaySystemTopologyStateType_ReadEdge: {
      PARSE_UNSIGNED_INTEGER(EdgeIdResult);
      PARSE_UNSIGNED_INTEGER(EdgeNodeAResult);
      PARSE_UNSIGNED_INTEGER(EdgeNodeBResult);

      Node *A = FindNodeById(Topology->Nodes, EdgeNodeAResult.Value);
      if (!A) {
        State.Type = ParseRailwaySystemTopologyStateType_Error;
        printf("Error: Node %llu is not defined\n", EdgeNodeAResult.Value);
        break;
      }
      Node *B = FindNodeById(Topology->Nodes, EdgeNodeBResult.Value);
      if (!B) {
        State.Type = ParseRailwaySystemTopologyStateType_Error;
        printf("Error: Node %llu is not defined\n", EdgeNodeBResult.Value);
        break;
      }

      Edge *E = RAII_New_IDENTIFIER(Edge)(EdgeIdResult.Value, A, B);
      Vector_Push(Topology->Edges, E);

#ifndef NDEBUG
      puts(TO_STRING_IDENTIFIER(Edge)(
          Vector_At(Topology->Edges, Vector_Size(Topology->Edges) - 1)));
#endif

      State.Type = Vector_Size(Topology->Edges) >= State.EdgeData.Count
                       ? ParseRailwaySystemTopologyStateType_Complete
                       : ParseRailwaySystemTopologyStateType_ReadEdge;

      break;
    }
    }
  }

  // TODO: set edges in Nodes

  goto Success;

Failure:
  // TODO cleanup allocated memory
Success:
  fclose(F);

  return Result;
}
