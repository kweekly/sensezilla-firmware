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
#include "protocol/datalink.h"

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
	
	wdt_enable(WDTO_8S);
	wdt_reset();
	
	cli();	
	#ifdef LED1
		LED1 = 1;
	#endif
	uart_init(UART_BAUD_SELECT_DOUBLE_SPEED(115200,F_CPU));
	stdout = &mystdout;
	sei();
	
	if ( wdrst ) {
		kputs("\n***WATCHDOG RESET***\n");
	}
	
	kputs("\nSetting DDR registers\n");
	DDRA = DDRA_SETTING;
	DDRB = DDRB_SETTING;
	DDRC = DDRC_SETTING;
	DDRD = DDRD_SETTING;
	DIDR0 = 0;
	DIDR1 = 0;
	/*
	kputs("Hi i2c\n");
	i2c_init();
	kputs("Hi humid\n");
	humid_init();
	humid_sleep();
	kputs("Hi accel\n");
	accel_init();
	accel_sleep();
	kputs("Hi light\n");
	light_init();
	light_sleep();
	kputs("Hi PIR\n");
	pir_wake();
	kputs("Hi RTC\n");
	rtctimer_init();
	kputs("Go away RTC\n");
	DDRC &= 0x3F;
	PORTC |= 0xC0;
	
	kputs("Go to sleep\n");
	uart_flush();
	ACSR = (1<<ACD);
	LED1 = LED2 = 0; 
	cli();
	wdt_disable();	
	SMCR = (2<<SM0) | (1<<SE);	 //Enable Power-Down Mode
	while(1) {
		asm volatile ("sleep"); // __sleep(); // Go to sleep
	}
*/
	kputs("Initializing PCINT\n");
	pcint_init();
	
	kputs("Initializing Reports\n");
	report_init();
	
	board_init_devices();
	
	kputs("Initializing wireless mote\n");
	wdt_reset();
	datalink_init();

	
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
		//wdt_enable(WDTO_2S);
		//wdt_reset();
		pcint_check();
		rtctimer_check_alarm();
		wdt_disable();
		
		datalink_tick();
		
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
	#ifdef LED1
		LED1 = 0;
		LED2 = 0;
	#endif
	
	#ifdef REPORT_TYPE_LIGHT
		light_sleep(); 
	#endif
	#ifdef REPORT_TYPE_GYRO
		gyro_sleep();
	#endif
	#ifdef REPORT_TYPE_ACCEL
		accel_sleep(); 
	#endif
	#ifdef REPORT_TYPE_HUMID
		humid_sleep();
	#endif
	pir_sleep(); 
	datalink_sleep();	
#endif
}


void board_init_devices(void) {
#if defined(ENVIRONMENT_SENSOR)	
	kputs("Initializing i2c\n");
	i2c_init();
	
	kputs("Initializing Devices... Light,");
	light_init();
	#ifdef REPORT_TYPE_GYRO
		kputs("Gyro, ");
		gyro_init();
	#endif
	#ifdef REPORT_TYPE_ACCEL
		kputs("Accell, ");
		accel_init();
	#endif
	#ifdef REPORT_TYPE_HUMID
		kputs("Humid, ");
		humid_init();
	#endif
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

#ifdef USE_DOOR_SENSORS
	kputs("Initializing Door Sensors...\n");
	door_sensors_init();
#endif
}

void board_setup_reporting(void) {
	kputs("Initializing Scheduler and tasks\n");
	scheduler_init();
	cmd_configure_sensor_cb(0x07, DEFAULT_FIELDS_TO_REPORT, DEFAULT_SAMPLE_INTERVAL, DEFAULT_RECORDSTORE_INTERVAL); // this initiates scheduler
}
