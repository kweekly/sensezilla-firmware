/*
 * PN532.c
 *
 * Created: 6/17/2013 3:07:13 PM
 *  Author: kweekly
 */ 
#include "devicedefs.h"
#ifdef USE_PN532

#include "avrincludes.h"
#include "protocol/report.h"
#include "utils/scheduler.h"

#include "devices/PN532.h"
#include "drivers/SPI.h"

#define PN532_PREAMBLE                      (0x00)
#define PN532_STARTCODE1                    (0x00)
#define PN532_STARTCODE2                    (0xFF)
#define PN532_POSTAMBLE                     (0x00)

#define PN532_HOSTTOPN532                   (0xD4)

// PN532 Commands
#define PN532_COMMAND_DIAGNOSE              (0x00)
#define PN532_COMMAND_GETFIRMWAREVERSION    (0x02)
#define PN532_COMMAND_GETGENERALSTATUS      (0x04)
#define PN532_COMMAND_READREGISTER          (0x06)
#define PN532_COMMAND_WRITEREGISTER         (0x08)
#define PN532_COMMAND_READGPIO              (0x0C)
#define PN532_COMMAND_WRITEGPIO             (0x0E)
#define PN532_COMMAND_SETSERIALBAUDRATE     (0x10)
#define PN532_COMMAND_SETPARAMETERS         (0x12)
#define PN532_COMMAND_SAMCONFIGURATION      (0x14)
#define PN532_COMMAND_POWERDOWN             (0x16)
#define PN532_COMMAND_RFCONFIGURATION       (0x32)
#define PN532_COMMAND_RFREGULATIONTEST      (0x58)
#define PN532_COMMAND_INJUMPFORDEP          (0x56)
#define PN532_COMMAND_INJUMPFORPSL          (0x46)
#define PN532_COMMAND_INLISTPASSIVETARGET   (0x4A)
#define PN532_COMMAND_INATR                 (0x50)
#define PN532_COMMAND_INPSL                 (0x4E)
#define PN532_COMMAND_INDATAEXCHANGE        (0x40)
#define PN532_COMMAND_INCOMMUNICATETHRU     (0x42)
#define PN532_COMMAND_INDESELECT            (0x44)
#define PN532_COMMAND_INRELEASE             (0x52)
#define PN532_COMMAND_INSELECT              (0x54)
#define PN532_COMMAND_INAUTOPOLL            (0x60)
#define PN532_COMMAND_TGINITASTARGET        (0x8C)
#define PN532_COMMAND_TGSETGENERALBYTES     (0x92)
#define PN532_COMMAND_TGGETDATA             (0x86)
#define PN532_COMMAND_TGSETDATA             (0x8E)
#define PN532_COMMAND_TGSETMETADATA         (0x94)
#define PN532_COMMAND_TGGETINITIATORCOMMAND (0x88)
#define PN532_COMMAND_TGRESPONSETOINITIATOR (0x90)
#define PN532_COMMAND_TGGETTARGETSTATUS     (0x8A)

#define PN532_WAKEUP                        (0x55)

#define PN532_SPI_STATREAD                  (0x02)
#define PN532_SPI_DATAWRITE                 (0x01)
#define PN532_SPI_DATAREAD                  (0x03)
#define PN532_SPI_READY                     (0x01)

#define PN532_MIFARE_ISO14443A              (0x00)

// Mifare Commands
#define MIFARE_CMD_AUTH_A                   (0x60)
#define MIFARE_CMD_AUTH_B                   (0x61)
#define MIFARE_CMD_READ                     (0x30)
#define MIFARE_CMD_WRITE                    (0xA0)
#define MIFARE_CMD_TRANSFER                 (0xB0)
#define MIFARE_CMD_DECREMENT                (0xC0)
#define MIFARE_CMD_INCREMENT                (0xC1)
#define MIFARE_CMD_STORE                    (0xC2)

