#include "scada/node_id.h"

#include <benchmark/benchmark.h>

#include <format>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

// Micro-benchmarks for the NodeId access patterns that dominate the system:
// point lookups in NodeId-keyed maps (address space, node-service registry) and
// repeated comparison of NodeIds. Build and run on demand:
//
//   cmake --build --preset release-dev --target scada_core_benchmarks
//   ./scada_core_benchmarks                       # all, with statistics
//   ./scada_core_benchmarks --benchmark_filter=NodeId
//   ./scada_core_benchmarks --benchmark_out=nodeid.json
//   --benchmark_out_format=json
//
// Numeric ids are by far the most common, so they get the numeric/numeric
// comparison fast path; string ids that were copied share one control block, so
// their comparison short-circuits on pointer identity.

namespace scada {
namespace {

std::vector<NodeId> MakeNumericIds(std::size_t count) {
  std::vector<NodeId> ids;
  ids.reserve(count);
  for (std::size_t i = 0; i < count; ++i) {
    // Spread values so map/hash probes are not artificially ordered.
    ids.emplace_back(static_cast<NumericId>(i * 2654435761u % count));
  }
  return ids;
}

std::vector<NodeId> MakeStringIds(std::size_t count) {
  std::vector<NodeId> ids;
  ids.reserve(count);
  for (std::size_t i = 0; i < count; ++i) {
    ids.emplace_back(std::format("Device.Channel.Tag_{}", i),
                     static_cast<NamespaceIndex>(2));
  }
  return ids;
}

template <class Map>
void FillMap(Map& map, const std::vector<NodeId>& ids) {
  for (std::size_t i = 0; i < ids.size(); ++i)
    map.emplace(ids[i], i);
}

// --- Lookup benchmarks: ordered std::map vs hashed std::unordered_map. ---

void BM_MapNumericLookup(benchmark::State& state) {
  const std::vector<NodeId> ids = MakeNumericIds(state.range(0));
  std::map<NodeId, std::size_t> map;
  FillMap(map, ids);
  std::size_t i = 0;
  for (auto _ : state) {
    benchmark::DoNotOptimize(map.find(ids[i])->second);
    i = (i + 1 == ids.size()) ? 0 : i + 1;
  }
}
BENCHMARK(BM_MapNumericLookup)->Arg(100000);

void BM_UnorderedNumericLookup(benchmark::State& state) {
  const std::vector<NodeId> ids = MakeNumericIds(state.range(0));
  std::unordered_map<NodeId, std::size_t> map;
  map.reserve(ids.size());
  FillMap(map, ids);
  std::size_t i = 0;
  for (auto _ : state) {
    benchmark::DoNotOptimize(map.find(ids[i])->second);
    i = (i + 1 == ids.size()) ? 0 : i + 1;
  }
}
BENCHMARK(BM_UnorderedNumericLookup)->Arg(100000);

void BM_MapStringLookup(benchmark::State& state) {
  const std::vector<NodeId> ids = MakeStringIds(state.range(0));
  std::map<NodeId, std::size_t> map;
  FillMap(map, ids);
  std::size_t i = 0;
  for (auto _ : state) {
    benchmark::DoNotOptimize(map.find(ids[i])->second);
    i = (i + 1 == ids.size()) ? 0 : i + 1;
  }
}
BENCHMARK(BM_MapStringLookup)->Arg(100000);

void BM_UnorderedStringLookup(benchmark::State& state) {
  const std::vector<NodeId> ids = MakeStringIds(state.range(0));
  std::unordered_map<NodeId, std::size_t> map;
  map.reserve(ids.size());
  FillMap(map, ids);
  std::size_t i = 0;
  for (auto _ : state) {
    benchmark::DoNotOptimize(map.find(ids[i])->second);
    i = (i + 1 == ids.size()) ? 0 : i + 1;
  }
}
BENCHMARK(BM_UnorderedStringLookup)->Arg(100000);

// --- Comparison benchmarks: numeric fast path and shared-pointer
// short-circuit. ---

void BM_CompareNumericEqual(benchmark::State& state) {
  const NodeId a{4242, 3};
  const NodeId b{4242, 3};
  for (auto _ : state)
    benchmark::DoNotOptimize(a == b);
}
BENCHMARK(BM_CompareNumericEqual);

void BM_CompareNumericLess(benchmark::State& state) {
  const NodeId a{4242, 3};
  const NodeId b{4243, 3};
  for (auto _ : state)
    benchmark::DoNotOptimize(a < b);
}
BENCHMARK(BM_CompareNumericLess);

// A copy shares the string's control block, so the short-circuit answers
// without dereferencing.
void BM_CompareStringEqual_SharedBuffer(benchmark::State& state) {
  const NodeId original{"Device.Channel.Tag_500", 2};
  const NodeId copy = original;
  for (auto _ : state)
    benchmark::DoNotOptimize(original == copy);
}
BENCHMARK(BM_CompareStringEqual_SharedBuffer);

// Two equal strings with distinct control blocks: the short-circuit misses and
// the full value compare runs (baseline for the row above).
void BM_CompareStringEqual_DistinctBuffer(benchmark::State& state) {
  const NodeId a{"Device.Channel.Tag_500", 2};
  const NodeId b{std::string{"Device.Channel.Tag_500"}, 2};
  for (auto _ : state)
    benchmark::DoNotOptimize(a == b);
}
BENCHMARK(BM_CompareStringEqual_DistinctBuffer);

void BM_CompareStringLess_SharedBuffer(benchmark::State& state) {
  const NodeId original{"Device.Channel.Tag_500", 2};
  const NodeId copy = original;
  for (auto _ : state)
    benchmark::DoNotOptimize(original < copy);
}
BENCHMARK(BM_CompareStringLess_SharedBuffer);

// --- Hashing. ---

void BM_HashNumeric(benchmark::State& state) {
  const std::hash<NodeId> hasher;
  const NodeId id{4242, 3};
  for (auto _ : state)
    benchmark::DoNotOptimize(hasher(id));
}
BENCHMARK(BM_HashNumeric);

void BM_HashString(benchmark::State& state) {
  const std::hash<NodeId> hasher;
  const NodeId id{"Device.Channel.Tag_500", 2};
  for (auto _ : state)
    benchmark::DoNotOptimize(hasher(id));
}
BENCHMARK(BM_HashString);

}  // namespace
}  // namespace scada
