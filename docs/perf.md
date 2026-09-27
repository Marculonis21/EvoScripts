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


### 2026-09-26 22:39 (Randomizer improvements)

| Subsystem                        |   Time (ms)  |  Share % | Calls |
|---------------------------------|-------------:|----------:|----------:|
|LPU Instruction Step             |   3476.54 ms |  30.99%   |   1007762 |
|Randomizer / Mutations           |   3567.98 ms |  31.81%   | 100776200 |
|Template Matching (fndb/fndf)    |   2172.96 ms |  19.37%   |   9822643 |
|Memory Allocation (maloc)        |   1372.52 ms |  12.24%   |    232710 |
|Memory Copying (movi)            |    347.77 ms |   3.10%   |   9261743 |
|Grave Cleanup                    |      1.66 ms |   0.01%   |       100 |
|Snapshot Writing                 |    277.83 ms |   2.48%   |        11 |
| | | |
|Total Tracked CPU Time           |  11217.26 ms | 100.00% | |

### 2026-09-27 12:57 (Scanning heap memory improvements)

| Subsystem                        |   Time (ms)  |  Share % | Calls     | 
|---------------------------------|-------------:|-----------:|------------:| 
| LPU Instruction Step             |   3248.41 ms |  32.24%   |    958495| 
| Randomizer / Mutations           |   3404.60 ms |  33.79%   |  95849500| 
| Template Matching (fndb/fndf)    |   1444.67 ms |  14.34%   |   9662631| 
| Memory Allocation (maloc)        |   1192.09 ms |  11.83%   |    201509| 
| Memory Copying (movi)            |    335.35 ms |   3.33%   |   9166945| 
| Grave Cleanup                    |      1.80 ms |   0.02%   |       100| 
| Snapshot Writing                 |    449.44 ms |   4.46%   |        11| 
| | | |
| Total Tracked CPU Time           |  10076.36 ms | 100.00% |            | 
