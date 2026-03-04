#include "cli.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "to_string_impl.h"

static const c8 *const ShortOptions = "huv";

static struct option LongOptions[] = {{"help", no_argument, NULL, 'h'},
                                      {"usage", no_argument, NULL, 'u'},
                                      {"version", no_argument, NULL, 'v'}};

void Usage(const c8 *const ProgramName, const ExitStatus Status) {
  FILE *Out = (Status == ExitStatus_FAILURE ? stderr : stdout);
  fprintf(Out, "Usage: %s [options] <file>\n\n", ProgramName);
  fputs("file                           topology filepath\n",
        Out); // TODO: topology specification
  fputs("Options:\n", Out);
  fputs("    -h, -u, --help, --usage    display this message\n", Out);
  fputs("    -v, --version              display version\n", Out);

  exit(Status);
}

CommandLineArgumentsParseResult
CommandLineArguments_Parse(CommandLineArguments *const Self, const i32 Argc,
                           const c8 *const *const Argv) {
  Self->Usage = false;
  Self->Version = false;
  Self->ProgramName = Argv[0];
  Self->TopologyFilePath = NULL;

  i32 OptionCharacter;
  i32 OptionIndex;
  while ((OptionCharacter = getopt_long(Argc, (c8 *const *)Argv, ShortOptions,
                                        LongOptions, &OptionIndex)) != -1) {
    switch (OptionCharacter) {
    case 'v': {
      Self->Version = true;
    } break;
    case 'h':
    case 'u': {
      Self->Usage = true;
    } break;
    case '?':
    default: {
      return CommandLineArgumentsParseResult_UNKNOWN_ARGUMENT;
    }
    }
  }

  if (Argv[optind] != NULL) {
    Self->TopologyFilePath = Argv[optind];
  } else {
    return CommandLineArgumentsParseResult_NO_POSITION_ARGUMENT;
  }

  return CommandLineArgumentsParseResult_SUCCESS;
}

TO_STRING_DEFINITION(CommandLineArguments, {},
                     "CommandLineArguments{ProgramName=%s, "
                     "TopologyFilePath=%s, Usage=%s, Version=%s}",
                     CommandLineArguments->ProgramName,
                     CommandLineArguments->TopologyFilePath,
                     btoa(CommandLineArguments->Usage),
                     btoa(CommandLineArguments->Version));
