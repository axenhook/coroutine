#ifndef __CO_COROUTINE_H__
#define __CO_COROUTINE_H__

#define CO_DESC(str)  1

struct co_scheduler;
typedef struct co_scheduler co_scheduler_t;

struct co_coroutine;
typedef struct co_coroutine co_coroutine_t;

typedef void *(*co_func_t)(void *);

co_scheduler_t *co_create_scheduler(void);
void            co_destroy_scheduler(co_scheduler_t *);

int  create_coroutine(co_scheduler_t *, co_func_t, void *);
int  resume_coroutine(void);
void yield_coroutine(const char *func, int line);

#define co_create(func, arg) create_coroutine(co_sched_self(), func, arg)
#define co_yield()           yield_coroutine(__func__, __LINE__)
#define co_resume()          resume_coroutine()

void  co_set_sched(co_scheduler_t *);
int   co_get_co_num(co_scheduler_t *);
int   co_get_sched_num(void);

co_scheduler_t *co_sched_self(void);
int             co_sched_self_id(void);

co_coroutine_t *co_self(void);
int             co_self_id(void);

typedef int co_spin_t;

static inline void co_spin_lock_init(co_spin_t *lock) {
    *lock = 0;
}

static inline void co_spin_lock(co_spin_t *lock) {
    while (1) {
        if (__sync_bool_compare_and_swap(lock, 0, 1) == 0) {
            return;
        }
    }
}

static inline void co_spin_unlock(co_spin_t *lock) {
    __sync_fetch_and_sub(lock, 1);
}

#if CO_DESC("semaphore for coroutine")

struct co_sem {
    int             cnt;
    co_coroutine_t *co;
};

typedef struct co_sem co_sem_t;

int  coroutine_sem_up(co_sem_t *);
int  coroutine_sem_down(co_sem_t *sem, const char *func, int line);

int     co_sem_init(co_sem_t *, int);
#define co_sem_up(sem)   coroutine_sem_up((sem));
#define co_sem_down(sem) coroutine_sem_down((sem), __func__, __LINE__);
int     co_sem_destroy(co_sem_t *);

#endif

#if CO_DESC("barrier for coroutine")

struct co_barrier {
    int               cnt;
    int               num;
    co_spin_t         lock;
    co_coroutine_t   *co;
};

typedef struct co_barrier co_barrier_t;

int coroutine_barrier_wait(co_barrier_t *barrier, const char *func, int line);

int     co_barrier_init(co_barrier_t *barrier, unsigned count);
int     co_barrier_destroy(co_barrier_t *barrier);
#define co_barrier_wait(barrier) coroutine_barrier_wait(barrier, __func__, __LINE__)

#endif

#endif
