/*
    ThreadPool();
    ThreadPool(int,int);
    ~ThreadPool();
    void AddTask(const Task&);
    int GetAliveNum();
    int GetBusyNum();
};
*/

#include "ThreadPool.hpp"
//! 构造函数
ThreadPool::ThreadPool(int min, int max)
    : m_min_num(min), m_max_num(max), m_busy_num(0), m_alive_num(min), m_taskQ(std::make_unique<TaskQueue>())
{
    m_shutdown = false;
    // 创建初始的工作线程（每个线程会在 work() 中循环等待并处理任务）
    for (int i = 0; i < m_min_num; ++i)
    {
        work_thread.emplace_back(&ThreadPool::work, this);
    }

    // 启动管理线程，用于监控并按需扩/缩容
    manager_thread = std::thread(&ThreadPool::manage, this);
}
// ！析构函数
ThreadPool::~ThreadPool()
{
    m_shutdown = true;
    not_empty.notify_all();
    for (auto &t : work_thread)
    {
        if (t.joinable())
            t.join();
    }
    if (manager_thread.joinable())
        manager_thread.join();
}
// ！工作函数
void ThreadPool::work()
{
    while (true)
    {
        Task task;
        {
            std::unique_lock<std::mutex> locker(m_mutex);
            not_empty.wait(locker, [this]()
                           { return m_shutdown || m_taskQ->GetTaskNum() > 0 || m_exit_num > 0; });
            if (m_exit_num > 0)
            {
                m_exit_num--;
                if (m_alive_num > m_min_num)
                {
                    m_alive_num--;
                    return;
                }
            }
            if (m_shutdown && m_taskQ->GetTaskNum() == 0)
            {
                break;
            }
            if (m_taskQ->GetTaskNum() > 0)
            {
                task = m_taskQ->TakeTask();
                m_busy_num++;
            }
            else
                continue;
        }
       task();
    m_busy_num--; 
    }
    m_alive_num--;
}
void ThreadPool::manage()
{
    while(!m_shutdown)
    {
        std::this_thread::sleep_for(std::chrono::seconds(5));
        int queue_size=m_taskQ->GetTaskNum();
        int alive=m_alive_num;
        int busy=m_busy_num;
        if(queue_size>alive&&alive<m_max_num)
        {
            int add_count=0;
            const int add_num=2;
            std::lock_guard<std::mutex>lock(m_mutex);
            for(int i=0;i<m_max_num&&add_count<add_num&&m_alive_num<m_max_num;++i)
            {
                work_thread.emplace_back(&ThreadPool::work,this);
                m_alive_num++;
                add_count++;
            }
        }
        if(m_busy_num*2<m_alive_num&&alive>m_min_num)
        {
            m_exit_num=1;
            for(int i=0;i<m_exit_num;i++)
            {
                not_empty.notify_one();
            }
        }
    }
}
int ThreadPool::GetBusyNum()
{
    return m_busy_num;
}
int ThreadPool::GetAliveNum()
{
    return m_alive_num;
} 
void ThreadPool::AddTask(const Task &task)
{
    m_taskQ->AddTask(task);
    not_empty.notify_all();
}