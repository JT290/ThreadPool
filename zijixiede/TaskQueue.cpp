#include "TaskQueue.hpp"

Task::Task(std::function<void()> func)
{
    m_func = std::move(func);
}
void Task::operator()() const

{
    if (m_func)
        m_func();
}
TaskQueue::TaskQueue() = default;
TaskQueue::~TaskQueue() = default;
void TaskQueue::AddTask(const Task& task)
{
    std::lock_guard<std::mutex>locker(queue_mutex);
    m_queue.push(task);
}
void TaskQueue::AddTask(std::function<void()>func)
{
      Task task(std::move(func));
    std::lock_guard<std::mutex> locker(queue_mutex);
    m_queue.push(std::move(task));
}
Task TaskQueue::TakeTask() {
    Task task;
    std::unique_lock<std::mutex> locker(queue_mutex);
    if (!m_queue.empty()) {
        task = m_queue.front();
        m_queue.pop();
    }
    return task;
}