// Prefixes for NDEF Records (to identify record type)
#define NDEF_URIPREFIX_NONE                 (0x00)
#define NDEF_URIPREFIX_HTTP_WWWDOT          (0x01)
#define NDEF_URIPREFIX_HTTPS_WWWDOT         (0x02)
#define NDEF_URIPREFIX_HTTP                 (0x03)
#define NDEF_URIPREFIX_HTTPS                (0x04)
#define NDEF_URIPREFIX_TEL                  (0x05)
#define NDEF_URIPREFIX_MAILTO               (0x06)
#define NDEF_URIPREFIX_FTP_ANONAT           (0x07)
#define NDEF_URIPREFIX_FTP_FTPDOT           (0x08)
#define NDEF_URIPREFIX_FTPS                 (0x09)
#define NDEF_URIPREFIX_SFTP                 (0x0A)
#define NDEF_URIPREFIX_SMB                  (0x0B)
#define NDEF_URIPREFIX_NFS                  (0x0C)
#define NDEF_URIPREFIX_FTP                  (0x0D)
#define NDEF_URIPREFIX_DAV                  (0x0E)
#define NDEF_URIPREFIX_NEWS                 (0x0F)
#define NDEF_URIPREFIX_TELNET               (0x10)
#define NDEF_URIPREFIX_IMAP                 (0x11)
#define NDEF_URIPREFIX_RTSP                 (0x12)
#define NDEF_URIPREFIX_URN                  (0x13)
#define NDEF_URIPREFIX_POP                  (0x14)
#define NDEF_URIPREFIX_SIP                  (0x15)
#define NDEF_URIPREFIX_SIPS                 (0x16)
#define NDEF_URIPREFIX_TFTP                 (0x17)
#define NDEF_URIPREFIX_BTSPP                (0x18)
#define NDEF_URIPREFIX_BTL2CAP              (0x19)
#define NDEF_URIPREFIX_BTGOEP               (0x1A)
#define NDEF_URIPREFIX_TCPOBEX              (0x1B)
#define NDEF_URIPREFIX_IRDAOBEX             (0x1C)
#define NDEF_URIPREFIX_FILE                 (0x1D)
#define NDEF_URIPREFIX_URN_EPC_ID           (0x1E)
#define NDEF_URIPREFIX_URN_EPC_TAG          (0x1F)
#define NDEF_URIPREFIX_URN_EPC_PAT          (0x20)
#define NDEF_URIPREFIX_URN_EPC_RAW          (0x21)
#define NDEF_URIPREFIX_URN_EPC              (0x22)
#define NDEF_URIPREFIX_URN_NFC              (0x23)

#define PN532_GPIO_VALIDATIONBIT            (0x80)
#define PN532_GPIO_P30                      (0)
#define PN532_GPIO_P31                      (1)
#define PN532_GPIO_P32                      (2)
#define PN532_GPIO_P33                      (3)
#define PN532_GPIO_P34                      (4)
#define PN532_GPIO_P35                      (5)

// some needed register defs
#define PN532_SFR_P3CFGA					(0xFFFC)
#define PN532_SFR_P3CFGB					(0xFFFD)

//uint8_t pn532ack[] = {0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00};
uint8_t pn532ack[] = {0x00, 0xFF, 0x00, 0xFF, 0x00};
uint8_t pn532response_firmwarevers[] = {0x00, 0xFF, 0x06, 0xFA, 0xD5, 0x03};
	
uint8_t _ss, _clk, _mosi, _miso;
uint8_t _uid[7];  // ISO14443A uid
uint8_t _uidLen;  // uid len
uint8_t _key[6];  // Mifare Classic key

// Uncomment these lines to enable debug output for PN532(SPI) and/or MIFARE related code
 #define PN532DEBUG
 #define MIFAREDEBUG

#define PN532_PACKBUFFSIZ 64
uint8_t pn532_packetbuffer[PN532_PACKBUFFSIZ];

uint8_t rfid_uid_buffer[16];

void rfid_init() {
  RFID_CSN = 1;
  spi_init(SPI_MODE0 | SPI_LSBFIRST | SPI_CLKDIV4);
  spi_transfer(0xFF); // try to flush out bad stuff
  
  _delay_ms(100);
  // not exactly sure why but we have to send a dummy command to get synced up
 /* pn532_packetbuffer[0] = PN532_COMMAND_GETFIRMWAREVERSION;
  rfid_send_command_check_ack(pn532_packetbuffer, 1);	*/
 
 uint32_t versiondata = rfid_get_firmware_version();
 if ( !versiondata ) {
	 versiondata = rfid_get_firmware_version(); // 1 retry
 }
 if ( !versiondata ) {
	 kputs("\tDidn't find board\n");
 } else {
	 printf_P(PSTR("\tFound chip PN5%02X\n"),(uint8_t)(versiondata>>24));
	 printf_P(PSTR("\tFirmware Ver. %d.%d\n"),(uint8_t)(versiondata>>16),(uint8_t)(versiondata>>8));
 }
 if (!rfid_SAMConfig()) {
	 kputs("\tError putting in SAM mode.\n");
 }
 
 kputs("Setting GPIO to OUTPUTs\n");
 
 if ( !rfid_write_register(PN532_SFR_P3CFGB, 0x30))
	kputs("\tError writing P3CFGB\n");
 
}

uint8_t rfid_passive_scan() {
  uint8_t success;
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
    
  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = rfid_read_passive_target_ID(PN532_MIFARE_ISO14443A, rfid_uid_buffer, &uidLength);
  
  if (success) {
    // Display some basic information about the card
	/*
    kputs("Found an ISO14443A card");
    printf_P(PSTR("  UID Length: %d bytes\n"),uidLength);
    kputs("  UID Value: ");
	for ( size_t c = 0; c < uidLength; c++ ) {
		printf_P(PSTR("%02X"),rfid_uid_buffer[c]);	
	}
	kputs("\n");
	*/
	return uidLength;
  }
  return 0;
}

