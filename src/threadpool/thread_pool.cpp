#include "thread_pool.h"
#include <unistd.h>


static void thread_pool_exit_handler(void *data);
static void *thread_pool_cycle(void *data);
static int_t thread_pool_init_default(thread_pool_t *tpp, char *name);

static uint_t       thread_pool_task_id;

static int debug = 1;
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


double get_cpu_usage() {
    FILE* file;
    static long long prev_idle = 0, prev_total = 0;
    long long idle, total, diff_idle, diff_total;

    file = fopen("/proc/stat", "r");
    if (file == NULL) {
        perror("fopen");
        return -1;
    }

    char buffer[256];
    fgets(buffer, sizeof(buffer), file);
    fclose(file);

    // 解析 CPU 使用情况
    sscanf(buffer, "cpu  %*d %*d %*d %lld %*d %*d %*d %lld", &idle, &total);

    diff_idle = idle - prev_idle;
    diff_total = total - prev_total;

    prev_idle = idle;
    prev_total = total;

    return (double)(diff_total - diff_idle) / diff_total * 100;
}




pthread_mutex_t num_thread_mutex = PTHREAD_MUTEX_INITIALIZER;  // 线程池的互斥锁
int max_threads = MAX_THREADS;
int min_threads = MIN_THREADS;
volatile int stop_adjust_thread = 0; // 0表示运行，1表示停止
pthread_t adjust_tid;


void adjust_thread_pool_size(thread_pool_t* tp) {
    while (!stop_adjust_thread) {
        double cpu_usage = get_cpu_usage();  // 获取CPU利用率
        printf("Current CPU Usage: %.2f%%\n", cpu_usage);

        pthread_mutex_lock(&tp->mtx);

        if (cpu_usage > HIGH_LOAD_THRESHOLD && tp->threads < MAX_THREADS) {
            // 增加线程数
            pthread_attr_t attr;
            pthread_t tid;
            int err = pthread_attr_init(&attr);
            if (err) {
                fprintf(stderr, "pthread_attr_init() failed: %s\n", strerror(errno));
            }
            else {
                pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);  // 设置线程为分离状态

                err = pthread_create(&tid, &attr, thread_pool_cycle, tp);
                if (err) {
                    fprintf(stderr, "pthread_create() failed: %s\n", strerror(errno));
                }
                else {
                    tp->threads++;  // 增加线程数
                    printf("Increasing thread pool size to %d\n", tp->threads);
                }
            }
            pthread_attr_destroy(&attr);
        }
        else if (cpu_usage < LOW_LOAD_THRESHOLD && tp->threads > MIN_THREADS) {
            // 减少线程数，发布退出任务给最后一个线程
            thread_task_t task;
            volatile uint_t lock;

            memset(&task, '\0', sizeof(thread_task_t));

            task.handler = thread_pool_exit_handler;  // 设置退出任务的处理函数
            task.ctx = (void*)&lock;

            // 只发布退出任务给最后一个线程
            lock = 1;
            if (thread_task_post(tp, &task) != T_OK) {
                pthread_mutex_unlock(&tp->mtx);
                return;
            }

            // 等待线程完成退出任务并退出
            while (lock) {
                sched_yield();  // 让出CPU资源，确保线程退出
            }

            tp->threads--;  // 减少线程数
            printf("Decreasing thread pool size to %d\n", tp->threads);
        }

        pthread_mutex_unlock(&tp->mtx);
        sleep(ADJUST_INTERVAL);  // 每隔一段时间调整一次
    }
    printf("Adjustment thread exiting...\n");
}

