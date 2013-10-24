/*
 * scheduler.c
 *
 * Created: 3/15/2013 4:18:17 PM
 *  Author: kweekly
 */ 
#include "devicedefs.h"
#include "avrincludes.h"
#include "utils/scheduler.h"
#include "drivers/pcint.h"

#define MAX_EVENTS 32

typedef struct {
	uint8_t task_id;
	uint16_t time;
	void (*callback)(void);
} scheduler_event_t;

scheduler_event_t event_lists[SCHEDULER_NUM_LISTS][MAX_EVENTS];
uint8_t numevents[SCHEDULER_NUM_LISTS];
uint16_t maxtime[SCHEDULER_NUM_LISTS];

uint8_t eventpos;

extern void avr_doze(void);


ISR(TIMER1_COMPA_vect, ISR_NAKED) { // entire purpose is just to wake up the chip
	 __asm__ __volatile__ ("reti" ); 
}

void scheduler_init() {
	TCCR1A = 0; // no outputs, normal mode
	TCCR1B = 0; //clock disabled for now
	TCCR1C = 0;
	
	// setup compare register
	OCR1A = 0xFFFF;

	// enable interrupts for compare A
	TIMSK1 = 0x02;
}

void scheduler_reset() {
	memset(event_lists,0,sizeof(event_lists));
	memset(numevents,0,sizeof(numevents));
	memset(maxtime,0,sizeof(maxtime));
	eventpos = 0;
}

void scheduler_add_task(uint8_t task_list, uint8_t task_id, uint16_t time_ms, void (*cb)(void)) {
	if ( numevents[task_list] == MAX_EVENTS) {
		kputs("Task list is full!\n");
		return;
	}
	if ( TCCR1B != 0  ) {
		kputs("Cannot add tasks while scheduler is running\n");
		return;
	}
	
	uint16_t timeticks;
	
	if ( time_ms != SCHEDULER_LAST_EVENTS ) {
		#if F_CPU==8000000L
			timeticks = (uint16_t)((uint32_t)1000*time_ms / 128);
		#elif F_CPU==16384000L
			timeticks = (uint16_t)((uint32_t)10000*time_ms / 625);
		#else
			#error "F_CPU not defined!"
		#endif
		if (timeticks >= maxtime[task_list]) 
			maxtime[task_list] = timeticks;
	} else {
		timeticks = SCHEDULER_LAST_EVENTS;
	}

	int8_t pos = numevents[task_list] - 1;
	while ( pos >= 0 && event_lists[task_list][pos].time > timeticks  ) {
		event_lists[task_list][pos+1] = event_lists[task_list][pos];
		pos--;
	}
	event_lists[task_list][pos+1].task_id = task_id;
	event_lists[task_list][pos+1].time = timeticks;
	event_lists[task_list][pos+1].callback = cb;
	//printf("New Task %d %p\n", pos+1, cb);
	numevents[task_list]++;
}

void scheduler_remove_tasks(uint8_t task_list, uint8_t task_id) {
	uint8_t p1=0,p2=0;
	for ( p1 = 0; p1 < numevents[task_list]; p1++ ) {
		if ( event_lists[task_list][p1].task_id != task_id ) {
			event_lists[task_list][p2++] = event_lists[task_list][p1];
		}
	}
	numevents[task_list] = p2;
}


void _scheduler_run_tasks(uint8_t task_list ){
	while (eventpos < numevents[task_list] && event_lists[task_list][eventpos].time <= TCNT1 ) {
		//printf("Excecute Task %d %d %p\n", eventpos, event_lists[task_list][eventpos].time, event_lists[task_list][eventpos].callback);
		event_lists[task_list][eventpos].callback();
		eventpos += 1;
	}
}


void scheduler_start(uint8_t task_list) {
	if ( numevents[task_list] == 0 ) {
		kputs("Scheduler told to start, but no events\n");
	}
	//printf_P(PSTR("Start task list %d\n"),task_list);
		
	TCNT1 = 0; // start from scratch
	eventpos = 0;
	OCR1A = 0xFFFF;
	TCCR1B = 0x05; // enable TC, 1024 prescaling ( each tick is 128uS , giving total period of 8.4s (8MHz))
	
	// set up for next events to run
	while(1) {
		_scheduler_run_tasks(task_list); // run all tasks that need to be run
		if ( eventpos >= numevents[task_list] || event_lists[task_list][eventpos].time == SCHEDULER_LAST_EVENTS) {
			break;
		}
		OCR1A = event_lists[task_list][eventpos].time; // set next alarm to wake up
		avr_doze();
		pcint_check(); // other interrupt sources could have woken us up
	}
	
	while(eventpos < numevents[task_list]) {
		event_lists[task_list][eventpos].callback();
		eventpos++;
	}
	
	TCCR1B = 0;
}