/**************************************************************************/
/*! 
    @brief  Checks the firmware version of the PN5xx chip

    @returns  The chip's firmware version and ID
*/
/**************************************************************************/
uint32_t rfid_get_firmware_version(void) {
  uint32_t response;

  pn532_packetbuffer[0] = PN532_COMMAND_GETFIRMWAREVERSION;
  
  if (! rfid_send_command_check_ack(pn532_packetbuffer, 1))
    return 0;
  
  // read data packet
  _rfid_readspidata(pn532_packetbuffer, 12);
  
  // check some basic stuff
  if (0 != strncmp((char *)pn532_packetbuffer, (char *)pn532response_firmwarevers, 6)) {
    return 0;
  }
  
  response = pn532_packetbuffer[6];
  response <<= 8UL;
  response |= pn532_packetbuffer[7];
  response <<= 8UL;
  response |= pn532_packetbuffer[8];
  response <<= 8UL;
  response |= pn532_packetbuffer[9];

  return response;
}


/**************************************************************************/
/*! 
    @brief  Sends a command and waits a specified period for the ACK

    @param  cmd       Pointer to the command buffer
    @param  cmdlen    The size of the command in bytes 
    @param  timeout   timeout before giving up
    
    @returns  1 if everything is OK, 0 if timeout occured before an
              ACK was recieved
*/
/**************************************************************************/
// default timeout of one second
uint8_t rfid_send_command_check_ack(uint8_t *cmd, uint8_t cmdlen) {
	uint8_t tries = 0;
	
  while (tries++ < 3) {
 // if(tries > 1) printf_P(PSTR("Try #%d\n"),tries);
  // send ACK to abort last command
  _rfid_spi_writeack();
  
  // write the command
  _rfid_spiwritecommand(cmd, cmdlen);
  
  // Wait for chip to say its ready!
  if(!_rfid_wait_for_data()) {
//	  kputs("PN532 Failed to send ACK\n");
	  continue;
  }	  
  
  // read acknowledgement
  if (!_rfid_spi_readack()) {
	//  kputs("PN532 ACK invalid\n");
    continue;
  }
  
  // Wait for chip to say its ready!
  if(_rfid_wait_for_data()) {
	 return 1;
  }	
  
 // kputs("PN532 Failed to Send response\n");
  }
  return 0; // failed
}

/**************************************************************************/
/*! 
    Writes an 8-bit value that sets the state of the PN532's GPIO pins
    
    @warning This function is provided exclusively for board testing and
             is dangerous since it will throw an error if any pin other
             than the ones marked "Can be used as GPIO" are modified!  All
             pins that can not be used as GPIO should ALWAYS be left high
             (value = 1) or the system will become unstable and a HW reset
             will be required to recover the PN532.
    
             pinState[0]  = P30     Can be used as GPIO
             pinState[1]  = P31     Can be used as GPIO
             pinState[2]  = P32     *** RESERVED (Must be 1!) ***
             pinState[3]  = P33     Can be used as GPIO
             pinState[4]  = P34     *** RESERVED (Must be 1!) ***
             pinState[5]  = P35     Can be used as GPIO
    
    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
uint8_t rfid_write_GPIO(uint8_t pinstate) {

  // Make sure pinstate does not try to toggle P32 or P34
  pinstate |= (1 << PN532_GPIO_P32) | (1 << PN532_GPIO_P34);
  
  // Fill command buffer
  pn532_packetbuffer[0] = PN532_COMMAND_WRITEGPIO;
  pn532_packetbuffer[1] = PN532_GPIO_VALIDATIONBIT | pinstate;  // P3 Pins
  pn532_packetbuffer[2] = 0x00;    // P7 GPIO Pins (not used ... taken by SPI)

  // Send the WRITEGPIO command (0x0E)  
  if (! rfid_send_command_check_ack(pn532_packetbuffer, 3))
    return 0x0;
  
  // Read response packet (00 FF PLEN PLENCHECKSUM D5 CMD+1(0x0F) DATACHECKSUM 00)
  _rfid_readspidata(pn532_packetbuffer, 8);

  return  (pn532_packetbuffer[5] == 0x0F);
}

uint8_t rfid_write_register(uint16_t address, uint8_t val) {
	pn532_packetbuffer[0] = PN532_COMMAND_WRITEREGISTER;
	pn532_packetbuffer[1] = (address>>8);
	pn532_packetbuffer[2] = address & 0xFF;
	pn532_packetbuffer[3] = val;
	
	if (! rfid_send_command_check_ack(pn532_packetbuffer, 4))
	  return 0x0;
	  
	  // Read response packet (00 FF PLEN PLENCHECKSUM D5 CMD+1(0x0F) DATACHECKSUM 00)
	  _rfid_readspidata(pn532_packetbuffer, 8);

	  return  (pn532_packetbuffer[5] == 0x09);	
}

/**************************************************************************/
/*! 
    Reads the state of the PN532's GPIO pins
    
    @returns An 8-bit value containing the pin state where:
    
             pinState[0]  = P30     
             pinState[1]  = P31     
             pinState[2]  = P32     
             pinState[3]  = P33     
             pinState[4]  = P34     
             pinState[5]  = P35     
*/
/**************************************************************************/
uint8_t rfid_read_GPIO(void) {
  pn532_packetbuffer[0] = PN532_COMMAND_READGPIO;

  // Send the READGPIO command (0x0C)  
  if (! rfid_send_command_check_ack(pn532_packetbuffer, 1))
    return 0x0;
  
  // Read response packet (00 FF PLEN PLENCHECKSUM D5 CMD+1(0x0D) P3 P7 IO1 DATACHECKSUM 00)
  _rfid_readspidata(pn532_packetbuffer, 11);

  /* READGPIO response should be in the following format:
  
    uint8_t            Description
    -------------   ------------------------------------------
    b0..5           Frame header and preamble
    b6              P3 GPIO Pins
    b7              P7 GPIO Pins (not used ... taken by SPI)
    b8              Interface Mode Pins (not used ... bus select pins) 
    b9..10          checksum */

  return pn532_packetbuffer[6];
}

