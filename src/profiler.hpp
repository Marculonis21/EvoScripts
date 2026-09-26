#pragma once

#include <chrono>
#include <cstdint>
#include <cstdio>
#include <string>

enum ProfileCategory {
    PROF_EXEC = 0,        // Core instruction execution
    PROF_RANDOMIZER,     // Mutation and cosmic rays
    PROF_TEMPLATE_MATCH, // fndb / fndf template scanning and sorting
    PROF_ALLOC,          // maloc and AllocSpacesContainer searching
    PROF_COPY,           // movi cell-to-cell byte copying
    PROF_GRAVES,         // Death reaper & grave cleanup
    PROF_SNAPSHOT,       // Visualizer JSON/file snapshot generation
    PROF_COUNT
};

#ifdef NO_PROFILING

// Compile-time zero-overhead stub:
// When NO_PROFILING is defined, every ProfileScope call optimizes away to literally 0 CPU instructions.
struct ProfileScope {
    inline explicit ProfileScope(ProfileCategory) noexcept {}
};

struct Profiler {
    bool enabled = false;
    static Profiler& get() { static Profiler instance; return instance; }
    void printReport() const {}
    void reset() {}
};

#else

struct ProfileScope;

struct Profiler {
    using clock = std::chrono::steady_clock;

    bool enabled = false; // Controlled at runtime (e.g. via --profile CLI flag)

    uint64_t inclusive_ns[PROF_COUNT] = {0};
    uint64_t exclusive_ns[PROF_COUNT] = {0};
    uint64_t call_counts[PROF_COUNT] = {0};

    ProfileScope* currentScope = nullptr;

    static Profiler& get() {
        static Profiler instance;
        return instance;
    }

    void reset() {
        for (int i = 0; i < PROF_COUNT; ++i) {
            inclusive_ns[i] = 0;
            exclusive_ns[i] = 0;
            call_counts[i] = 0;
        }
    }

    void printReport() const {
        if (!enabled) return;

        const char* names[PROF_COUNT] = {
            "LPU Instruction Step",
            "Randomizer / Mutations",
            "Template Matching (fndb/fndf)",
            "Memory Allocation (maloc)",
            "Memory Copying (movi)",
            "Grave Cleanup",
            "Snapshot Writing"
        };

        uint64_t total_exclusive = 0;
        for (int i = 0; i < PROF_COUNT; ++i) {
            total_exclusive += exclusive_ns[i];
        }
        if (total_exclusive == 0) return;

        std::printf("\n================ Performance Subsystem Breakdown ================\n");
        std::printf("  %-32s |   Time (ms)  |  Share %% | Calls\n", "Subsystem");
        std::printf("  ---------------------------------+-------------+-----------+-----------\n");
        for (int i = 0; i < PROF_COUNT; ++i) {
            if (call_counts[i] == 0 && exclusive_ns[i] == 0) continue;
            double pct = (double)exclusive_ns[i] / total_exclusive * 100.0;
            double ms = (double)exclusive_ns[i] / 1e6;
            std::printf("  %-32s | %9.2f ms | %6.2f%%   | %9lu\n",
                        names[i], ms, pct, call_counts[i]);
        }
        std::printf("  ---------------------------------+--------------+----------+-----------\n");
        std::printf("  Total Tracked CPU Time           | %9.2f ms | 100.00%% |\n", (double)total_exclusive / 1e6);
        std::printf("=================================================================\n\n");
    }
};

struct ProfileScope {
    ProfileCategory cat;
    Profiler::clock::time_point start;
    uint64_t child_ns = 0;
    ProfileScope* parent = nullptr;
    bool active = false;

    inline explicit ProfileScope(ProfileCategory category) : cat(category) {
        if (!Profiler::get().enabled) return;
        active = true;
        auto& p = Profiler::get();
        parent = p.currentScope;
        p.currentScope = this;
        start = Profiler::clock::now();
    }

    inline ~ProfileScope() {
        if (!active) return;
        auto end = Profiler::clock::now();
        uint64_t total = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        uint64_t excl = (total > child_ns) ? (total - child_ns) : 0;

        auto& p = Profiler::get();
        p.inclusive_ns[cat] += total;
        p.exclusive_ns[cat] += excl;
        p.call_counts[cat]++;

        if (parent) {
            parent->child_ns += total;
        }
        p.currentScope = parent;
    }
};

#endif
