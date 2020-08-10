# Chapter 1: The FreeRTOS Distribution

## 1.5 Data Types and Coding Style Guide

- Data types
	- TickType_t: se usa para contar tiempo, 32 bits
	- BaseType_t: para pdTRUE/pdFALSE por ejemplo, 32 bits

- Variable names
	- Prefixed with their type
		- c: int8 (char)
		- s: int16 (short)
		- l (small L): int32 (long)
		- v: void
		- x: BaseType_t and any other non-standard types (task handles, queue handles...)
	- If unsigned, also prefixed with 'u' ('uc', 'ul')
	- If pointer, also prefixed with 'p' ('pv', 'px')
- Function names
	- first letters: type they return
	- first word (camel): file where they're defined
	- examples:
		- `vTaskPrioritySet()`: returns void, defined in task.c
		- `xQueueReceive()`: returns a variable of type BaseType_t and is defined within queue.c
		- `pvTimerGetTimerID()`: returns a pointer to void and is defined within timers.c
	- prv: prefix for file scope (private) functions

- Macro names
	- written in upper case
	- prefixed with lower case letters that indicate where the macro is defined
	- example: `configUSE_PREEMPTION`, defined in FreeRTOSConfig.h



# Chapter 3: Task Management

## 3.2 Task functions
- implemented as C functions
- prototype: receive void pointer, return void
- must not be allowed to return
	- no statement
	- not allowed to execute past the end of the function
	- if no longer required, should be explicitly deleted
		- a task can delete itself by calling `vTaskDelete(NULL)` (by using `NULL`, we signal that the calling task is to be deleted)

## 3.4 Creating tasks
- stack can be allocated statically or dynamically
- priority: 0 lowest, configMAX_PRIORITIES-1 highest
- task handle: can be set to NULL
- returns pdPASS / pdFAIL

- main must call `vTaskStartScheduler()` to pass the control of the program to the task scheduler and start all tasks


## 3.6 Time measurement and the tick interrupt
- time slice / tick period: determined by configTICK_RATE_HZ
- The optimal value for configTICK_RATE_HZ is dependent on the application being developed, although a value of 100 is typical.
- FreeRTOS API calls always specify time in multiples of tick periods, which are often referred to
simply as ‘ticks’
- `pdMS_TO_TICKS()` macro converts a time specified in milliseconds into a time specified in ticks
	- cannot be used if configTICK_RATE_HZ is greater than 1000


## 3.7 Expanding the 'not running' state
- a continuous processing task can only be created at the very lowest priority, otherwise they prevent tasks of lower priority ever running at all
- since this is not very useful, tasks must be event-driven
- Blocked state
	- waiting for an event
	- a task can be waiting for a time-related (such as a timer expiring) or a syncronization event (such as data arriving on a queue)
	- It is possible for a task to block on a synchronization event with a timeout, effectively blocking on both types of event simultaneously. For example, a task may choose to wait for a maximum of 10 milliseconds for data to arrive on a queue. The task will leave the Blocked state if either data arrives within 10 milliseconds, or 10 milliseconds pass with no data arriving.
	- example timer: 
		- `void vTaskDelay(TickType_t xTicksToDelay)` 
		- `void vTaskDelayUntil(TickType_t * pxPreviousWakeTime, TickType_t xTimeIncrement)`
			- `pxPreviousWakeTime`: must be initialized with the current tick count before it is used for the first time, after that updated but the DelayUntil function
- Suspended state
	- task not available to the scheduler
	- vTaskSuspend() / vTaskResume() or vTaskResumeFromISR() to enter/leave the suspended state
- Ready state
	- able to run but waiting for its turn, another task is being executed


## 3.8 The idle task and the idle task hook
- idle task
	- automatically created by scheduler
	- run by the scheduler when no task is ready to run
	- has priority 0, therefore it can technically share the priority of an user-defined task (why would you do this tho)
- idle task hook functions
	- idle hook or idle callbacks are called by the idle task once per iteration of the idle task loop
	- must be called `vApplicationIdleHook(void)`
	- configUSE_IDLE_HOOK must be set to 1 for the idle hook function to be called

## 3.9 Changing the priority of a task
- `void vTaskPrioritySet( TaskHandle_t pxTask, UBaseType_t uxNewPriority );` to change the priority of a task after the scheduler has been started
	- INCLUDE_vTaskPrioritySet must be set to 1 for it to be available

## 3.10 Deleting a task
- `vTaskDelete(TaskHandle_t pxTaskToDelete)`
	- if called with NULL deletes the task that called it
	- any task can be deleted by any other task
	- function is available only when INCLUDE_vTaskDelete is set to 1
	- It is the responsibility of the idle task to free memory allocated to tasks that have since been deleted. Therefore, it is important that applications using the vTaskDelete() API function do not completely starve the idle task of all processing time