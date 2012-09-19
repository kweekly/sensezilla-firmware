 #ifndef CS5467_h
 #define CS5467_h

#include "Arduino.h"

#define CAL_CHANNEL_I1 1
#define CAL_CHANNEL_V1 2
#define CAL_CHANNEL_I2 4
#define CAL_CHANNEL_V2 8

#define PWR_RESET 0
#define PWR_SLEEP 1
#define PWR_WAKEUP 2
#define PWR_STANDBY 3

#define PAGE0 0
#define PAGE1 (1<<5)
#define PAGE2 (2<<5)
#define PAGE5 (5<<5)

#define CONFIG_REG     (PAGE0 | 0)

#define I1_REG         (PAGE0 | 1)
#define V1_REG         (PAGE0 | 2)
#define P1_REG         (PAGE0 | 3)
#define P1_AVG_REG         (PAGE0 | 4)
#define I1_RMS_REG         (PAGE0 | 5)
#define V1_RMS_REG         (PAGE0 | 6)

#define I2_REG         (PAGE0 | 7)
#define V2_REG         (PAGE0 | 8)
#define P2_REG         (PAGE0 | 9)
#define P2_AVG_REG         (PAGE0 | 10)
#define I2_RMS_REG         (PAGE0 | 11)
#define V2_RMS_REG         (PAGE0 | 12)

#define Q1_AVG_REG         (PAGE0 | 13)
#define Q1_REG         (PAGE0 | 14)

#define STATUS_REG     (PAGE0 | 15)

#define Q2_AVG_REG         (PAGE0 | 16)
#define Q2_REG         (PAGE0 | 17)

#define I1_PEAK_REG    (PAGE0 | 18)
#define V1_PEAK_REG    (PAGE0 | 19)
#define S1_REG    (PAGE0 | 20)
#define PF1_REG    (PAGE0 | 21)

#define I2_PEAK_REG    (PAGE0 | 22)
#define V2_PEAK_REG    (PAGE0 | 23)
#define S2_REG    (PAGE0 | 24)
#define PF2_REG    (PAGE0 | 25)

#define MASK_REG (PAGE0 | 26)
#define T_REG  (PAGE0 | 27)
#define CTRL_REG       (PAGE0 | 28)

#define E_PULSE_REG (PAGE0 | 29)
#define S_PULSE_REG (PAGE0 | 30)
#define Q_PULSE_REG  (PAGE0 | 31)
#define PAGE_REG (PAGE0 | 31)

#define I1_OFF_REG (PAGE1 | 0)
#define I1_GAIN_REG (PAGE1 | 1)
#define V1_OFF_REG  (PAGE1 | 2)
#define V1_GAIN_REG (PAGE1 | 3)
#define P1_OFF_REG (PAGE1 | 4)
#define I1_ACOFF_REG  (PAGE1 | 5)
#define V1_ACOFF_REG  (PAGE1 | 6)

#define I2_OFF_REG (PAGE1 | 7)
#define I2_GAIN_REG (PAGE1 | 8)
#define V2_OFF_REG  (PAGE1 | 9)
#define V2_GAIN_REG (PAGE1 | 10)
#define P2_OFF_REG (PAGE1 | 11)
#define I2_ACOFF_REG  (PAGE1 | 12)
#define V2_ACOFF_REG  (PAGE1 | 13)

#define PULSEWIDTH_REG (PAGE1 | 14)
#define PULSERATE_REG (PAGE1 | 15)
#define MODES_REG (PAGE1 | 16)
#define EPSILON_REG (PAGE1 | 17)
#define N_REG (PAGE1 | 19)
#define Q1_WB_REG (PAGE1 | 20)
#define Q2_WB_REG (PAGE1 | 21)
#define T_GAIN_REG (PAGE1 | 22)
#define T_OFF_REG (PAGE1 | 23)
#define T_SETTLE_REG (PAGE1 | 25)
#define LOAD_MIN_REG (PAGE1 | 26)
#define VF_RMS_REG (PAGE1 | 27)
#define G_REG (PAGE1 | 28)
#define TIME_REG (PAGE1 | 29)

#define V1_SAG_DUR_REG (PAGE2 | 0)
#define V1_SAG_LEVEL_REG  (PAGE2 | 1)
#define I1_FAULT_DUR_REG  (PAGE2 | 4)
#define I1_FAULT_LEVEL_DUR  (PAGE2 | 5)

#define V2_SAG_DUR_REG (PAGE2 | 8)
#define V2_SAG_LEVEL_REG  (PAGE2 | 9)
#define I2_FAULT_DUR_REG  (PAGE2 | 12)
#define I2_FAULT_LEVEL_DUR  (PAGE2 | 13)

#define T_MEAS_REG (PAGE5 | 26)

class CS5467
{
   public:
     CS5467(int CSpin);
     void init();
     unsigned long readreg(char address) ;
     void writereg(char address, unsigned long data);
     
     void startConversion(char continuous);
     void powerCmd(char cmd);
     void softwareReset();
     
     void calibrateDCOffset( char channel );
     void calibrateACOffset( char channel );
     
     void calibrateDCGain( char channel );
     void calibrateACGain( char channel );
     

     
     void waitUntilReady();
          
   private:
     int CSpin;
     char current_page;
     
     void changePage(char address);
};
 
 
 #endif
