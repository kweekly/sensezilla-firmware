/*
 * recordstore.c
 *
 * Created: 10/15/2013 4:45:55 PM
 *  Author: kweekly
 */ 
#include "devicedefs.h"

#ifdef USE_RECORDSTORE
#include <avrincludes.h>
#include "protocol/sensor_packet.h"
#include "recordstore.h"

#define MAX_RECORD_TYPES 16
#define RECORDSTORE_MEMSIZE (37*120L)

uint8_t * first_record_pointers[MAX_RECORD_TYPES];
uint8_t first_record_pos;

uint8_t memblock_raw[RECORDSTORE_MEMSIZE];
uint8_t * memblock;
uint8_t * memblock_pos;

uint8_t * oldmemblock_end;

void recordstore_init() {
	recordstore_clear();
}

void recordstore_clear() {
	uint16_t off;
	// set default MT and BT
	off = packet_construct_recordstore_header(memblock_raw);
	memblock = memblock_raw + off;
	
	oldmemblock_end = NULL;
	memblock_pos = memblock;
	first_record_pos = 0;
	for ( size_t c = 0; c < MAX_RECORD_TYPES; c++ ) {
		first_record_pointers[c] = NULL;
	}
}

void recordstore_insert(uint8_t * data, uint8_t len) {
	size_t c;
	uint8_t bdiff[16];
	
	if ( len & 0x80 ) {
		kputs("Record size too big");
		return;
	}
	if ( (memblock_pos - memblock_raw) + len + 1 >= RECORDSTORE_MEMSIZE ) {
		kputs("Memblock overflow");
		return;
	}
	
	for ( c = 0; c < first_record_pos; c++ ) {
		if (first_record_pointers[c] != NULL && 
				(0x80 ^ *(first_record_pointers[c])) == len) {
			break;
		}
	}
	//printf_P(PSTR("Inserting %d byte record:"),len);
	if ( c == first_record_pos ) { // new first record
		first_record_pointers[c] = memblock_pos;
		memblock_pos[0] = 0x80 | len;
		memcpy(memblock_pos+1,data,len);
		memblock_pos += len + 1;
		first_record_pos++;
		//printf_P(PSTR(" New record created 1 Byte overhead"));
	} else {
		// look for differences in bytes
		uint8_t * first_record = first_record_pointers[c] + 1;
		uint8_t bdiff_size = (len + 7) / 8;
		uint8_t * data_insert = memblock_pos + bdiff_size + 1;
		uint8_t bytes_saved = 0;
		uint8_t bytes_total = bdiff_size + 1;
		
		memset(bdiff,0,sizeof(bdiff));
		for ( size_t b = 0; b < len; b++ ) {
			if ( first_record[b] != data[b] ) {
				bdiff[b/8] |= (1<<(b%8));
				*(data_insert++) = data[b];
				bytes_total++;
			} else {
				bytes_saved++;
			}
		}
		// len
		memblock_pos[0] = len;
		// bdiff
		memcpy(memblock_pos+1,bdiff,bdiff_size);
		// data already inserted
		// ready for next record
		memblock_pos = data_insert;
		//printf_P(PSTR("%d bytes overhead, %d bytes saved, %d bytes total"),1+bdiff_size,bytes_saved,bytes_total);
	}
	//printf_P(PSTR(" recordstore %d/%d bytes\n"),memblock_pos-memblock_raw,RECORDSTORE_MEMSIZE);
}

uint8_t * recordstore_dump(uint16_t * len) {
	*len = (uint16_t)(memblock_pos - memblock_raw);
	return memblock_raw;
}

void recordstore_start_new_block() {
	first_record_pos = 0;
	for ( size_t c = 0; c < MAX_RECORD_TYPES; c++ ) {
		first_record_pointers[c] = NULL;
	}	
	oldmemblock_end = memblock_pos;
}

void recordstore_flush_sent_block() {
	if (oldmemblock_end) {
		uint16_t offset =  (oldmemblock_end - memblock);
		for ( size_t c = 0; c < MAX_RECORD_TYPES; c++ ) {
			if ( first_record_pointers[c] ) {
				first_record_pointers[c] -= offset;
			}
		}
		for ( uint8_t * m = memblock; m < oldmemblock_end; m++) {
			*m = *(m+offset);
		}
		memblock_pos -= offset;
	}
}

#endif