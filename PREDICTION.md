# Prediction sheet — push by 0:20, before you compile

> Marked on **having predicted** and on reconciling it in S3.1 — **not on being
> right.** A confident wrong prediction you then explain is full marks. A blank
> page is none. A page timestamped after your first run is worse than none.
>
> Read `src/given.c` and `BRIEF.md`. Run nothing.

Cores:  8
Lab 0 spread:  220.8

> **P1.** `./bar given` on **one** thread — does it come out right? Yes/no, one
> sentence why.

Yes, since the same thread which is in the wait, is also going to be the signal. and so it resets the count. The barrier degenerates to a lock and unlock.

> **P2.** On **8** threads, pick one and commit to it: right answer / wrong
> answer / it stops. If wrong, roughly how big is `bad`? If it stops, say at
> which of the two waits in a round.

It stops because, the program never finishes since there are 8 threads with 7 waiters so one wakes up and sees that they arent equal, while the remaining 6 threads stay asleep, therefore they stay at the first wait of the first round, and the program never finishes.

> **P3.** Three runs at 8 threads — **identical** numbers, or different? Think
> about this one before you write it; it is the most useful line on the page.

It's probably identical since the last arriver holds the mutex, and so all 7 are asleep when the signal goes out. 

> **P4.** Seconds, before measuring. Orders of magnitude are what matter. `cpu`
> is process CPU time over all threads, so `cpu`/`time` is how many cores were
> busy — one number per box.

| | 1 thread: time | 8 threads: time | 8 threads: cpu/time |
|---|---|---|---|
| `given` | 0.001  | never finishes | 0  |
| `fixed` | 0.001  | 0.05  | 2  |
| `alt` | 0.001  | 0.1  | 2–3 |

> **P5.** Fastest and slowest at 8 threads? Name anything you expect to get
> **slower** as threads are added, and anything you expect to stop altogether.

Fastest is fixed.c , as its one broadcast per round. Slowest is probably alt.c since more wakeups and context switches. given.c stops alltogather.
