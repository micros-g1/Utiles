//
// Created by lisan on 6/9/2019.
//
#define MAX_QUEUE_LENGTH    10
#define MAX_DATA_LENGTH 100


typedef struct{
    char data[MAX_DATA_LENGTH];
}fsm_event_t;

typedef struct {
    fsm_event_t buffer[MAX_QUEUE_LENGTH];
    int head;
    int tail;
    int count;
}fsm_event_queue;

void init_event_queue();
void push_event(fsm_event_t event);
void pop_event(fsm_event_t * event);

