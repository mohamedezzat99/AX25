/*
 ============================================================================
 Name        : AX25.c
 Author      : Mohamed Ezzat
 Version     :
 Copyright   : Your copyright notice
 Description : Form AX25 frame structure
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "ax25.h"
#include "AX25_CRC.h"

void AX25_prepareIFrame(uint8 *buffer, uint8 *info, uint32 frameSize, uint8 * ADDR, uint8 control, uint8 * padding) {
	int i;
  /* Set the length of the frame to lengthInfoField + 19 bytes (address + control + FCS + 2 flags). */

  /* Put flags at the right place in the buffer. */
  buffer[0] = buffer[frameSize-1] = 0x7E;

  /* Add the address in the buffer. */
  for(i=1; i < ADDR_LEN + ADDR_OFFSET; i++) {
    buffer[i] = ADDR[i-1];
  }
  /* Add the control byte */
  for(;i<CNTRL_OFFSET+CNTRL_LEN;i++){
	  buffer[i] = control;
  }
  /* Add the info field in the buffer. */
  for(; i < (INFO_OFFSET+INFO_MAX_SIZE); i++) {
    buffer[i] = *info;
    info++;
  }
  for(;i<PADDING_OFFSET+PADDING_LEN;i++){
	  buffer[i] = *padding;
	  padding++;
  }

  /* Calculation and insertion of the FCS in the buffer. */
  AX25_putCRC(buffer, frameSize);
}

#if 0
void AX25_prepareIFrame(TX_FRAME *frame, uint8 control) {

	frame[0] = 0x7E;
	uint8 SSID_OctetDest = 0, SSID_OctetSource = 0;
	/*--------------------------------------------------------------------------*
	 * AX.25 TX header : < Address  | Control >
	 *                   < 14 bytes |  1 byte >
	 *
	 *	 - Frame address data from page 10 in documentation
	 *   - Address : Destination = NJ7P  (+ 2 spaces), SSID
	 *   - Address : Source = N7LEM (+ 1 space) , SSID
	 *--------------------------------------------------------------------------*/

	static uint8 AX25_txAddressField[ADDR_L] = { 'N', 'J', '7', 'P', ' ', ' ',
			SSID_OctetDest, 'N', '7', 'L', 'E', 'M', ' ', SSID_OctetSource };

	/*******************************
	 *    config SSID subfield     *
	 *******************************/
	uint8 SSIDSource = 0xf;
	uint8 SSIDDest = 0xe;

	SSID_OctetSource |= (1 << 0); /* set X bit */
	SSID_OctetSource |= ((SSIDSource & 0x0F) << 1); /* insert SSID into the SSID octet */
	/* todo: insert C bit*/

	SSID_OctetDest |= (SSIDDest << 1); /* insert SSID into the SSID octet */
	/* todo: insert C bit*/

	for (uint16 i = 0; i < ADDR_L; i++) {
		frame->address[i] = AX25_txAddressField[i];
	}

	/******************************
	 *    config control field
	 ******************************/
	IframeControlField();
	SframeControlField();
	UframeControlField();

}
void printTxFrame(uint8 * tx_ptr, uint16 size) {
	for (uint16 i = 0; i < size; i++) {
		printf("%d", tx_ptr[i]);
	}
}

IframeControlField(TX_FRAME *frame) {
	frame->control = (frame->control & 0x1F) | ((NR << 5) & 0xE0); /* insert N(R) into control field */
	frame->control = (frame->control & 0xF1) | ((NS << 1) & 0x0E); /* insert N(S) into control field */
	frame->control &= ~(1 << 0); /* put zero in BIT0 of control field as in page 3 I frame */
	frame->control &= ~(1 << 4); /* clear P bit as it's not used as stated in section 6.2 */

	NR++;
	NS++;
	if (NR > 7) {
		NR = 0;
	}
	if (NS > 7) {
		NS = 0;
	}
}

SframeControlField(TX_FRAME *frame) {
	frame->control = 1; /* to initially make two LSB = 01 */
	frame->control = (frame->control & 0x1F) | ((NR << 5) & 0xE0); /* insert N(R) into control field */
	/* todo: p/f */

	if (RRFrame) {
		SSBits = 0;
	} else if (RNR) {
		SSBits = 1;
	} else if (REJ) {
		SSBits = 2;
	} else if (SREJ) {
		SSBits = 3;
	}
	frame->control = (frame->control & 0xF3) | ((SSBits << 2) & 0x0C); /* insert SS Bits into control field */

}

UframeControlField() {

#endif
