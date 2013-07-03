/*
 * scheduler.h
 *
 * Created: 3/15/2013 4:18:08 PM
 *  Author: kweekly
 */ 


#ifndef SCHEDULER_H_
#define SCHEDULER_H_
// uses TC1 for high-speed events

void scheduler_init();
void scheduler_start(); 

#define SCHEDULER_LAST_EVENTS 0xFFFF
void scheduler_add_task(uint8_t task_id, uint16_t time_ms, void (*cb)(void));
void scheduler_remove_tasks(uint8_t task_id);

void _scheduler_run_tasks();
#endif /* SCHEDULER_H_ */