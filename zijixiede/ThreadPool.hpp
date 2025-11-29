#pragma once
#include<thread>
#include<vector>
#include<mutex>
#include<iostream>
#include<condition_variable>
#include<memory.h>
#include<atomic>
#include"TaskQueue.hpp"
class ThreadPool
{
    private:
    int m_min_num;//最小
    int m_max_num;//最大
    std::atomic<int>m_alive_num;//可用线程数
    std::atomic<int>m_busy_num;//在忙线程数
    std::atomic<int>m_exit_num;//推出线程数
    bool m_shutdown;
    std::thread manager_thread;//管理者线程
    std::vector<std::thread> work_thread;//!这是干嘛的？
    std::mutex m_mutex;//锁
    std::condition_variable not_empty;//条件变量
    std::unique_ptr<TaskQueue> m_taskQ;//！讲讲智能指针？
    void work();
    void manage();
    void thread_exit();

    public:
    ThreadPool();
    ThreadPool(int,int);
    ~ThreadPool();
    void AddTask(const Task&);
    int GetAliveNum();
    int GetBusyNum();
};