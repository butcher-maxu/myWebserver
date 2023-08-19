#ifndef LOCKER_H
#define LOCKER_H

#include <exception>
#include <pthread.h>
#include <semaphore.h>

// 线程同步机制封装类

// 互斥锁类---实现互斥访问
class locker {
  public:
    locker() {
        if (pthread_mutex_init(&m_mutex, NULL) != 0) {
            throw std::exception();
        }
    }

    ~locker() { pthread_mutex_destroy(&m_mutex); }

    // 加锁
    bool lock() { return pthread_mutex_lock(&m_mutex) == 0; }

    // 解锁
    bool unlock() { return pthread_mutex_unlock(&m_mutex) == 0; }

    // 取得锁
    pthread_mutex_t *get() { return &m_mutex; }

  private:
    pthread_mutex_t m_mutex;
};

// 条件变量类--多线程的同步和通信： 等待特定条件满足或者通知其他线程条件已经满足
// 有数据 无数据
class cond {
  public:
    cond() {
        if (pthread_cond_init(&m_cond, NULL) != 0) {
            throw std::exception();
        }
    }
    ~cond() { pthread_cond_destroy(&m_cond); }

    /*等待函数： 让线程进入等待状态，直到其他线程调用相应的 signal 或者
    broadcast, 唤醒等待中的线程 ， 等待期间传入一个互斥锁，用于保护条件的访问*/
    bool wait(pthread_mutex_t *m_mutex) {
        int ret = 0;
        ret = pthread_cond_wait(&m_cond, m_mutex);
        return ret == 0;
    }

    /* 超时等待函数，等待一段指定的时间。如果在指定的时间内条件还未满足，线程就会唤醒*/
    bool timewait(pthread_mutex_t *m_mutex, struct timespec t) {
        int ret = 0;
        ret = pthread_cond_timedwait(&m_cond, m_mutex, &t);
        return ret == 0;
    }

    // 从所有等待中的线程中唤醒一个等待的线程
    bool signal() { return pthread_cond_signal(&m_cond) == 0; }

    // 唤醒所有等待的线程
    bool broadcast() { return pthread_cond_broadcast(&m_cond) == 0; }

  private:
    pthread_cond_t m_cond;
};

// 信号量类--多线程(多进程)同步机制， 控制对共享资源的访问
class sem {
  public:
    sem() {
        if (sem_init(&m_sem, 0, 0) != 0) {
            throw std::exception();
        }
    }

    sem(int num) {
        if (sem_init(&m_sem, 0, num) != 0) {
            throw std::exception();
        }
    }

    ~sem() { sem_destroy(&m_sem); }

    // 等待信号量  对信号量进行减操作，if m_sem > 0, 操作成功， 返回true，
    // else if m_sem == 0 ,阻塞线程，返回false, 直到其他线程增加该值
    bool wait() { return sem_wait(&m_sem) == 0; }

    // 增加信号量
    bool post() { return sem_post(&m_sem) == 0; }

  private:
    sem_t m_sem;
};

#endif