#pragma once
#include<queue>
#include<mutex> 
#include<functional>
class Task
{
    private:
    std::function<void()>m_func;
    public:
    Task()=default;
    Task(std::function<void()>);
    void operator()() const;
};
class TaskQueue{
    public:
    TaskQueue();
    ~TaskQueue();
    void AddTask(const Task&);
    void AddTask(std::function<void()>);
    Task TakeTask();
     inline int GetTaskNum() {
                std::lock_guard<std::mutex> locker(queue_mutex);
                return (int)m_queue.size();
        } // 内联函数需要在声明处实现

    private:
    std::queue<Task>m_queue;
    std::mutex queue_mutex;
};