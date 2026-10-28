*This project has been created as part of the 42 curriculum by yaarab.*

# Codexion

## Description

Codexion is a C concurrency simulation built with POSIX threads. It adapts the
dining philosophers problem to coders sharing USB dongles: each coder needs two
adjacent dongles to compile, then releases them to debug and refactor. A coder
burns out if too much time passes without starting another compilation.

The objective is to coordinate exclusive access to limited resources while
allowing independent coders to compile concurrently. A single shared priority
heap supports FIFO and EDF resource scheduling, and released dongles must cool
down before reuse. The activities are simulated delays; no external compiler
is launched during the simulation.

## Instructions

### Build

Use a POSIX environment with `make`, a C compiler available as `cc`, and pthread
support:

```sh
make
```

The Makefile compiles all 14 C files with
`-Wall -Wextra -Werror -pthread` and links `codexion`. It uses object files,
tracks `codex.h` as a prerequisite, and avoids rebuilding unchanged targets.

### Run

All eight arguments are mandatory:

```text
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug time_to_refactor number_of_compiles_req dongle_cooldown scheduler
```

| Position | Argument | Meaning |
| --- | --- | --- |
| 1 | `number_of_coders` | Number of coder threads and shared dongles. Must be at least one for initialization to succeed. |
| 2 | `time_to_burnout` | Maximum elapsed time since a coder's last compilation **started**, initially measured from the common simulation start. |
| 3 | `time_to_compile` | Duration of a compilation while holding both dongles. |
| 4 | `time_to_debug` | Debugging duration after releasing the dongles. |
| 5 | `time_to_refactor` | Refactoring duration before requesting the dongles again. |
| 6 | `number_of_compiles_req` | Completed compilations required from each coder. |
| 7 | `dongle_cooldown` | Minimum delay between releasing a dongle and reserving it again. |
| 8 | `scheduler` | Exactly `fifo` or `edf`, in lowercase. |

All durations are in **milliseconds**. The seven numeric arguments must contain
only decimal digits and fit in an `int`; signs and surrounding whitespace are
rejected. Zero durations are accepted. A zero compilation quota ends without
coder activity; a zero burnout duration causes immediate burnout when the quota
is positive. A zero coder count fails initialization.

**FIFO** gives the earliest submitted request priority among eligible requests.
**EDF** gives the smallest deadline priority, where
`deadline = last_compile + time_to_burnout`. Equal EDF deadlines use submission
order; coder ID is the final tie-breaker in both policies. These policies govern
dongle grants, not the operating system's CPU scheduling policy.

Examples:

```sh
./codexion 4 350 100 50 50 3 20 fifo
./codexion 4 350 100 50 50 3 20 edf
./codexion 2 800 200 100 100 3 20 fifo
```

These commands also have valid arguments, but illustrate expected burnout:

```sh
# One coder cannot obtain two distinct dongles.
./codexion 1 350 100 50 50 3 20 fifo

# Burnout can occur while a compilation is still running.
./codexion 4 100 200 50 50 3 20 edf
```

Output uses `elapsed_ms coder_id action`, with coder IDs starting at 1. Actions
include `has taken a dongle`, `is compiling`, `is debugging`, `is refactoring`,
and `burned out`. Two acquisition lines precede each compilation. Thread
execution order and timestamps can vary. Successful quota completion has no
separate summary line. Burnout is reported in the output and, like normal
completion, currently returns exit status 0; input or setup failures return 1.

### Clean and rebuild

```sh
make clean   # Remove object files.
make fclean  # Remove object files and codexion.
make re      # Clean everything, then rebuild.
```

## Implementation

The shared `t_data` structure holds configuration, coder and dongle arrays,
synchronization objects, and **one global request heap**. Each coder uses
dongles `i` and `(i + 1) % number_of_coders`; there are no per-dongle heaps.

The main thread creates one monitor and one worker per coder. They wait until
the common start time and initial `last_compile` values have been published.
Each worker then repeats the following sequence until its quota or simulation
stop:

1. Submit one request containing its ID, deadline snapshot, and submission
   sequence number.
2. Poll for a grant. Any waiting worker can run arbitration for the entire heap
   while holding `sch_mutex`; there is no dedicated scheduler thread.
3. Once both dongles are reserved, check stop and deadline expiry, update
   `last_compile`, log the start, and simulate compilation.
4. Count a completed compilation and release both dongles. If its quota is met,
   the worker exits; otherwise, it simulates debugging and refactoring.

`find_ready_request()` scans the heap for the highest-priority request whose
two distinct dongles are free and out of cooldown. It uses one timestamp for
each scan. `try_grant_requests()` reserves that pair, removes the request using
`heap_remove_at()`, publishes the grant, and repeats. A blocked higher-priority
request can therefore be bypassed by an eligible request. Arbitration explicitly
compares priorities rather than treating the heap array as a sorted list.

Reservation marks both dongles in use together. The short-lived dongle mutexes
are released before the simulated compilation; the protected `in_use` fields
record ownership throughout that activity. Release clears ownership and sets
each dongle's `available_at` to the release time plus its cooldown.

The monitor checks unfinished coders for burnout and detects when all quotas
are met. Either event sets the shared stop flag. Workers observe that flag in
their waiting loops, release outstanding grants, and exit. The main thread joins
the monitor and workers before destroying synchronization objects and freeing
the heap and arrays.

