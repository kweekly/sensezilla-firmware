/*
 * PN532.h
 *
 * Much copied from Adafruit's PN532 Project: http://learn.adafruit.com/adafruit-pn532-rfid-nfc/overview
 * Github: https://github.com/adafruit/Adafruit-PN532
 *
 * Created: 6/17/2013 3:07:23 PM
 *  Author: kweekly
 */ 


#ifndef PN532_H_
#define PN532_H_


void rfid_init();

uint8_t rfid_passive_scan();


// Generic PN532 functions
uint8_t rfid_SAMConfig(void);
uint32_t rfid_get_firmware_version(void);
uint8_t rfid_send_command_check_ack(uint8_t *cmd, uint8_t cmdlen);
uint8_t rfid_write_GPIO(uint8_t pinstate);
uint8_t rfid_read_GPIO(void);
uint8_t rfid_set_passive_activation_retries(uint8_t maxRetries);
  
// ISO14443A functions
uint8_t rfid_read_passive_target_ID(uint8_t cardbaudrate, uint8_t * uid, uint8_t * uidLength);
  
// Mifare Classic functions
uint8_t rfid_mifareclassic_IsFirstBlock (uint32_t uiBlock);
uint8_t rfid_mifareclassic_IsTrailerBlock (uint32_t uiBlock);
uint8_t rfid_mifareclassic_AuthenticateBlock (uint8_t * uid, uint8_t uidLen, uint32_t blockNumber, uint8_t keyNumber, uint8_t * keyData);
uint8_t rfid_mifareclassic_ReadDataBlock (uint8_t blockNumber, uint8_t * data);
uint8_t rfid_mifareclassic_WriteDataBlock (uint8_t blockNumber, uint8_t * data);
uint8_t rfid_mifareclassic_FormatNDEF (void);
uint8_t rfid_mifareclassic_WriteNDEFURI (uint8_t sectorNumber, uint8_t uriIdentifier, const char * url);
  
// Mifare Ultralight functions
uint8_t rfid_mifareultralight_ReadPage (uint8_t page, uint8_t * buffer);
  
uint8_t _rfid_spi_readack();
void _rfid_spi_writeack();
uint8_t _rfid_wait_for_data(void);
void _rfid_readspidata(uint8_t* buff, uint8_t n);
void _rfid_spiwritecommand(uint8_t* cmd, uint8_t cmdlen);

void _rfid_spiwrite(uint8_t c);
uint8_t _rfid_spiread(void);

void rfid_setup_interrupt_schedule(uint16_t starttime, void (*dcb)(uint8_t * uid, uint8_t uidlen));
void _rfid_check_interrupt();

#endif /* PN532_H_ */