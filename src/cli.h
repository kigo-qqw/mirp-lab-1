#pragma once

#include "types.h"

typedef enum ExitStatus {
  ExitStatus_SUCCESS = 0,
  ExitStatus_FAILURE = 1,
} ExitStatus;

typedef enum CommandLineArgumentsParseStatus {
  // TODO: parse errors, invalid argument, unknown argument, etc.
  CommandLineArgumentsParseStatus_SUCCESS = 0,
} CommandLineArgumentsParseStatus;

typedef struct CommandLineArguments {
  const char *const TopologyFilePath;
} CommandLineArguments;

CommandLineArgumentsParseStatus
CommandLineArguments_Parse(CommandLineArguments *Self, i32 argc,
                           const c8 *const *argv);
