/*
 * recordstore.h
 *
 * Created: 10/15/2013 4:46:35 PM 
 *  Author: kweekly
 */ 


#ifndef RECORDSTORE_H_
#define RECORDSTORE_H_

void recordstore_init();
void recordstore_clear();
void recordstore_insert(uint8_t * data, uint8_t len);
uint8_t * recordstore_dump(uint16_t * len);
void recordstore_start_new_block();
void recordstore_flush_sent_block();
#endif /* RECORDSTORE_H_ */