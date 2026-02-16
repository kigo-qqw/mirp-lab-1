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

#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "topology.h"

void PrintTopology(const RailwaySystemTopology *Topology) {
  printf("Nodes (%zu):\n", Vector_Size(Topology->Nodes));
  for (int i = 0; i < Vector_Size(Topology->Nodes); ++i) {
    const Node *N = Vector_At(Topology->Nodes, i);
    printf("  Node %zu, connected to %zu edges: ", N->Id,
           Vector_Size(N->Edges));
    // for (int j = 0; j < N->EdgeCount; ++j) {
    //     Edge *E = N->Edges[j];
    //     Node *Other = (E->A == N) ? E->B : E->A;
    //     printf("%d ", Other->Id);
    // }
    printf("\n");
  }

  printf("Edges (%zu):\n", Vector_Size(Topology->Edges));
  for (int i = 0; i < Vector_Size(Topology->Edges); ++i) {
    const Edge *E = Vector_At(Topology->Edges, i);
    printf("  Edge %zu connects Node %zu <-> Node %zu\n", E->Id, E->A->Id,
           E->B->Id);
  }

  printf("Railways (%zu):\n", Vector_Size(Topology->Railways));
  for (int i = 0; i < Vector_Size(Topology->Railways); ++i) {
    const Railway *R = Vector_At(Topology->Railways, i);
    printf("  Railway %zu connected to Node %zu\n", R->Id,
           R->ConnectedNode->Id);
  }
}

int main(const int argc, const char *const *const argv) {
  RailwaySystemTopology Topology;
  const ParseRailwaySystemTopologyResult ParseResult =
      ParseRailwaySystemTopology(argv[1], &Topology);

  if (ParseResult == ParseRailwaySystemTopologyResult_Failure) {
    return 1;
  }

  puts(TO_STRING_IDENTIFIER(Node)(Vector_At(Topology.Nodes, 0)));

  puts(TO_STRING_IDENTIFIER(RailwaySystemTopology)(&Topology));

  RAII_Destroy_IDENTIFIER(RailwaySystemTopology)(&Topology);

  return 0;
}
