/*
 * USF.c - Universal Sensor Framework
 *
 * Created: 7/3/2013 2:26:55 PM
 *  Author: kweekly
 */
 #include "devicedefs.h"

#include "avrincludes.h"
static FILE mystdout = FDEV_SETUP_STREAM(uart_putc, NULL, _FDEV_SETUP_WRITE);

#include "drivers/uart.h"
#include "drivers/i2cmaster.h"
#include "drivers/rtctimer.h"
#include "drivers/pcint.h"
#include "drivers/SPI.h"
#include "drivers/power.h"

#include "device_headers.h"

#include "utils/scheduler.h"
#include "protocol/report.h"
#include "protocol/sensor_packet.h"
#include "protocol/logic.h"

#include "motes/xbee.h"

// board-specific logic
void board_power_down_devices(void);
void board_init_devices(void);
void board_setup_reporting(void);


int main(void)
{
	char wdrst = 0;
	uint8_t mcusr;
	mcusr = MCUSR;
	if ( mcusr & _BV(WDRF) ) {
		wdrst = 1;
	}	
	MCUSR = 0;
	wdt_disable();
	
	cli();	
	LED1 = 1;
	uart_init(UART_BAUD_SELECT_DOUBLE_SPEED(115200,F_CPU));
	stdout = &mystdout;
	sei();
	
	if ( wdrst ) {
		kputs("\n***WATCHDOG RESET***\n");
	}
	
	wdt_enable(WDTO_8S);
	wdt_reset();
	kputs("Setting DDR registers\n");
	DDRA = DDRA_SETTING;
	DDRB = DDRB_SETTING;
	DDRC = DDRC_SETTING;
	DDRD = DDRD_SETTING;
	DIDR0 = 0;
	DIDR1 = 0;
	
	kputs("Initializing PCINT\n");
	pcint_init();
	
	kputs("Initializing Reports\n");
	report_init();
	
	board_init_devices();
	
	kputs("Initializing wireless mote\n");
	wdt_reset();
	xbee_init();

	
	/*
	PIR_VCC = 1;
	while(1) {
		if(PIR_OUT_PIN) {
			kputs("ON\n");
		} else {
			kputs("OFF\n");
		}
		_delay_ms(50);
	}*/
	
	kputs("Powering down all devices\n");
	board_power_down_devices();
	
	kputs("Init logic subsystem\n");
	logic_init();
	
	/*
	kputs("Turning on always-on devices\n");
	pir_wake();
	accel_wake();
	*/

	board_setup_reporting();
	
	kputs("Starting RTC clock\n");
	rtctimer_init();
	rtctimer_set_periodic_alarm(report_interval_needed(),&rtc_timer_cb);
	wdt_reset();
	wdt_disable();
    while(1)
    {
		wdt_enable(WDTO_2S);
		wdt_reset();
		pcint_check();
		rtctimer_check_alarm();
		wdt_disable();
		
		#ifdef USE_PN532
			rtc_timer_cb(); // causes monitor list to be run
		#else
		#if defined LOW_POWER // do we have a RTC clock and battery-powered
			avr_sleep();
		#else
			avr_doze();
		#endif
		#endif
    }
}

// Only used if power-saving is needed
void board_power_down_devices(void) {
#ifdef ENVIRONMENT_SENSOR
	LED1 = 0;
	LED2 = 0;
	
	light_sleep(); 
	gyro_sleep();
	accel_sleep(); 
	humid_sleep();
	pir_sleep(); 
	xbee_sleep();	
#endif
}


void board_init_devices(void) {
#if defined(ENVIRONMENT_SENSOR)	
	kputs("Initializing i2c\n");
	i2c_init();
	
	kputs("Initializing Devices... Light,");
	light_init();
	kputs("Gyro, ");
	gyro_init();
	kputs("Accell, ");
	accel_init();
	kputs("Humid, ");
	humid_init();
	kputs("PIR\n");
	pir_init();	
#elif defined POWER_STRIP_MONITOR
	kputs("Initializing Devices... Power\n");
	powermon_init();	
#endif

#ifdef USE_PN532
	kputs("Initializing RFID reader... \n");
	rfid_init();
#endif

#ifdef USE_MACHXO2_PMCO2
	kputs("Initializing CO2 and PM extension...\n");
	machxo2_init();
#endif
}

void board_setup_reporting(void) {
	kputs("Initializing Scheduler and tasks\n");
	scheduler_init();
	cmd_configure_sensor_cb(0, DEFAULT_FIELDS_TO_REPORT, DEFAULT_SAMPLE_INTERVAL); // this initiates scheduler	
}
