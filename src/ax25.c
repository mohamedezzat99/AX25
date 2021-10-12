/*
 ============================================================================
 Name        : AX25.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Form AX25 frame structure
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "ax25.h"

void AX25_prepareIFrame(TX_FRAME *frame) {
	static uint8 NR = 0;
	static uint8 NS = 0;
	frame->flag = 0x7E;
	frame->pid = 0xF0; /* PID : 0xF0 (no layer 3 protocol implemented) */
	uint8 SSID_OctetDest = 0, SSID_OctetSource = 0;
	/*--------------------------------------------------------------------------*
	 * AX.25 TX header : < Address     | Control    |  PID   >
	 *                   < 14 bytes |  1 byte  | 1byte  >
	 *
	 *	 - Frame address data from page 10 in documentation
	 *   - Address : Destination = NJ7P  (+ 2 spaces), SSID
	 *   - Address : Source = N7LEM (+ 1 space) , SSID
	 *--------------------------------------------------------------------------*/

	static uint8 AX25_txAddressField[ADDR_L] = { 'N', 'J', '7', 'P', ' ', ' ',
			SSID_OctetDest, 'N', '7', 'L', 'E', 'M', ' ', SSID_OctetSource };

	/******************************
	 *    config SSID subfield
	 *******************************/
	uint8 SSIDSource = 0xf;
	uint8 SSIDDest = 0xe;

	SSID_OctetSource |= (1 << 0); /* set X bit */
	SSID_OctetSource |= ((SSIDSource & 0x0F) << 1); /* insert SSID into the SSID octet */
	/* todo: insert C bit*/

	SSID_OctetDest |= (SSIDDest << 1); /* insert SSID into the SSID octet */
	/* todo: insert C bit*/

	/******************************
	 *    config control field
	 ******************************/
	frame->control = (frame->control & 0x1F) | ((NR << 5) & 0xE0); /* insert N(R) into control field */
	frame->control = (frame->control & 0xF1) | ((NS << 1) & 0x0E); /* insert N(S) into control field */
	frame->control &= ~(1 << 0); /* put zero in BIT0 of control field as in page 16 I frame */
	frame->control &= ~(1 << 4); /* clear P bit as it's not used as stated in section 6.2 */

	uint16 i;
	for (i = 0; i < ADDR_L; i++) {
		frame->address[i] = AX25_txAddressField[i];
	}
	NR++;
	NS++;
	if (NR > 7 || NS > 7) {
		NR = 0;
		NS = 0;
	}
}
void printTxFrame(TX_FRAME *tx_ptr) {
	int i;
	printf("flag : %x \n", tx_ptr->flag);
	printf("Address: \n");
	for (i = 0; i < ADDR_L; i++) {
		printf("\t %x \n", tx_ptr->address[i]);
	}
	printf("control: %x\n", tx_ptr->control);
	printf("PID : %x \n", tx_ptr->pid);
	printf("flag : %x \n", tx_ptr->flag);
}

