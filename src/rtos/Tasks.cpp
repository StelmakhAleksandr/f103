#include "Tasks.h"

Task::Task(const TaskHandle_t& handle)
    : m_handle(handle)
{
}

const TaskHandle_t& Task::handle() const
{
    return m_handle;
}

class TaskWrapper
{
public:
    TaskWrapper(const std::function<void()>& function)
        : m_function(function)
    {
    }

    void execute()
    {
        if (m_function) {
            m_function();
        }
    }

private:
    std::function<void()> m_function;
};

void TaskFunctionWrapper(void* pvParameters)
{
    TaskWrapper* wrapper = static_cast<TaskWrapper*>(pvParameters);
    wrapper->execute();
    delete wrapper;
    vTaskDelete(NULL);
}

std::shared_ptr<Task> makeTask(const std::function<void()>& function, const std::string& title, TaskStackSize stackSize, TaskPriority priority)
{
    TaskHandle_t handle;
    auto wrapper = new TaskWrapper(function);
    xTaskCreate(
        TaskFunctionWrapper,
        title.c_str(),
        static_cast<configSTACK_DEPTH_TYPE>(stackSize),
        wrapper,
        static_cast<UBaseType_t>(priority),
        &handle);
    return std::make_shared<Task>(handle);
}