thread_pool_t* thread_pool_init()
{
 //   int             err;
 //   pthread_t       tid;
 //   uint_t          n;
 //   pthread_attr_t  attr;
	//thread_pool_t   *tp=NULL;

	//tp = (thread_pool_t*)calloc(1,sizeof(thread_pool_t));

	//if(tp == NULL){
	//    fprintf(stderr, "thread_pool_init: calloc failed!\n");
	//}

	//thread_pool_init_default(tp, NULL);

 //   thread_pool_queue_init(&tp->queue);

 //   if (thread_mutex_create(&tp->mtx) != T_OK) {
	//	free(tp);
 //       return NULL;
 //   }

 //   if (thread_cond_create(&tp->cond) != T_OK) {
 //       (void) thread_mutex_destroy(&tp->mtx);
	//	free(tp);
 //       return NULL;
 //   }

 //   err = pthread_attr_init(&attr);
 //   if (err) {
 //       fprintf(stderr, "pthread_attr_init() failed, reason: %s\n",strerror(errno));
	//	free(tp);
 //       return NULL;
 //   }

 //   err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);    //设置线程为分离状态，使得线程在完成任务后会自动释放资源。
 //   if (err) {
 //       fprintf(stderr, "pthread_attr_setdetachstate() failed, reason: %s\n",strerror(errno));
	//	free(tp);
 //       return NULL;
 //   }


 //   for (n = 0; n < tp->threads; n++) {
 //       err = pthread_create(&tid, &attr, thread_pool_cycle, tp);
 //       if (err) {
 //           fprintf(stderr, "pthread_create() failed, reason: %s\n",strerror(errno));
	//		free(tp);
 //           return NULL;
 //       }
 //   }

 //   (void) pthread_attr_destroy(&attr);//线程属性对象（pthread_attr_t 类型）在不再需要时应该销毁，以释放与其关联的资源。

 //   return tp;
    return thread_pool_init_with_threads(DEFAULT_THREADS_NUM, "default");
}

thread_pool_t* thread_pool_init_with_threads(uint_t threads, const char* name)
{
    int             err;
    pthread_t       tid;
    uint_t          n;
    pthread_attr_t  attr;
    thread_pool_t* tp = (thread_pool_t*)calloc(1, sizeof(thread_pool_t));
    if (tp == NULL) {
        fprintf(stderr, "thread_pool_init: calloc failed!\n");
        return NULL;
    }

    thread_pool_queue_init(&tp->queue);

    tp->threads = threads > 0 ? threads : DEFAULT_THREADS_NUM;
    tp->max_queue = DEFAULT_QUEUE_NUM;
    tp->name = strdup(name ? name : "custom");

    if (thread_mutex_create(&tp->mtx) != T_OK ||
        thread_cond_create(&tp->cond) != T_OK) {
        free(tp);
        return NULL;
    }

    err = pthread_attr_init(&attr);
    if (err) {
        fprintf(stderr, "pthread_attr_init() failed: %s\n", strerror(errno));
        free(tp);
        return NULL;
    }

    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    for (n = 0; n < tp->threads; n++) {
        err = pthread_create(&tid, &attr, thread_pool_cycle, tp);
        if (err) {
            fprintf(stderr, "pthread_create() failed: %s\n", strerror(errno));
            free(tp);
            return NULL;
        }
    }
    // 启动调整线程池线程数的线程
    err = pthread_create(&adjust_tid, NULL, (void* (*)(void*))adjust_thread_pool_size, tp);
    if (err) {
        fprintf(stderr, "pthread_create() for adjust thread failed: %s\n", strerror(errno));
        free(tp);
        return NULL;
    }
    pthread_attr_destroy(&attr);

    if (debug) {
        fprintf(stderr,
            "thread_pool_init_with_threads, name: %s, threads: %u, max_queue: %d\n",
            tp->name, tp->threads, tp->max_queue);
    }
   
    return tp;
}


void thread_pool_destroy(thread_pool_t *tp)
{
    uint_t           n;
    thread_task_t    task;
    volatile uint_t  lock;//用于同步线程退出操作，volatile 确保它的值在多线程环境中实时更新。

    stop_adjust_thread = 1;
    memset(&task,'\0', sizeof(thread_task_t));

    task.handler = thread_pool_exit_handler;
    task.ctx = (void *) &lock;

    for (n = 0; n < tp->threads; n++) {
        lock = 1;

        if (thread_task_post(tp, &task) != T_OK) {//向任务队列发布退出任务
            return;
        }

        while (lock) {
            sched_yield();//让出CPU资源
        }
        //一个线程成功执行结束任务，结束一次循环

        //task.event.active = 0;
    }

    pthread_join(adjust_tid, NULL);  // 等待调整线程结束
    (void) thread_cond_destroy(&tp->cond);
    (void) thread_mutex_destroy(&tp->mtx);

	free(tp);
}


