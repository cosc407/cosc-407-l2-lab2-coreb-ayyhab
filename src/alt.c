/* Lab 2, core B -- THE ALTERNATIVE named in BRIEF.md.
 *
 * The two-turnstile barrier, out of counting semaphores: arrive, count, and
 * when the last one arrives open the first turnstile for everybody; then
 * leave, count down, and when the last one leaves open the second.
 * (Downey, "The Little Book of Semaphores", 3.6-3.7, and class 6.)
 *
 * TWO turnstiles, not one. Work out for yourself what a fast thread does to a
 * single-turnstile version before you decide the second one is decoration --
 * and say so in S2.3, because it is the same lesson as your fix.
 *
 * my_sem_init / my_sem_wait / my_sem_post / my_sem_destroy are declared in
 * include/mysem.h and implemented, correctly, in src/mysem_ref.c. A
 * pthread_mutex_t is fine for the counter.
 *
 * It is correct, and it is not expected to be fast. Do not tune it: it is
 * evidence, not a submission.
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "barrier.h"
#include "mysem.h"

/* TODO: the barrier's state. What has to be shared between the threads, and
 *       what does each thread have to remember for itself? */
typedef struct {
    pthread_mutex_t lock;     /* protects count                          */
    int             n;        /* how many threads have to arrive         */
    int             count;    /* threads between the two turnstiles      */
    my_sem_t        in;       /* first turnstile: opens when all arrived */
    my_sem_t        out;      /* second turnstile: opens when all left   */
} bar_t;

static void *create(int nthreads)
{
    /* TODO: allocate it, initialise everything, and return it. Anything a
     *       thread might lock or wait on has to be ready BEFORE the first
     *       thread can reach it. */
    bar_t *b = malloc(sizeof *b);
    if (b == NULL) {
        return NULL;
    }
    if (pthread_mutex_init(&b->lock, NULL) != 0) {
        fprintf(stderr, "barrier init failed\n");
        free(b);
        return NULL;
    }
    if (my_sem_init(&b->in, 0) != 0) {
        fprintf(stderr, "barrier init failed\n");
        pthread_mutex_destroy(&b->lock);
        free(b);
        return NULL;
    }
    if (my_sem_init(&b->out, 0) != 0) {
        fprintf(stderr, "barrier init failed\n");
        my_sem_destroy(&b->in);
        pthread_mutex_destroy(&b->lock);
        free(b);
        return NULL;
    }
    b->n     = nthreads;
    b->count = 0;
    return b;
}

static void wait_(void *p)
{
    /* TODO: the barrier. Write the invariant you are keeping in a comment
     *       above it, in one line, before you write the code -- your report
     *       and your oral both ask you to state it. */
    bar_t *b = (bar_t *)p;
    pthread_mutex_lock(&b->lock);
    b->count++;
    if (b->count == b->n) {
        for (int i = 0; i < b->n; i++) {
            my_sem_post(&b->in);      
        }
    }
    pthread_mutex_unlock(&b->lock);
    my_sem_wait(&b->in);              

    pthread_mutex_lock(&b->lock);
    b->count--;
    if (b->count == 0) {
        for (int i = 0; i < b->n; i++) {
            my_sem_post(&b->out);     
        }
    }
    pthread_mutex_unlock(&b->lock);
    my_sem_wait(&b->out);   

}

static void destroy(void *p)
{
    /* TODO: release what create() took. Every thread has been joined by the
     *       time this is called. */
    bar_t *b = (bar_t *)p;
    my_sem_destroy(&b->out);
    my_sem_destroy(&b->in);
    pthread_mutex_destroy(&b->lock);
    free(b);
}

const bar_ops_t bar_alt = { "alt", create, wait_, destroy };
