# DoubleClique

DoubleClique is a high‑performance, **in‑memory, replicated key–value store** that runs on Dolphin’s PCIe interconnect using the SISCI API.

Originally developed as a part of a master's thesis ([https://hdl.handle.net/10852/120753](https://hdl.handle.net/10852/120753)) by [@kritjo](https://github.com/kritjo/DoubleClique).

## Quick tour

```
.
├── client/         # client process (benchmarks + GET/PUT implementations)
├── server/         # replica process (hash table + request region handlers)
├── common/         # shared utils, buddy allocator, protocol headers
├── cmake/          # FindSISCI.cmake helper
└── CMakeLists.txt  # top‑level build recipe
```

---

## Building

> **Prerequisites**
>
> * C compiler with C17 support  
> * [SISCI headers](https://www.dolphinics.com/) installed in **/opt/DIS**  
> * CMake ≥ 3.28
> * x86-64 CPU with AVX2 and SSE4.2 (the build uses `-march=native`)

```bash
cmake -B build
cmake --build build -j$(nproc)
```

This produces two binaries:

| Binary      | Location           | Purpose |
|-------------|--------------------|---------|
| `server`    | `build/server/`    | runs on each replica node |
| `client`    | `build/client/`    | benchmark / KV workload generator |

Build the client and every replica from the same commit, on the same CPU type. The request layout and the
payload hash are part of the protocol, and both changed over time.

---

## Configuration

All settings are compile-time constants. Client and replicas must agree on them, so rebuild both after a change.

| Setting | File | Default | Meaning |
|---------|------|---------|---------|
| `REPLICA_COUNT` | `common/index_data_protocol.h` | 1 | number of replicas |
| `DATA_REGION_SIZE` | `common/index_data_protocol.h` | 16 MB | value storage per replica |
| `SPECULATIVE_SIZE` | `common/index_data_protocol.h` | 256 | largest value a 2-sided GET returns in one round |
| `MAX_SIZE_ELEMENT_EXP` | `common/sisci_glob_defs.h` | 11 | log2 of the largest stored element (key + value + version) |
| `REQUEST_SLOTS` | `common/request_region.h` | 1024 | requests that can be outstanding at once |
| `PUT_TIMEOUT_NS` | `client/put.h` | 1 ms | PUT timeout |
| `GET_TIMEOUT_2_SIDED_NS` | `client/2_phase_2_sided.h` | 1 ms | 2-sided GET timeout, per round |
| `VALUE_LEN` | `client/main.c` | 8 | value size used by the benchmark |
| `PERF_ENABLED` | `common/profiler.h` | 1 | per-stage timing report; adds timing calls to the request path |

Larger values need a larger `DATA_REGION_SIZE`, and `MAX_SIZE_ELEMENT_EXP` must cover key + value + version.
If the replicas run out of space the initial load never finishes.

The request region is a 32 MB multicast segment. The default multicast group size of the MX driver is 2 MB, so
set `ntb_mcast_group_size=25;` in `/opt/DIS/lib/modules/dis_mx.conf` on **all** nodes and reload the driver.

---

## Running a test cluster

1. **Start the replica nodes** (`REPLICA_COUNT` of them, here three)

```bash
# on node 0
build/server/server 0
# on node 1
build/server/server 1
# on node 2
build/server/server 2
```

2. **Run the client** (pass the Dolphin *node IDs* of the replicas in **the order you prefer**):

```bash
# on the client host
build/client/client 8 12 16
```

The client loads the table and then runs each workload (PUT, 2-sided GET, 1-sided GET and three GET/PUT
mixes, with uniform and zipf keys) at 1 and 128 requests outstanding. For each one it prints:

* throughput and average, p50, p95 and p99 latency;
* `TAIL`: p99.9, p99.99, maximum, and how many requests took over 30 µs and 60 µs;
* `ROUNDS`: GET latency split into requests answered in one round and requests that needed a second one;
* `Status`: how many requests ended in each result, so failures are visible. A summary table follows at the end.

Reported GET latency is end to end, from the first round's submission.

---

## Acknowledgements
* **Paul Hsieh** – for the `SuperFastHash`.  
* **Stanislav Paskalev** – for the `buddy_alloc` implementation.