/**************************************************************************/
/*! 
    @brief  Configures the SAM (Secure Access Module)
*/
/**************************************************************************/
uint8_t rfid_SAMConfig(void) {
  pn532_packetbuffer[0] = PN532_COMMAND_SAMCONFIGURATION;
  pn532_packetbuffer[1] = 0x01; // normal mode;
  pn532_packetbuffer[2] = 0x14; // timeout 50ms * 20 = 1 second
  pn532_packetbuffer[3] = 0x01; // use IRQ pin!
  
  if (! rfid_send_command_check_ack(pn532_packetbuffer, 4))
     return 0;

  // read data packet
  _rfid_readspidata(pn532_packetbuffer, 8);
  
  return  (pn532_packetbuffer[5] == 0x15);
}

/**************************************************************************/
/*! 
    Sets the MxRtyPassiveActivation uint8_t of the RFConfiguration register
    
    @param  maxRetries    0xFF to wait forever, 0x00..0xFE to timeout
                          after mxRetries
    
    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
uint8_t rfid_set_passive_activation_retries(uint8_t maxRetries) {
  pn532_packetbuffer[0] = PN532_COMMAND_RFCONFIGURATION;
  pn532_packetbuffer[1] = 5;    // Config item 5 (MaxRetries)
  pn532_packetbuffer[2] = 0xFF; // MxRtyATR (default = 0xFF)
  pn532_packetbuffer[3] = 0x01; // MxRtyPSL (default = 0x01)
  pn532_packetbuffer[4] = maxRetries;
  
  if (! rfid_send_command_check_ack(pn532_packetbuffer, 5))
    return 0x0;  // no ACK
  
  return 1;
}

/***** ISO14443A Commands ******/

/**************************************************************************/
/*! 
    Waits for an ISO14443A target to enter the field
    
    @param  cardBaudRate  Baud rate of the card
    @param  uid           Pointer to the array that will be populated
                          with the card's UID (up to 7 bytes)
    @param  uidLength     Pointer to the variable that will hold the
                          length of the card's UID.
    
    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
uint8_t rfid_read_passive_target_ID(uint8_t cardbaudrate, uint8_t * uid, uint8_t * uidLength) {
  pn532_packetbuffer[0] = PN532_COMMAND_INLISTPASSIVETARGET;
  pn532_packetbuffer[1] = 1;  // max 1 cards at once (we can set this to 2 later)
  pn532_packetbuffer[2] = cardbaudrate;
  
  if (! rfid_send_command_check_ack(pn532_packetbuffer, 3))
    return 0x0;  // no cards read
  
  // read data packet
  _rfid_readspidata(pn532_packetbuffer, 20);
  // check some basic stuff

  /* ISO14443A card response should be in the following format:
  
    uint8_t            Description
    -------------   ------------------------------------------
    b0..6           Frame header and preamble
    b7              Tags Found
    b8              Tag Number (only one used in this example)
    b9..10          SENS_RES
    b11             SEL_RES
    b12             NFCID Length
    b13..NFCIDLen   NFCID                                      */

  if (pn532_packetbuffer[6] != 1) 
    return 0;
    
  uint16_t sens_res = pn532_packetbuffer[7];
  sens_res <<= 8;
  sens_res |= pn532_packetbuffer[9];

  /* Card appears to be Mifare Classic */
  *uidLength = pn532_packetbuffer[11];

  for (uint8_t i=0; i < pn532_packetbuffer[11]; i++) 
  {
    uid[i] = pn532_packetbuffer[12+i];

  }

  return 1;
}


