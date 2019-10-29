//
// Created by lisan on 6/9/2019.
//
#include "circular_buffer.h"

fsm_event_queue queue;

void init_event_queue(){
    queue.tail = queue.head = queue.count = 0;
}

void push_event(fsm_event_t event){
    if(MAX_QUEUE_LENGTH == queue.count)
        return;
    queue.buffer[queue.head] = event;
    queue.count++;
    queue.head = (queue.head + 1) % MAX_QUEUE_LENGTH;
}

void pop_event(fsm_event_t * event)
{
    if(queue.count == 0)
        return;
    queue.count--;
    event = queue.buffer[queue.tail];
    queue.tail = (queue.tail + 1) % MAX_QUEUE_LENGTH;
}

