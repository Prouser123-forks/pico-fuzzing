// FreeRTOS
#include "FreeRTOS.h"
// FreeRTOS version macro (tskKERNEL_VERSION_NUMBER)
#include "task.h"

// Standard libraries
#include "pico/stdio.h"
#include "pico/stdlib.h"

// Accompanying header file
#include "main.h"

// USB FreeRTOS tasks
#include "usb/tasks.h"

#include "tasks/bulk.h"
#include "tasks/mcu_temperature.h"

// Source control information embedded at build-time
#include "git.h"

extern void *__BUILD_INCLUDES_FLASHLOADER;

int main(void)
{
    // Setup hardware (init stdio, etc.)
    prvSetupHardware();
#ifdef INCLUDES_FLASHLOADER
    printf("INCLUDES: Flashloader support!\n");
#endif

    const int has_flashloader = (int)&__BUILD_INCLUDES_FLASHLOADER;

    printf("FLASHLOADER: %i\n", has_flashloader);

    prvSerialDisplaySystemInfo();

    pvCreateBulkQueue();

    // Init some tasks!

    pvCreateUsbTasks();
    pvRegisterMcuTempTask();

    // TinyUSB demos call this after creating tasks.
    vTaskStartScheduler();

    return 0;
}

static void prvSetupHardware(void)
{
    stdio_init_all(); // init the things uwu
}

// Display hardware and firmware information over standard output.
void prvSerialDisplaySystemInfo(void)
{
    // Determine which RTOS type we are running
    const char *rtos_name;
#if (portSUPPORT_SMP == 1)
    rtos_name = "FreeRTOS SMP";
#else
    rtos_name = "FreeRTOS";
#endif
    printf("%s, %s\n", rtos_name, tskKERNEL_VERSION_NUMBER);

#if (portSUPPORT_SMP == 1) && (configNUM_CORES == 2)
    printf("Running on both cores\n");
#endif

#if (mainRUN_ON_CORE == 1)
    printf("Kernel running on core 1\n");
#else
    printf("Kernel running on core 0\n");
#endif

    if (git_IsPopulated())
    {
        printf("Commit: %s\n", git_CommitSHA1());
        printf("Commit date: %s\n", git_CommitDate());
    }
    else
    {
        printf("This image was built without source control.\n");
    }
}