/***** Mifare Classic Functions ******/

/**************************************************************************/
/*! 
      Indicates whether the specified block number is the first block
      in the sector (block 0 relative to the current sector)
*/
/**************************************************************************/
uint8_t rfid_mifareclassic_IsFirstBlock (uint32_t uiBlock)
{
  // Test if we are in the small or big sectors
  if (uiBlock < 128)
    return ((uiBlock) % 4 == 0);
  else
    return ((uiBlock) % 16 == 0);
}

/**************************************************************************/
/*! 
      Indicates whether the specified block number is the sector trailer
*/
/**************************************************************************/
uint8_t rfid_mifareclassic_IsTrailerBlock (uint32_t uiBlock)
{
  // Test if we are in the small or big sectors
  if (uiBlock < 128)
    return ((uiBlock + 1) % 4 == 0);
  else
    return ((uiBlock + 1) % 16 == 0);
}

/**************************************************************************/
/*! 
    Tries to authenticate a block of memory on a MIFARE card using the
    INDATAEXCHANGE command.  See section 7.3.8 of the PN532 User Manual
    for more information on sending MIFARE and other commands.

    @param  uid           Pointer to a uint8_t array containing the card UID
    @param  uidLen        The length (in bytes) of the card's UID (Should
                          be 4 for MIFARE Classic)
    @param  blockNumber   The block number to authenticate.  (0..63 for
                          1KB cards, and 0..255 for 4KB cards).
    @param  keyNumber     Which key type to use during authentication
                          (0 = MIFARE_CMD_AUTH_A, 1 = MIFARE_CMD_AUTH_B)
    @param  keyData       Pointer to a uint8_t array containing the 6 uint8_t
                          key value
    
    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
uint8_t rfid_mifareclassic_AuthenticateBlock (uint8_t * uid, uint8_t uidLen, uint32_t blockNumber, uint8_t keyNumber, uint8_t * keyData)
{
  uint8_t i;
  
  // Hang on to the key and uid data
  memcpy (_key, keyData, 6); 
  memcpy (_uid, uid, uidLen); 
  _uidLen = uidLen;  

  // Prepare the authentication command //
  pn532_packetbuffer[0] = PN532_COMMAND_INDATAEXCHANGE;   /* Data Exchange Header */
  pn532_packetbuffer[1] = 1;                              /* Max card numbers */
  pn532_packetbuffer[2] = (keyNumber) ? MIFARE_CMD_AUTH_B : MIFARE_CMD_AUTH_A;
  pn532_packetbuffer[3] = blockNumber;                    /* Block Number (1K = 0..63, 4K = 0..255 */
  memcpy (pn532_packetbuffer+4, _key, 6);
  for (i = 0; i < _uidLen; i++)
  {
    pn532_packetbuffer[10+i] = _uid[i];                /* 4 uint8_t card ID */
  }

  if (! rfid_send_command_check_ack(pn532_packetbuffer, 10+_uidLen))
    return 0;

  // Read the response packet
  _rfid_readspidata(pn532_packetbuffer, 12);
  // check if the response is valid and we are authenticated???
  // for an auth success it should be bytes 5-7: 0xD5 0x41 0x00
  // Mifare auth error is technically uint8_t 7: 0x14 but anything other and 0x00 is not good
  if (pn532_packetbuffer[7] != 0x00)
  {
    return 0;
  }

  return 1;
}

/**************************************************************************/
/*! 
    Tries to read an entire 16-uint8_t data block at the specified block
    address.

    @param  blockNumber   The block number to authenticate.  (0..63 for
                          1KB cards, and 0..255 for 4KB cards).
    @param  data          Pointer to the uint8_t array that will hold the
                          retrieved data (if any)
    
    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
uint8_t rfid_mifareclassic_ReadDataBlock (uint8_t blockNumber, uint8_t * data)
{
  /* Prepare the command */
  pn532_packetbuffer[0] = PN532_COMMAND_INDATAEXCHANGE;
  pn532_packetbuffer[1] = 1;                      /* Card number */
  pn532_packetbuffer[2] = MIFARE_CMD_READ;        /* Mifare Read command = 0x30 */
  pn532_packetbuffer[3] = blockNumber;            /* Block Number (0..63 for 1K, 0..255 for 4K) */

  /* Send the command */
  if (! rfid_send_command_check_ack(pn532_packetbuffer, 4))
  {
    return 0;
  }

  /* Read the response packet */
  _rfid_readspidata(pn532_packetbuffer, 26);

  /* If uint8_t 8 isn't 0x00 we probably have an error */
  if (pn532_packetbuffer[7] != 0x00)
  {
    return 0;
  }
    
  /* Copy the 16 data bytes to the output buffer        */
  /* Block content starts at uint8_t 9 of a valid response */
  memcpy (data, pn532_packetbuffer+8, 16);

  return 1;  
}

