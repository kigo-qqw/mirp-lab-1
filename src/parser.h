#pragma once

#include "topology.h"

typedef enum ParseRailwaySystemTopologyResult {
  ParseRailwaySystemTopologyResult_Success = 0,
  ParseRailwaySystemTopologyResult_Failure = -1
} ParseRailwaySystemTopologyResult;

ParseRailwaySystemTopologyResult
ParseRailwaySystemTopology(const char *TopologyFilename,
                           RailwaySystemTopology *Topology);
