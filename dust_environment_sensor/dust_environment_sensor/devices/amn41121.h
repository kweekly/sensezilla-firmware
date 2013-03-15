/*
 * amn41121.h
 *
 * Created: 3/12/2013 5:49:54 PM
 *  Author: kweekly
 */ 


#ifndef AMN41121_H_
#define AMN41121_H_

#define pir_init() 
#define pir_sleep() { PIR_VCC = 0; }
void pir_wake(void);
#define pir_read() (PIR_OUT_PIN)





#endif /* AMN41121_H_ */