static void
thread_pool_exit_handler(void *data)
{
    uint_t *lock = (uint_t *)data;

    *lock = 0;

    pthread_exit(0);
}


thread_task_t *
thread_task_alloc(size_t size)
{
    thread_task_t  *task;
    //第一个参数表示需要分配的内存块的数量
    task = (thread_task_t *)calloc(1,sizeof(thread_task_t) + size);
    if (task == NULL) {
        return NULL;
    }

    task->ctx = task + 1;//task 是指向分配内存的首地址，task + 1 表示跳过 sizeof(thread_task_t) 的区域。

    return task;
}

void thread_task_free(thread_task_t * task)
{
	if (task)
	{
		free(task);
	}
}

/*
锁定任务队列：通过互斥锁保护队列操作，确保线程安全。

检查队列是否已满：避免任务队列溢出。

添加任务到队列尾部：更新任务队列并通知工作线程。

解锁队列并返回状态：释放锁后返回任务添加结果。
*/

int_t
thread_task_post(thread_pool_t *tp, thread_task_t *task)
{
    if (thread_mutex_lock(&tp->mtx) != T_OK) {
        return T_ERROR;
    }

    if (tp->waiting >= tp->max_queue) {
        (void) thread_mutex_unlock(&tp->mtx);

        fprintf(stderr,"thread pool \"%s\" queue overflow: %ld tasks waiting\n",
                      tp->name, tp->waiting);
        return T_ERROR;
    }

    //task->event.active = 1;

    task->id = thread_pool_task_id++;
    task->next = NULL;



    *tp->queue.last = task;
    tp->queue.last = &task->next;

    tp->waiting++;
    if (thread_cond_signal(&tp->cond) != T_OK) {
        (void)thread_mutex_unlock(&tp->mtx);
        return T_ERROR;
    }
    (void) thread_mutex_unlock(&tp->mtx);

    if(debug)fprintf(stderr,"task #%lu added to thread pool \"%s\"\n",
                   task->id, tp->name);

    return T_OK;
}


static void *
thread_pool_cycle(void *data)
{
    thread_pool_t *tp = (thread_pool_t *)data;

    int                 err;
    thread_task_t       *task;


    if(debug)fprintf(stderr,"thread in pool \"%s\" started\n", tp->name);

   

    for ( ;; ) {
        if (thread_mutex_lock(&tp->mtx) != T_OK) {
            return NULL;
        }

        
     

        while (tp->queue.first == NULL) {
            if (thread_cond_wait(&tp->cond, &tp->mtx)//释放锁，等待条件变量通知，唤醒后，线程会重新加锁继续执行。
                != T_OK)
            {
                (void) thread_mutex_unlock(&tp->mtx);
                return NULL;
            }
        }
       
        task = tp->queue.first;
        tp->queue.first = task->next;

        if (tp->queue.first == NULL) {
            tp->queue.last = &tp->queue.first;
        }
        tp->waiting--;
        if (thread_mutex_unlock(&tp->mtx) != T_OK) {
            return NULL;
        }



        if(debug) fprintf(stderr,"run task #%lu in thread pool \"%s\"\n",
                       task->id, tp->name);

        task->handler(task->ctx);

        if(debug) fprintf(stderr,"complete task #%lu in thread pool \"%s\"\n",task->id, tp->name);

        task->next = NULL;
		thread_task_free(task);
        //notify 
    }
}




static int_t
thread_pool_init_default(thread_pool_t *tpp, char *name)
{
	if(tpp)
    {
        tpp->threads = DEFAULT_THREADS_NUM;
        tpp->max_queue = DEFAULT_QUEUE_NUM;
            
        
		tpp->name = strdup(name?name:"default");
        if(debug)fprintf(stderr,
                      "thread_pool_init, name: %s ,threads: %lu max_queue: %ld\n",
                      tpp->name, tpp->threads, tpp->max_queue);

        return T_OK;
    }

    return T_ERROR;
}
