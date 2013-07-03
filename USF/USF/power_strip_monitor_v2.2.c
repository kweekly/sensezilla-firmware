/*
 * power_strip_monitor_v2.c
 *
 * Created: 5/15/2013 3:31:38 PM
 *  Author: kweekly
 */ 

/* Fuse Settings:
Extended: FF
High : 91
Low D2
*/

#include <all.h>

static FILE mystdout = FDEV_SETUP_STREAM(uart_putc, NULL, _FDEV_SETUP_WRITE);

void board_power_down_devices(void);
void avr_doze(void);
void avr_sleep(void);

#define TASK_REPORTING 0x02
void task_begin_report(void);
void task_print_report(void);
void task_send_report(void);

uint8_t rx_packet_buf[128];
void status_changed_cb(uint8_t status);
void rx_cb(xbee_16b_address addr_16b, xbee_64b_address addr_64b, uint8_t rssi, uint16_t nBytes);
void tx_cb(uint8_t frame_id, uint8_t status);

xbee_64b_address dest64;
xbee_16b_address dest16;
enum{
	BROADCAST,
	SEND64,
	SEND16
} dest_mode;


void turn_on_gate(char gateno);
void turn_off_gate(char gateno);
void toggle_gate(char gateno);

/* PERIPHERAL RESOURCES
Timer 0 : unused
Timer 1 : Scheduler
Timer 2 : CS5467 clock
Timer 3 : RTC

SPI : CS5467
I2C : unused

USART0 : Console/Arduino bootloader
USART1 : XBee
*/


int main(void)
{
	char wdrst = 0;
	
	if ( MCUSR & _BV(WDRF) ) {
		wdrst = 1;
		MCUSR = 0;
	}	
	wdt_disable();

	DDRA = DDRA_SETTING;
	DDRB = DDRB_SETTING;
	DDRC = DDRC_SETTING;
	DDRD = DDRD_SETTING;
	DIDR0 = 0;
	DIDR1 = 0;
	
	cli();	
	LED1 = 1;
	uart_init(UART_BAUD_SELECT_DOUBLE_SPEED(115200,F_CPU));
	console_init();
	stdout = &mystdout;
	sei();
	
	if ( wdrst ) {
		kputs("\n***WATCHDOG RESET***\n");
	}

	kputs("Initializing PCINT\n");
	pcint_init();
		
	kputs("Initializing Reports\n");
	report_init();
	
	kputs("Initializing Devices... ");
	powermon_init();
	
	
	kputs("Initializing wireless mote\n");
	dest_mode = BROADCAST;
	xbee_init();
	xbee_set_modem_status_callback(&status_changed_cb);
	xbee_set_tx_status_callback(&tx_cb);
	xbee_set_rx_callback(&rx_cb,rx_packet_buf);
	
	kputs("Initialize interrupts\n");
	
	kputs("Initializing Scheduler and tasks\n");
	scheduler_init();
	
	scheduler_add_task(TASK_REPORTING, 0, &task_begin_report);
	powermon_setup_reporting_schedule(0);
	//scheduler_add_task(MOTE_TASK_ID, 1, &xbee_wake);
	//scheduler_add_task(TASK_REPORTING, SCHEDULER_LAST_EVENTS, &task_print_report);
	scheduler_add_task(TASK_REPORTING, SCHEDULER_LAST_EVENTS, &task_send_report);
	scheduler_add_task(TASK_REPORTING, SCHEDULER_LAST_EVENTS, &report_poplast);
	//scheduler_add_task(MOTE_TASK_ID, SCHEDULER_LAST_EVENTS, &xbee_sleep);
	
	
	kputs("Starting RTC clock\n");
	rtctimer_init();
	rtctimer_set_periodic_alarm(1,&scheduler_start);
	
    while(1)
    {
		wdt_enable(WDTO_250MS);
		wdt_reset();
		pcint_check();
		rtctimer_check_alarm();
		console_check();		
		wdt_disable();
		avr_sleep();
    }
}



void task_begin_report(void) {
	report_new(rtctimer_read());	
}

void task_print_report(void) {
	report_print_human(report_current());
}

void task_send_report(void) {
	uint8_t packetbuf[128];
	xbee_tick();
	uint16_t len = report_populate_real(report_current(),packetbuf);
	if (dest_mode == BROADCAST) {
		xbee_send_packet_64(XBEE_BROADCAST_64b_ADDR,len,packetbuf,XBEE_TX_OPTION_BROADCAST_PAN);
	} else if ( dest_mode == SEND16 ){
		xbee_send_packet_16(dest16,len,packetbuf,0);
	} else if (dest_mode == SEND64) {
		xbee_send_packet_64(dest64,len,packetbuf,0);
	}
	xbee_tick();
}

void status_changed_cb(uint8_t status) {
	printf_P(PSTR("Modem status is now %d\n"),status);
}

void rx_cb(xbee_16b_address addr_16b, xbee_64b_address addr_64b, uint8_t rssi, uint16_t nBytes) {

	if ( nBytes == 0 ) return;
	
	uint32_t * timeptr;
	switch(rx_packet_buf[0]){
		case 0x01:
			timeptr = (uint32_t *)(&(rx_packet_buf[1]));
			printf_P(PSTR("Time is now: %ld\n"),*timeptr);
			rtctimer_write(*timeptr);
			if ( addr_16b != XBEE_UNKNOWN_16b_ADDR ) {
				dest_mode = SEND16;
				dest16 = addr_16b;
			} else if ( addr_64b != XBEE_UNKNOWN_64b_ADDR ) {
				dest_mode = SEND64;
				dest64 = addr_64b;
			}
			break;
		default:
			printf_P(PSTR("Unknown remote command %02X\n"),rx_packet_buf[0]);
	}
}
void tx_cb(uint8_t frame_id, uint8_t status){
	//printf_P(PSTR("Last TX message had error code %d\n"),status);
}

void avr_doze(void) {
	// go into idle mode
	set_sleep_mode(SLEEP_MODE_IDLE);
	sleep_enable();
	sleep_cpu();
	sleep_disable();
}

void avr_sleep(void) {
	uart_flush();
	uart1_flush();
	xbee_tick();
}


void turn_on_gate(char gateno) {
	if ( gateno == 6 ) {
		GATE6 = 1;
	} else if ( gateno >= 1 && gateno <= 5 ) {
		PORTB |= _BV(gateno-1);
	} else {
		printf_P(PSTR("Error: Invalid gate number %d\n"),gateno);
	}
}

void turn_off_gate(char gateno) {
	if ( gateno == 6 ) {
		GATE6 = 0;
	} else if ( gateno >= 1 && gateno <= 5 ) {
		PORTB &= ~_BV(gateno-1);
	} else {
		printf_P(PSTR("Error: Invalid gate number %d\n"),gateno);
	}
}

void toggle_gate(char gateno) {
	if ( gateno == 6 ) {
		GATE6 = !GATE6;
	} else if ( gateno >= 1 && gateno <= 5 ) {
		PINB = _BV(gateno-1);
	} else {
		printf_P(PSTR("Error: Invalid gate number %d\n"),gateno);
	}
}
