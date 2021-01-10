# Notes on "Mastering FreeRTOS"

## Chapter 1: The FreeRTOS Distribution

### 1.5 Data Types and Coding Style Guide

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



## Chapter 3: Task Management

### 3.2 Task functions
- implemented as C functions
- prototype: receive void pointer, return void
- must not be allowed to return
	- no statement
	- not allowed to execute past the end of the function
	- if no longer required, should be explicitly deleted
		- a task can delete itself by calling `vTaskDelete(NULL)` (by using `NULL`, we signal that the calling task is to be deleted)

### 3.4 Creating tasks
- stack can be allocated statically or dynamically
- priority: 0 lowest, configMAX_PRIORITIES-1 highest
- task handle: can be set to NULL
- returns pdPASS / pdFAIL

- main must call `vTaskStartScheduler()` to pass the control of the program to the task scheduler and start all tasks


### 3.6 Time measurement and the tick interrupt
- time slice / tick period: determined by configTICK_RATE_HZ
- The optimal value for configTICK_RATE_HZ is dependent on the application being developed, although a value of 100 is typical.
- FreeRTOS API calls always specify time in multiples of tick periods, which are often referred to
simply as ‘ticks’
- `pdMS_TO_TICKS()` macro converts a time specified in milliseconds into a time specified in ticks
	- cannot be used if configTICK_RATE_HZ is greater than 1000


### 3.7 Expanding the 'not running' state
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


### 3.8 The idle task and the idle task hook
- idle task
	- automatically created by scheduler
	- run by the scheduler when no task is ready to run
	- has priority 0, therefore it can technically share the priority of an user-defined task (why would you do this tho)
- idle task hook functions
	- idle hook or idle callbacks are called by the idle task once per iteration of the idle task loop
	- must be called `vApplicationIdleHook(void)`
	- configUSE_IDLE_HOOK must be set to 1 for the idle hook function to be called

### 3.9 Changing the priority of a task
- `void vTaskPrioritySet( TaskHandle_t pxTask, UBaseType_t uxNewPriority );` to change the priority of a task after the scheduler has been started
	- INCLUDE_vTaskPrioritySet must be set to 1 for it to be available

### 3.10 Deleting a task
- `vTaskDelete(TaskHandle_t pxTaskToDelete)`
	- if called with NULL deletes the task that called it
	- any task can be deleted by any other task
	- function is available only when INCLUDE_vTaskDelete is set to 1
	- It is the responsibility of the idle task to free memory allocated to tasks that have since been deleted. Therefore, it is important that applications using the vTaskDelete() API function do not completely starve the idle task of all processing time

### Scheduling algorithms
- The scheduling algorithm is the software routine that decides which Ready state task to transition into the Running state.
- the algorithm can be changed using the configUSE_PREEMPTION and configUSE_TIME_SLICING configuration constants
- configUSE_TICKLESS_IDLE also affects the scheduling algorithm, as its use can result in the tick interrupt being turned off completely for extended periods. configUSE_TICKLESS_IDLE is an advanced option provided specifically for use in applications that must minimize their power consumption
- In all possible configurations the FreeRTOS scheduler will work with Round Robin scheduling
- A Round Robin scheduling algorithm does not guarantee time is shared equally between tasks of equal priority, only that Ready state tasks of equal priority will enter the Running state in turn.
- Default configuration: Fixed Priority Pre-emptive Scheduling with Time Slicing
	- Fixed priority: does not change the priority assigned to the tasks being scheduled, but also do not prevent the tasks themselves from changing their own priority, or that of other tasks.
	- Pre-emptive: 
		- immediately ‘pre-empts’ the Running state task if a task that has a priority higher than the Running state task enters the Ready state. Being pre-empted means being involuntarily (without explicitly yielding or blocking) moved out of the Running state and into the Ready state to allow a different task to enter the Running state.
		- If preemption is not used, the scheduler is said to be a cooperative scheduler. A context switch will only occur when the Running state task enters the Blocked state, or the Running state task explicitly yields (manually requests a re-schedule) by calling taskYIELD(). Tasks are never pre-empted, so time slicing cannot be used.
	- Time slicing: 
		- shares processing time between tasks of equal priority, even when the tasks do not explicitly yield or enter the Blocked state.
		- If time slicing is not used, then the scheduler will only select a new task to enter the Running state when either a higher priority task enters the Ready state, or the task in the Running state enters the Blocked or Suspended state.
- configIDLE_SHOULD_YIELD: the Idle task will yield (voluntarily give up whatever remains of its allocated time slice) on each iteration of its loop if there are other Idle priority tasks in the Ready state 


## Chapter 4: Queue Management

### 4.1 Chapter introduction and scope
‘Queues’ provide a task-to-task, task-to-interrupt, and interrupt-to-task communication mechanism.

### 4.2 Characteristics of a queue
- holds a finite number of fixed data items
- the length of the queue and the size of each data item are set when the queue is created
- normally used as FIFO. It is also possible to wirte to the front of a queue, and to overwrite data that is already at the front of a queue
- recommended when data size isn't too large: queue by copy (as opposed to by referenced)
- Any number of tasks can write to the same queue, and any number of tasks can read from the same queue. In practice it is very common for a queue to have multiple writers, but much less common for a queue to have multiple readers.
- When a task attempts to read/write from a queue, it can optionally specify a ‘block’ time. This is the time the task will be kept in the Blocked state to wait for data to be available from the queue/ for space to become available, should the queue already be empty/full.
- Queues can have multiple readers/writers, so it is possible for a single queue to have more than one task blocked on it waiting for data/for space to write. When this is the case, only one task will be unblocked when data/space becomes available. The task that is unblocked will always be the highest priority task that is waiting for data/space. If the blocked tasks have equal priority, then the task that has been waiting for data the longest will be unblocked. 
- Queues can be grouped into sets, allowing a task to enter the Blocked state to wait for data to become available on any of the queues in the set.

### 4.3 Using a queue
- `QueueHandle_t xQueueCreate( UBaseType_t uxQueueLength, UBaseType_t uxItemSize )`
	- will return NULL if there is insufficient heap RAM available for the queue to be created
- `xQueueReset()` clears the queue
- Writing
	- `BaseType_t xQueueSendToBack(QueueHandle_t xQueue, const void * pvItemToQueue, TickType_t xTicksToWait )`
	- xQueueSendToFront and xQueueSend with same interface
	- xQueueSend and xQueueSendToBack are the same function
	- interrupt safe versions: xQueueSendToBackFromISR / xQueueSendToFrontFromISR()
	- pvItemToQueue: a pointer to the data _to be copied into_ the queue. The number of bytes to be copied is determied by uxItemSize
	- xTicksToWait:
		- portMAX_DELAY will cause the task to wait indefinitely (without timing out), provided INCLUDE_vTaskSuspend is set to 1 in FreeRTOSConfig.h.
		- 0: returns immediately 
	- Return value: pdPASS/errQUEUE_FULL 

- Reading: 
	- `BaseType_t xQueueReceive( QueueHandle_t xQueue, void * const pvBuffer, TickType_t xTicksToWait );`
	

 