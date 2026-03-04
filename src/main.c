/* Для выполнения пункта 1.1 задания необходимо:
 *
 * 1. Спроектировать структуру многопоточного приложения, поставить в
 * соответствие элементам моделируемой системы адекватные структуры данных и
 * совокупности потоков управления.
 *
 * 2. Разработать и отладить на выбранном языке многопоточную программу
 * моделирования поведения заданной системы, предусмотреть возможность вывода
 * детальной истории поведения всех элементов системы.
 *
 * 3. Промоделировать поведение спроектированной системы для различных значений
 * ее параметров, исследовать зависимости результатов моделирования от этих
 * значений.
 *
 * 4. Проанализировать взаимосвязи потоков/процессов в моделирующей программе,
 * выявить возможности возникновения гонок данных, дедлоков и других
 * специфических проблем. Выбрать способы предотвращения возможности
 * возникновения таких проблем.
 *
 * 5. Проанализировать результаты моделирования заданной системы, выявить и
 * объяснить закономерности их поведения и, если есть, – возможные отклонения
 * результатов от ожидаемых, в том числе такие как возникновение дедлоков.
 *
 * 24. На узловой станции железной дороги сходятся n путей, по которым движутся
 * поезда в разных направлениях, существуют цепочки стрелок для проезда с любого
 * пути на любой другой. Длина каждого поезда не превышает расстояние между
 * стрелками, переключающими пути. Любой такой отрезок пути между стрелками
 * считается полностью занятым, пока на нем находится хотя бы один вагон поезда.
 */

#include "cli.h"
#include "parser.h"
#include "topology.h"
#include "types.h"

#ifndef VERSION
#error Provide VERSION as -DVERSION="<version>"
#endif

i32 main(const i32 Argc, const c8 *const *const Argv) {
  CommandLineArguments Args;
  const CommandLineArgumentsParseResult CliParseResult =
      CommandLineArguments_Parse(&Args, Argc, Argv);

#ifndef NDEBUG
  String ArgsAsString = TO_STRING(CommandLineArguments)(&Args);
  puts(ArgsAsString);
#endif

  if (Args.Usage) {
    Usage(Args.ProgramName, ExitStatus_SUCCESS);
  }
  if (Args.Version) {
    printf("mirp-lab-1 %s\n", VERSION);
    exit(ExitStatus_SUCCESS);
  }
  if (CliParseResult != CommandLineArgumentsParseResult_SUCCESS) {
    Usage(Args.ProgramName, ExitStatus_FAILURE);
  }

  RailwaySystemTopology Topology;
  const ParseRailwaySystemTopologyResult ParseResult =
      ParseRailwaySystemTopology(Args.TopologyFilePath, &Topology);

  if (ParseResult == ParseRailwaySystemTopologyResult_Failure) {
    return ExitStatus_FAILURE;
  }

  String S = TO_STRING(RailwaySystemTopology)(&Topology);
  puts(S);

  printf("IsFullyConnected = %s\n",
         btoa(RailwaySystemTopology_IsFullyConnected(&Topology)));

  RAII_Destroy(RailwaySystemTopology)(&Topology);
  return ExitStatus_SUCCESS;
}
