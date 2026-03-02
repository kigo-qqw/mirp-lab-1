#pragma once

#include <stdbool.h>

#include "types.h"

typedef enum ExitStatus {
  ExitStatus_SUCCESS = 0,
  ExitStatus_FAILURE = 1,
} ExitStatus;

typedef enum CommandLineArgumentsParseResult {
  // TODO: parse errors, invalid argument, unknown argument, etc.
  CommandLineArgumentsParseResult_NO_POSITION_ARGUMENT = -2,
  CommandLineArgumentsParseResult_UNKNOWN_ARGUMENT = -1,
  CommandLineArgumentsParseResult_SUCCESS = 0,
} CommandLineArgumentsParseResult;

typedef struct CommandLineArguments {
  const char *TopologyFilePath;
  bool Usage;
  bool Version;
} CommandLineArguments;

void Usage(const c8 *ProgramName, ExitStatus Status);

CommandLineArgumentsParseResult
CommandLineArguments_Parse(CommandLineArguments *Self, i32 Argc,
                           const c8 *const *Argv);
