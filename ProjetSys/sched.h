#include <sys/sysinfo.h>
#include <pthread.h>

struct scheduler
{
    int nb_thread;// nbre de threads
    int q_len;// nbre maximum de tache dans la pile
    struct  Stack* stk;// ma pile
    pthread_t *thread ;// tableau de threads
    pthread_mutex_t mutex_;
    pthread_cond_t cond_;

};

typedef void (*taskfunc)(void*, struct scheduler *);//prototype de tache

struct tskfunc
{
    void* cls;
    taskfunc  tsk;
};

struct Stack
{
  //l nombre d'element du tablau
    int top;
    //la capacité du tableau
    unsigned capacity;
    //tableau de tache
    struct tskfunc* array[];
};

struct info
{ struct scheduler *s;
  //tskfunc funct;
};

static inline int sched_default_threads()
{
    return  get_nprocs();
    //sysconf(_SC_NPROCESSORS_ONLN);
}

int sched_init(int nthreads, int qlen, taskfunc f, void *closure);
int sched_spawn(taskfunc f, void *closure, struct scheduler *s);
