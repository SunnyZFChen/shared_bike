#ifndef _THREAD_POOL_H_INCLUDED_
#define _THREAD_POOL_H_INCLUDED_

#ifdef __cpplusplus
extern "C" {
#endif // __cpplusplus

#include "thread.h"

#define DEFAULT_THREADS_NUM 14
#define DEFAULT_QUEUE_NUM  65535

#define MAX_THREADS 100  // 最大线程数
#define MIN_THREADS 10   // 最小线程数
#define HIGH_LOAD_THRESHOLD 70  // CPU使用率高于此值时增加线程数
#define LOW_LOAD_THRESHOLD 30   // CPU使用率低于此值时减少线程数
#define ADJUST_INTERVAL 2       // 动态调整的时间间隔（秒）

	typedef unsigned long         atomic_uint_t;
	typedef struct thread_task_s  thread_task_t;
	typedef struct thread_pool_s  thread_pool_t;

	/*任务的结构体如下，由待处理的任务，任务标识id，任务上下文，任务处理函数组成，*/
	struct thread_task_s {
		thread_task_t       *next;
		uint_t               id;
		void                *ctx;
		void(*handler)(void *data);
	};
	//任务队列
	typedef struct {
		thread_task_t        *first;
		thread_task_t        **last;
	} thread_pool_queue_t;

#define thread_pool_queue_init(q)                                         \
    (q)->first = NULL;                                                    \
    (q)->last = &(q)->first



	struct thread_pool_s {
		pthread_mutex_t        mtx;			//互斥锁，用于锁定任务队列，避免竞争访问
		thread_pool_queue_t   queue;		//任务队列
		int_t                 waiting;		//等待处理的任务数
		pthread_cond_t         cond;		//条件变量，用于通知线程池有任务处理
		char* name;							//线程池名字
		uint_t                threads; 		//当前线程池中的线程数
		int_t                 max_queue;    //线程池能处理的最大任务数
	};

	thread_task_t *thread_task_alloc(size_t size);
	void thread_task_free(thread_task_t *task);
	int_t thread_task_post(thread_pool_t *tp, thread_task_t *task);
	thread_pool_t* thread_pool_init();
	thread_pool_t* thread_pool_init_with_threads(uint_t threads, const char* name);

	void thread_pool_destroy(thread_pool_t *tp);

#ifdef __cpplusplus
}
#endif

#endif /* _THREAD_POOL_H_INCLUDED_ */
