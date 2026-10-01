# Lab 2 results — sealed core

Name:  Ahab Masud Siddiqui
Student number:  33276585
Lab section:  L02
Core: B
Machine:  Macbook M5 Pro
Cores: 4

## Tools and sources

Tools and sources: my hand and reading notes

> Mandatory, even if it says "none". **No AI in the lab, at all** — see the
> README. Missing declaration: zero until you supply one. False one: misconduct.

## S2 — the defect · 40 marks

Three or more runs of `./bar given`, including one thread:

```
mode=given threads=1 rounds=2000 bad=0 firstbad=-1 checksum=ok correct=yes deadlock=no time=0.0017 cpu=0.0017
mode=given threads=2 rounds=2000 bad=0 firstbad=-1 checksum=ok correct=yes deadlock=no time=0.0503 cpu=0.0532
mode=given threads=3 rounds=2000 bad=-1 firstbad=-1 checksum=unknown correct=no deadlock=yes time=5.0092 cpu=0.0026
mode=given threads=4 rounds=2000 bad=-1 firstbad=-1 checksum=unknown correct=no deadlock=yes time=5.0085 cpu=0.0025
mode=given threads=8 rounds=2000 bad=-1 firstbad=-1 checksum=unknown correct=no deadlock=yes time=5.0099 cpu=0.0027
```

**S2.1** Name the mechanism: which claim in `given.c`'s header is false, and
what is actually happening? State the barrier's invariant and say which half of
it this code does not keep.

The code claims the last thread wakes everyone up, but it only wakes one. The
rest keep sleeping forever because nobody is left to wake them.

A barrier should stop anyone leaving until everyone arrives, then let them all
go. This code does the first part but not the second as nobody leaves early, but
some threads never leave at all.

**S2.2** Prove it, in the form your `BRIEF.md` requires.

(1) With two threads only one is ever waiting, so waking one wakes everyone.
With one thread, nobody waits. So testing at one or two tells you nothing.

(2) The stuck run barely used any CPU, so the threads were asleep, not looping.
ThreadSanitizer stayed quiet because the threads never clash over shared data.
The bug is a missing wake-up, and that's not what it looks for.

**S2.3** Minimality: what breaks if you do less, what it costs if you do more.

The fix is one word: swap pthread_cond_signal for pthread_cond_broadcast,
so the last thread wakes everyone, not just one.

That's all it needs. Everyone else is already asleep when the last thread
arrives, so they all get woken, and the round number stops anyone mixing up
rounds. Adding more doesn't help. If you want to release threads one at a time
instead, you'd need extra work, like posting once per thread (alt) or having
each thread wake the next.


## S3 — the measurement · 30 marks

`./bar all <t> <rounds>` at 1, 2, 4 and 8 threads. Pasted, not retyped. If a
mode stops, `all` stops with it — run the modes one at a time and paste those.

```
mode=given threads=1 rounds=2000 bad=0 firstbad=-1 checksum=ok correct=yes deadlock=no time=0.0017 cpu=0.0017
mode=given threads=2 rounds=2000 bad=0 firstbad=-1 checksum=ok correct=yes deadlock=no time=0.0503 cpu=0.0532
mode=given threads=4 rounds=2000 bad=-1 firstbad=-1 checksum=unknown correct=no deadlock=yes time=5.0085 cpu=0.0025
mode=given threads=8 rounds=2000 bad=-1 firstbad=-1 checksum=unknown correct=no deadlock=yes time=5.0099 cpu=0.0027
mode=fixed threads=1 rounds=2000 bad=0 firstbad=-1 checksum=ok correct=yes deadlock=no time=0.0016 cpu=0.0017
mode=fixed threads=2 rounds=2000 bad=0 firstbad=-1 checksum=ok correct=yes deadlock=no time=0.0471 cpu=0.0442
mode=fixed threads=4 rounds=2000 bad=0 firstbad=-1 checksum=ok correct=yes deadlock=no time=0.0808 cpu=0.1507
mode=fixed threads=8 rounds=2000 bad=0 firstbad=-1 checksum=ok correct=yes deadlock=no time=0.1842 cpu=0.4153
mode=alt threads=1 rounds=2000 bad=0 firstbad=-1 checksum=ok correct=yes deadlock=no time=0.0018 cpu=0.0019
mode=alt threads=2 rounds=2000 bad=0 firstbad=-1 checksum=ok correct=yes deadlock=no time=0.0922 cpu=0.0784
mode=alt threads=4 rounds=2000 bad=0 firstbad=-1 checksum=ok correct=yes deadlock=no time=0.2221 cpu=0.3946
mode=alt threads=8 rounds=2000 bad=0 firstbad=-1 checksum=ok correct=yes deadlock=no time=0.6425 cpu=1.2086
```

| threads | given: correct? | given: time | given: cpu | fixed: time | fixed: cpu | alt: time | alt: cpu |
|---|---|---|---|---|---|---|---|
| 1 | yes | 0.0017 | 0.0017 | 0.0016 | 0.0017 | 0.0018 | 0.0019 |
| 2 | yes | 0.0503 | 0.0532 | 0.0471 | 0.0442 | 0.0922 | 0.0784 |
| 4 | no (stops) | 5.0085 | 0.0025 | 0.0808 | 0.1507 | 0.2221 | 0.3946 |
| 8 | no (stops) | 5.0099 | 0.0027 | 0.1842 | 0.4153 | 0.6425 | 1.2086 |

**S3.1** Reconcile with `PREDICTION.md`: quote what you predicted, say what
happened, account for the difference. If you were right, say what would have
made you wrong.

P1, Yes with 1 thread: I was on the right track, it gave the correct answer. I would have been wrong if that one thread had frozen, stuck waiting for itself.

P2, It stops with 8 threads. It froze and had to be cut off after 5 seconds, and the threads were asleep nearly the whole time. I would have been wrong if it had finished but with a wrong answer.

P3, Identical: Every run with 3, 4 and 8 threads froze in the same way. I would have been wrong if it had finished on some runs and frozen on others.

P4, timings: I had the right idea but guessed too low. For fixed with 8 threads I guessed 0.05 s and got 0.1842 s. For alt I guessed 0.1 s and got 0.6425 s. Putting threads to sleep and waking them up again takes longer than I expected, and 8 threads had to share 4 cores.

P5, fixed is fastest, alt is slowest, given freezes with 3 or more threads. With 8 threads, alt took about 3.5 times as long as fixed.

**S3.2** Which would you ship on this machine, **and what measurement would
change your mind?**

I would ship fixed. given freezes with 3 or more threads, so it is not an option. alt gives the right answer too, but it took 2 to 3.5 times as long as fixed, and with 8 threads it kept the processor about 3 times as busy.

I would change my mind if alt came out faster than fixed with 16 and 32 threads, tested five times each. I would also change my mind if fixed ever froze or gave a wrong answer.

## S4 — explain-back · 15 marks

> Two or three sentences, your own words: someone who has not seen this code
> asks *what was wrong with it, and what did fixing it cost?*

When the last thread showed up, the barrier only woke one of the waiting
threads, so with three or more the rest just slept forever. It seemed fine with
one or two threads because there's never more than one waiting. The fix was changing signal to broadcast, and the only cost is that it now wakes everyone each round instead of just one. In Short, signal wakes up one sleepy thread, broadcast wakes up all of them.

## Anything you got stuck on

Optional. One or two lines.
