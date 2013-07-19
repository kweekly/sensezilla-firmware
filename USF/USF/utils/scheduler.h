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
void scheduler_reset();
void scheduler_start(uint8_t task_list); 

#define SCHEDULER_LAST_EVENTS 0xFFFF
void scheduler_add_task(uint8_t task_list, uint8_t task_id, uint16_t time_ms, void (*cb)(void));
void scheduler_remove_tasks(uint8_t task_list, uint8_t task_id);

void _scheduler_run_tasks(uint8_t task_list);

#define SCHEDULER_NUM_LISTS				2
#define SCHEDULER_PERIODIC_SAMPLE_LIST  0
#define SCHEDULER_MONITOR_LIST			1

#endif /* SCHEDULER_H_ */