## Blocking cases handled

| Case | Implemented mechanism and limits |
| --- | --- |
| Deadlock and Coffman's conditions | Mutual exclusion and non-preemption of an active compilation remain necessary. Pair reservation breaks hold-and-wait for logical dongle ownership: a waiting coder owns neither dongle. Pair mutexes are always locked in increasing index order, preventing circular wait between them. The broader nested lock paths are described below. |
| Starvation prevention | Submission order prioritizes older eligible FIFO requests; EDF prioritizes earlier deadlines. Blocked requests do not prevent independent eligible work, and finished coders stop competing. There is no aging mechanism or proven bounded waiting guarantee. Resource contention, cooldown, and insufficient burnout time can still prevent completion. |
| Dongle cooldown | Reservation checks both `available_at` timestamps as well as ownership. Release starts a new cooldown for both dongles. |
| Concurrent resource access | `sch_mutex` serializes scheduling and release; dongle mutexes protect occupancy and availability. Two workers may compile simultaneously only when their reserved pairs are disjoint. |
| Precise burnout detection | The monitor checks `current - last_compile >= time_to_burnout` for unfinished coders, polling with `usleep(500)` between checks. Compilation start also rejects an expired deadline before resetting it. This aims for prompt detection, but does not establish a hard 10 ms guarantee: OS scheduling, lock contention, and output blocking can delay it. Timing uses millisecond `gettimeofday()` values and is sensitive to wall-clock changes. |
| Serialized logging | `log_mutex` protects output. Compilation start emits its two acquisition records and compilation record together with one timestamp. The monitor uses the same mutex when deciding and reporting termination; ordinary logging suppresses messages after stop. |
| Safe thread termination | Interruptible polling loops check `stop`; compilation interruption and unconsumed grants release reserved pairs. Partial worker-creation failure sets stop before joining only successfully created workers. Initialization failure paths clean up previously initialized objects. Normal cleanup follows all joins. |

With one coder, both indices refer to the same dongle. Reservation rejects this
pair instead of locking the same mutex twice; a positive quota therefore leads
to burnout rather than compilation.

## Thread synchronization mechanisms

`pthread_mutex_t` provides exclusive access to shared state. In this project,
each mutex has a specific responsibility:

| Mutex | Protected state or operation |
| --- | --- |
| `state_mutex` | `stop`, startup publication, `last_compile`, and monitor-visible compilation counts (`complie_c`). Each worker is the sole writer of its own count and also reads that count after updating it. |
| `log_mutex` | Output serialization and coordination of logging, compilation start, and monitor termination decisions. |
| `sch_mutex` | Heap access, request sequence numbers, `request_pending`, `dongles_granted`, arbitration, and release. |
| Each dongle's `mutex` | `in_use` and `available_at`; runtime pair operations also take place inside the scheduler critical section. |

The nested lock paths are `log_mutex -> state_mutex`,
`sch_mutex -> state_mutex`, and
`sch_mutex -> lower-index dongle mutex -> higher-index dongle mutex`.
Code does not acquire the scheduler or logging mutex while holding
`state_mutex`, and releases locks before joining threads.

`pthread_cond_t` normally supports sleeping until a shared predicate may have
changed. **No condition variable is actively waited on in this implementation:**

- `sch_cond` is initialized and destroyed, signaled on request submission, and
  broadcast after grants and releases.
- Each dongle's `cond` is initialized and destroyed but is neither signaled nor
  waited on.
- There are no calls to `pthread_cond_wait()` or `pthread_cond_timedwait()`.
  Scheduler notifications therefore do not wake workers. Communication actually
  uses mutex-protected state and polling with `usleep(500)`, including startup,
  grant waiting, activity delays, and monitoring.

The monitor observes timestamps and counts under `state_mutex` and publishes
stop through that same mutex. Workers observe grants under `sch_mutex` and stop
under `state_mutex`. Configuration and pair indices are initialized before
worker execution and remain unchanged.

## Resources

- [POSIX threads overview — Linux man-pages](https://man7.org/linux/man-pages/man7/pthreads.7.html)
- [Mutex locking — POSIX Programmer's Manual](https://man7.org/linux/man-pages/man3/pthread_mutex_lock.3p.html)
- [Condition variable waits — POSIX Programmer's Manual](https://man7.org/linux/man-pages/man3/pthread_cond_wait.3p.html)
- [Heaps and Priority Queues — OpenDSA](https://opendsa-server.cs.vt.edu/ODSA/Books/CS3/html/Heaps.html)
- [CPU Scheduling, including FCFS/FIFO — University of Illinois Chicago](https://www.cs.uic.edu/~jbell/CourseNotes/OperatingSystems/6_CPU_Scheduling.html)
- [EDF background — Linux kernel deadline scheduling documentation](https://docs.kernel.org/scheduler/sched-deadline.html)
- [Deadlocks and Coffman's conditions — University of Illinois Chicago](https://www.cs.uic.edu/~jbell/CourseNotes/OperatingSystems/7_Deadlocks.html)

These references explain the underlying concepts; their implementations and
scheduling guarantees should not be attributed to this simulation. The Codexion
subject PDF was not available in this checkout, so this README describes the
source code rather than claiming independent verification of every subject rule.

## AI Usage

AI tools, were used to help understand the project requirements, assist with debugging, and test the implementation.