/**************************************************************************/
/*! 
    Tries to write an entire 16-uint8_t data block at the specified block
    address.

    @param  blockNumber   The block number to authenticate.  (0..63 for
                          1KB cards, and 0..255 for 4KB cards).
    @param  data          The uint8_t array that contains the data to write.
    
    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
uint8_t rfid_mifareclassic_WriteDataBlock (uint8_t blockNumber, uint8_t * data)
{
  
  /* Prepare the first command */
  pn532_packetbuffer[0] = PN532_COMMAND_INDATAEXCHANGE;
  pn532_packetbuffer[1] = 1;                      /* Card number */
  pn532_packetbuffer[2] = MIFARE_CMD_WRITE;       /* Mifare Write command = 0xA0 */
  pn532_packetbuffer[3] = blockNumber;            /* Block Number (0..63 for 1K, 0..255 for 4K) */
  memcpy (pn532_packetbuffer+4, data, 16);          /* Data Payload */

  /* Send the command */
  if (! rfid_send_command_check_ack(pn532_packetbuffer, 20))
  {
    return 0;
  }  
  _delay_ms(10);
  
  /* Read the response packet */
  _rfid_readspidata(pn532_packetbuffer, 26);

  return 1;  
}

/**************************************************************************/
/*! 
    Formats a Mifare Classic card to store NDEF Records 
    
    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
uint8_t rfid_mifareclassic_FormatNDEF (void)
{
  uint8_t sectorbuffer1[16] = {0x14, 0x01, 0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1};
  uint8_t sectorbuffer2[16] = {0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1};
  uint8_t sectorbuffer3[16] = {0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0x78, 0x77, 0x88, 0xC1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

  // Write block 1 and 2 to the card
  if (!(rfid_mifareclassic_WriteDataBlock (1, sectorbuffer1)))
    return 0;
  if (!(rfid_mifareclassic_WriteDataBlock (2, sectorbuffer2)))
    return 0;
  // Write key A and access rights card
  if (!(rfid_mifareclassic_WriteDataBlock (3, sectorbuffer3)))
    return 0;

  // Seems that everything was OK (?!)
  return 1;
}

/**************************************************************************/
/*! 
    Writes an NDEF URI Record to the specified sector (1..15)
    
    Note that this function assumes that the Mifare Classic card is
    already formatted to work as an "NFC Forum Tag" and uses a MAD1
    file system.  You can use the NXP TagWriter app on Android to
    properly format cards for this.

    @param  sectorNumber  The sector that the URI record should be written
                          to (can be 1..15 for a 1K card)
    @param  uriIdentifier The uri identifier code (0 = none, 0x01 = 
                          "http://www.", etc.)
    @param  url           The uri text to write (max 38 characters).
    
    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
uint8_t rfid_mifareclassic_WriteNDEFURI (uint8_t sectorNumber, uint8_t uriIdentifier, const char * url)
{
  // Figure out how long the string is
  uint8_t len = strlen(url);
  
  // Make sure we're within a 1K limit for the sector number
  if ((sectorNumber < 1) || (sectorNumber > 15))
    return 0;
  
  // Make sure the URI payload is between 1 and 38 chars
  if ((len < 1) || (len > 38))
    return 0;
    
  // Setup the sector buffer (w/pre-formatted TLV wrapper and NDEF message)
  uint8_t sectorbuffer1[16] = {0x00, 0x00, 0x03, len+5, 0xD1, 0x01, len+1, 0x55, uriIdentifier, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  uint8_t sectorbuffer2[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  uint8_t sectorbuffer3[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  uint8_t sectorbuffer4[16] = {0xD3, 0xF7, 0xD3, 0xF7, 0xD3, 0xF7, 0x7F, 0x07, 0x88, 0x40, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  if (len <= 6)
  {
    // Unlikely we'll get a url this short, but why not ...
    memcpy (sectorbuffer1+9, url, len);
    sectorbuffer1[len+9] = 0xFE;
  }
  else if (len == 7)
  {
    // 0xFE needs to be wrapped around to next block
    memcpy (sectorbuffer1+9, url, len);
    sectorbuffer2[0] = 0xFE;
  }
  else if ((len > 7) || (len <= 22))
  {
    // Url fits in two blocks
    memcpy (sectorbuffer1+9, url, 7);
    memcpy (sectorbuffer2, url+7, len-7);
    sectorbuffer2[len-7] = 0xFE;
  }
  else if (len == 23)
  {
    // 0xFE needs to be wrapped around to final block
    memcpy (sectorbuffer1+9, url, 7);
    memcpy (sectorbuffer2, url+7, len-7);
    sectorbuffer3[0] = 0xFE;
  }
  else
  {
    // Url fits in three blocks
    memcpy (sectorbuffer1+9, url, 7);
    memcpy (sectorbuffer2, url+7, 16);
    memcpy (sectorbuffer3, url+23, len-24);
    sectorbuffer3[len-22] = 0xFE;
  }
  
  // Now write all three blocks back to the card
  if (!(rfid_mifareclassic_WriteDataBlock (sectorNumber*4, sectorbuffer1)))
    return 0;
  if (!(rfid_mifareclassic_WriteDataBlock ((sectorNumber*4)+1, sectorbuffer2)))
    return 0;
  if (!(rfid_mifareclassic_WriteDataBlock ((sectorNumber*4)+2, sectorbuffer3)))
    return 0;
  if (!(rfid_mifareclassic_WriteDataBlock ((sectorNumber*4)+3, sectorbuffer4)))
    return 0;

  // Seems that everything was OK (?!)
  return 1;
}

/***** Mifare Ultralight Functions ******/

