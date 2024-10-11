#pragma once

#include <FreeRTOS.h>
#include <task.h>


/**
 * @brief C++ wrapper for the FreeRTOS task
 * 
 * Wraps basic functionality of the FreeRTOS task.
 * The derived class needs to implement the Run() method 
 * with the task functionality.
 */
class OSTask
{
public:
    /**
     * @brief OSTask constructor
     *
     * Creates a new task and adds it to the FreeRTOS scheduler for execution.
     * 
     * @param name Name of the task
     * @param priority Priority of the task
     * @param stackDepth Stack depth of the task
     */
    OSTask(const char* name, UBaseType_t priority = tskIDLE_PRIORITY, configSTACK_DEPTH_TYPE stackDepth = configMINIMAL_STACK_SIZE)
    : m_Handle(nullptr)
    {
        taskENTER_CRITICAL();
        BaseType_t result = xTaskCreate(TaskProc, name, stackDepth, this, priority, &m_Handle);
        if (result != pdPASS)
        {
            panic("Failed to create task");
        }

        taskEXIT_CRITICAL();
    }
    
    virtual ~OSTask()
    {
        End();
    }

    /**
     * @brief Returns the task handle
     * @returns Task handle
     */
    constexpr TaskHandle_t GetHandle() const { return m_Handle; }

protected:
    /**
     * @brief Task implementation
     * 
     * This method should run in a loop until the task is no more needed.
     * When this method returns, the task is removed from the task scheduler
     * and can be no longer re-entered.
     */
    virtual void Run() = 0;

    /**
     * Forces a context switch
     */
    void Yield()
    {
        taskYIELD();
    }

    /**
     * @brief Removes the task from FreeRTOS scheduler
     * 
     */
    void End()
    {
        taskENTER_CRITICAL();
        if (m_Handle != nullptr)
        {
            vTaskDelete(m_Handle);
            m_Handle = nullptr;
        }

        taskEXIT_CRITICAL();
    }

private:
    static void TaskProc(void* params)
    {
        OSTask* thisPtr = reinterpret_cast<OSTask*>(params);
        if (thisPtr != nullptr)
        {
            thisPtr->Run();
            thisPtr->End();
        }
    }

    TaskHandle_t m_Handle;
};
