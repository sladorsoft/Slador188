#include "MultiIO.h"

//#include <stdlib.h>
//#include <pico/stdlib.h>
#include <FreeRTOS.h>
#include <task.h>


extern "C" 
{
    /**
     * @brief FreeRTOS stack overflow hook
     * 
     * Needs to be defined if configCHECK_FOR_STACK_OVERFLOW != 0 in FreeRTOSConfig.h
     * 
     * @param xTask Handle to the task
     * @param pcTaskName Task name
     */
    void vApplicationStackOverflowHook(TaskHandle_t xTask, char* pcTaskName)
    {
        panic("Stack overflow. Task: %s\n", pcTaskName);
    }

    /**
     * @brief FreeRTOS malloc failed hook
     * 
     * Needs to be defined if configUSE_MALLOC_FAILED_HOOK != 0 in FreeRTOSConfig.h
     */
    void vApplicationMallocFailedHook()
    {
        panic("malloc failed");
    }

    
    // The next 3 functions implement FreeRTOS heap using the existing C runtime
    // instead of using one of the FreeRTOS built-in implementations of the heap.
    // Since we have a proper C runtime heap, there is no point of using a separate one
    // for FreeRTOS. In this case the "configTOTAL_HEAP_SIZE" setting has no meaning.
    // These functions can be removed, but then one of the built-in FreeRTOS heap implementations
    // need to be linked in. The default one is "FreeRTOS-Kernel-Heap4".
    /**
     * @brief FreeRTOS "malloc()" implementation
     * 
     * @param xWantedSize Number of bytes to allocate
     * @return void* Pointer to the beginning of newly allocated memory
     */
    void* pvPortMalloc(size_t xWantedSize)
    {
        return malloc(xWantedSize);
    }

    /**
     * @brief FreeRTOS "calloc()" implementation
     * 
     * @param xNum Number of objects
     * @param xSize Size of each object
     * @return void* Pointer to the beginning of newly allocated memory
     */
    void* pvPortCalloc(size_t xNum, size_t xSize)
    {
        return calloc(xNum, xSize);
    }

    /**
     * @brief FreeRTOS "free()" implementation
     * 
     * @param pv Pointer to the memory to deallocate
     */
    void vPortFree(void* pv)
    {
        free(pv);
    }
}

/**
 * @brief The entry point
 */
int main()
{
#ifndef NDEBUG
    stdio_init_all();
#endif

    DBG_PRINT("Initialising devices...\n");
    MultiIO::InitDevices();    

    DBG_PRINT("Starting FreeRTOS...\n");
    vTaskStartScheduler();

    panic_unsupported();
}