/**************************************************************************/
/*! 
    Tries to read an entire 4-uint8_t page at the specified address.

    @param  page        The page number (0..63 in most cases)
    @param  buffer      Pointer to the uint8_t array that will hold the
                        retrieved data (if any)
*/
/**************************************************************************/
uint8_t rfid_mifareultralight_ReadPage (uint8_t page, uint8_t * buffer)
{
  if (page >= 64)
  {
    return 0;
  }


  /* Prepare the command */
  pn532_packetbuffer[0] = PN532_COMMAND_INDATAEXCHANGE;
  pn532_packetbuffer[1] = 1;                   /* Card number */
  pn532_packetbuffer[2] = MIFARE_CMD_READ;     /* Mifare Read command = 0x30 */
  pn532_packetbuffer[3] = page;                /* Page Number (0..63 in most cases) */

  /* Send the command */
  if (! rfid_send_command_check_ack(pn532_packetbuffer, 4))
  {
    return 0;
  }
  
  /* Read the response packet */
  _rfid_readspidata(pn532_packetbuffer, 26);

  /* If uint8_t 8 isn't 0x00 we probably have an error */
  if (pn532_packetbuffer[7] == 0x00)
  {
    /* Copy the 4 data bytes to the output buffer         */
    /* Block content starts at uint8_t 9 of a valid response */
    /* Note that the command actually reads 16 uint8_t or 4  */
    /* pages at a time ... we simply discard the last 12  */
    /* bytes                                              */
    memcpy (buffer, pn532_packetbuffer+8, 4);
  }
  else
  {
    return 0;
  }

  // Return OK signal
  return 1;
}



/************** high level SPI */
void _rfid_spi_writeack() {
	RFID_CSN = 1;
	_delay_ms(1);
	RFID_CSN = 0;
	_rfid_spiwrite(PN532_SPI_DATAWRITE);
	_rfid_spiwrite(0);
	_rfid_spiwrite(0);
	_rfid_spiwrite(0xFF);
	_rfid_spiwrite(0);
	_rfid_spiwrite(0xFF);
	_rfid_spiwrite(0);
	RFID_CSN = 1;	
}

/**************************************************************************/
/*! 
    @brief  Tries to read the SPI ACK signal
*/
/**************************************************************************/
uint8_t _rfid_spi_readack() {
  uint8_t ackbuff[6];
  
  _rfid_readspidata(ackbuff, 6);
  
  return (0 == strncmp((char *)ackbuff, (char *)pn532ack, 6));
}


/************** mid level SPI */

/**************************************************************************/
/*! 
    @brief  Reads the SPI status register (to know if the PN532 is ready)
*/
/**************************************************************************/
uint8_t _rfid_wait_for_data(void) {
	uint8_t statb;
	uint16_t timer = 0;

  while(timer < 1000) {
	  RFID_CSN = 0;
	  _rfid_spiwrite(PN532_SPI_STATREAD);
	  // read uint8_t
	  statb = _rfid_spiread();
	  RFID_CSN = 1;
	  
	  if ( statb == PN532_SPI_READY) {
		  return 1; // chip is making it easy :)
	  } else if ( statb != 0)  { // check if chip is making it difficult :(
		  /*
		  for (uint8_t c = 1; c < 8; c++ ) {
			 if ( (statb ^ _BV(c)) == 0 )  {
				kputs("PN532 Misbehaving, sending some clocks\n");
				spi_disable();
				RFID_CSN = 0;
				_delay_us(1);
				for ( uint8_t d = 0; d < c; d++ ){
					EXP_SCK = 1;
					EXP_SCK = 0;
				}
				_delay_us(1);
				RFID_CSN = 1;
				spi_enable();
				break; 
			 }				 
		  }	*/		  
	  }
	  
	  
	  _delay_us(100);
	  timer += 1;
  }
  return 0;
}

