# Perf doc

* Running with profiler flag `-P`
* 10000 iterations (-i 10000`)

***

### 2026-09-26 19:58 (First run of the profiler)

|Subsystem                        |   Time (ms)  |  Share %  | Calls     |
|---------------------------------|-------------:|----------:|----------:|
|LPU Instruction Step             |   3650.51 ms |  22.19%   |   1063121 |
|Randomizer / Mutations           |   8697.02 ms |  52.85%   | 106312100 |
|Template Matching (fndb/fndf)    |   2206.45 ms |  13.41%   |   9909983 |
|Memory Allocation (maloc)        |   1433.26 ms |   8.71%   |    242579 |
|Memory Copying (movi)            |    190.73 ms |   1.16%   |   9169117 |
|Grave Cleanup                    |      1.51 ms |   0.01%   |       100 |
|Snapshot Writing                 |    275.26 ms |   1.67%   |        11 |
| | | |
|Total Tracked CPU Time           |  16454.75 ms | 100.00%   |           |



