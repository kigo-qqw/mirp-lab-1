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

#define PARSE_LABEL_HEADER(HEADER)                                             \
  static const char HEADER[] = #HEADER;                                        \
  static const size_t HEADER##_LENGTH = sizeof(HEADER) - 1;                    \
  StrStart = StrStart + HEADER##_LENGTH;                                       \
  if (strncmp(HEADER, StrStart - HEADER##_LENGTH, HEADER##_LENGTH) == 0)

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
      PARSE_LABEL_HEADER(NODES) {
        PARSE_UNSIGNED_INTEGER(R);
        State.NodeData.Count = R.Value;
        Vector_InitWithCapacity(Topology->Nodes, State.NodeData.Count);

        State.Type = ParseRailwaySystemTopologyStateType_ReadNode;
      }
      else {
        State.Type = ParseRailwaySystemTopologyStateType_Error;
      }
      break;
    }
    case ParseRailwaySystemTopologyStateType_ReadNode: {
      PARSE_UNSIGNED_INTEGER(NodeIdResult);

      Node *N = RAII_New(Node)(NodeIdResult.Value);
      Vector_Push(Topology->Nodes, N);

#ifndef NDEBUG
      String NodeAsString = TO_STRING(Node)(
          Vector_At(Topology->Nodes, Vector_Size(Topology->Nodes) - 1));
      puts(NodeAsString);
      String NodesAsString = Vector_ToString(Topology->Nodes, TO_STRING(Node));
      puts(NodesAsString);
#endif

      State.Type = Vector_Size(Topology->Nodes) >= State.NodeData.Count
                       ? ParseRailwaySystemTopologyStateType_ReadRailwaysLabel
                       : ParseRailwaySystemTopologyStateType_ReadNode;

      break;
    }
    case ParseRailwaySystemTopologyStateType_ReadRailwaysLabel: {
      PARSE_LABEL_HEADER(ROADS) {
        PARSE_UNSIGNED_INTEGER(R);
        State.RailwayData.Count = R.Value;
        Vector_InitWithCapacity(Topology->Railways, State.RailwayData.Count);

        State.Type = ParseRailwaySystemTopologyStateType_ReadRailway;
      }
      else {
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
      Railway *R = RAII_New(Railway)(RailwayIdResult.Value, ConnectedNode);
      Vector_Push(Topology->Railways, R);

#ifndef NDEBUG
      String RailwayAsString = TO_STRING(Railway)(
          Vector_At(Topology->Railways, Vector_Size(Topology->Railways) - 1));
      puts(RailwayAsString);
#endif

      State.Type = Vector_Size(Topology->Railways) >= State.RailwayData.Count
                       ? ParseRailwaySystemTopologyStateType_ReadEdgesLabel
                       : ParseRailwaySystemTopologyStateType_ReadRailway;

      break;
    }
    case ParseRailwaySystemTopologyStateType_ReadEdgesLabel: {
      PARSE_LABEL_HEADER(EDGES) {
        PARSE_UNSIGNED_INTEGER(R);
        State.EdgeData.Count = R.Value;
        Vector_InitWithCapacity(Topology->Edges, State.EdgeData.Count);

        State.Type = ParseRailwaySystemTopologyStateType_ReadEdge;
      }
      else {
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

      Edge *E = RAII_New(Edge)(EdgeIdResult.Value, A, B);
      Vector_Push(Topology->Edges, E);

#ifndef NDEBUG
      String EdgeAsString = TO_STRING(Edge)(
          Vector_At(Topology->Edges, Vector_Size(Topology->Edges) - 1));
      puts(EdgeAsString);
#endif

      State.Type = Vector_Size(Topology->Edges) >= State.EdgeData.Count
                       ? ParseRailwaySystemTopologyStateType_Complete
                       : ParseRailwaySystemTopologyStateType_ReadEdge;

      break;
    }
    }
  }

  for (size_t i = 0; i < Vector_Size(Topology->Edges); ++i) {
    Edge *E = Vector_At(Topology->Edges, i);
    Vector_Push(E->A->Edges, E);
    Vector_Push(E->B->Edges, E);
  }

  goto Success;

Failure:
  RAII_Destroy(RailwaySystemTopology)(Topology);
Success:
  fclose(F);

  return Result;
}