/**************************************************************************/
/*! 
    @brief  Reads n bytes of data from the PN532 via SPI

    @param  buff      Pointer to the buffer where data will be written
    @param  n         Number of bytes to be read
*/
/**************************************************************************/
void _rfid_readspidata(uint8_t* buff, uint8_t n) {
  RFID_CSN = 0;
  _rfid_spiwrite(PN532_SPI_DATAREAD);

  _rfid_spiread(); // read the first 0x00 byte
  for (uint8_t i=0; i<n-1; i++) {
    buff[i] = _rfid_spiread();

  }

  RFID_CSN = 1;
}

/**************************************************************************/
/*! 
    @brief  Writes a command to the PN532, automatically inserting the
            preamble and required frame details (checksum, len, etc.)

    @param  cmd       Pointer to the command buffer
    @param  cmdlen    Command length in bytes 
*/
/**************************************************************************/
void _rfid_spiwritecommand(uint8_t* cmd, uint8_t cmdlen) {
  uint8_t checksum;

  cmdlen++;
  
  RFID_CSN = 0;
  //_delay_ms(2);     // or whatever the _delay_ms is for waking up the board
  _rfid_spiwrite(PN532_SPI_DATAWRITE);

  checksum = PN532_PREAMBLE + PN532_PREAMBLE + PN532_STARTCODE2;
  _rfid_spiwrite(PN532_PREAMBLE);
  _rfid_spiwrite(PN532_PREAMBLE);
  _rfid_spiwrite(PN532_STARTCODE2);

  _rfid_spiwrite(cmdlen);
  _rfid_spiwrite(~cmdlen + 1);
 
  _rfid_spiwrite(PN532_HOSTTOPN532);
  checksum += PN532_HOSTTOPN532;

  for (uint8_t i=0; i<cmdlen-1; i++) {
   _rfid_spiwrite(cmd[i]);
   checksum += cmd[i];
  }
  
  _rfid_spiwrite(~checksum);
  _rfid_spiwrite(PN532_POSTAMBLE);
  RFID_CSN = 1;

} 
/************** low level SPI */

/**************************************************************************/
/*! 
    @brief  Low-level SPI write wrapper

    @param  c       8-bit command to write to the SPI bus
*/
/**************************************************************************/
void _rfid_spiwrite(uint8_t c) {
 // printf_P(PSTR("> %02X\n"),c);
  spi_transfer(c);
	/*
  int8_t i;
  digitalWrite(_clk, HIGH);

  for (i=0; i<8; i++) {
    digitalWrite(_clk, LOW);
    if (c & _BV(i)) {
      digitalWrite(_mosi, HIGH);
    } else {
      digitalWrite(_mosi, LOW);
    }    
    digitalWrite(_clk, HIGH);
  }*/
}

/**************************************************************************/
/*! 
    @brief  Low-level SPI read wrapper

    @returns The 8-bit value that was read from the SPI bus
*/
/**************************************************************************/
uint8_t _rfid_spiread(void) {
  uint8_t rval = spi_transfer(0xFF);
 // printf_P(PSTR("< %02X\n"),rval);
  return rval;
	/*
  int8_t i, x;
  x = 0;
  digitalWrite(_clk, HIGH);

  for (i=0; i<8; i++) {
    if (digitalRead(_miso)) {
      x |= _BV(i);
    }
    digitalWrite(_clk, LOW);
    digitalWrite(_clk, HIGH);
  }
  return x;
  */
}
void (*detection_cb)(uint8_t * uid, uint8_t uidlen);

char last_uid_detected[sizeof(rfid_uid_buffer)];

void rfid_setup_interrupt_schedule(uint16_t starttime, void (*dcb)(uint8_t * uid, uint8_t uidlen)) {
	detection_cb = dcb;
	scheduler_add_task(SCHEDULER_MONITOR_LIST, RFID_TASK_ID, starttime, &_rfid_check_interrupt);
	memset(last_uid_detected, 0, sizeof(last_uid_detected));
}

void _rfid_check_interrupt() {
	uint8_t uidlen;
	LED1 = 1;
	uidlen = rfid_passive_scan();
	if (uidlen && detection_cb) {
		if ( memcmp(last_uid_detected, rfid_uid_buffer, uidlen) ) {
			detection_cb(rfid_uid_buffer, uidlen);
			memcpy(last_uid_detected, rfid_uid_buffer, uidlen);
		} else {
			// already detected
		}			
	} else {
		memset(last_uid_detected, 0, sizeof(last_uid_detected));
	}
	LED1 = 0;
}


#endif