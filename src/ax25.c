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

uint8 g_flag = 0x7E;
/*--------------------------------------------------------------------------*
 * AX.25 TX header : < Address     | Control    |  PID   >
 *                   < 14/28 bytes |  1/2 byte  | 1byte  >
 *
 *	 - Frame address data from page 10 in documentation
 *   - Address : Destination = NJ7P  (+ 2 spaces), SSID = 0x60
 *   - Address : Source = N7LEM (+ 1 space) , SSID = 0xE0
 *
 *  NB : Characters coded in standard ASCII 7-bits with LSB (HDLC extension
 *      bit) set to 0. Except for the last SSID (means that's the last byte).
 *
 *   - PID     : 0xF0 (no layer 3 protocol implemented)
 *--------------------------------------------------------------------------*/

uint8 g_PID = 0xF0;


#if(ADDR_L == 14)

static const uint8 AX25_txAddressField[ADDR_L] = { 'N' << 1, 'J' << 1, '7'
		<< 1, 'P' << 1, ' ' << 1, ' ' << 1, 0xE0, 'N' << 1, '7' << 1, 'L' << 1,
		'E' << 1, 'M' << 1, ' ' << 1, 0x61 };

#elif(ADDR_L == 28)

static const uint8 AX25_txAddressField[ADDR_L] = { 'N' << 1, 'J' << 1, '7' << 1, 'P' << 1, ' '
		<< 1, ' ' << 1, 'N' << 1, 'J' << 1, '7' << 1, 'P' << 1, ' '
		<< 1, ' ' << 1, 0xE0 , 'N' << 1, '7' << 1, 'L' << 1, 'E' << 1, 'M'
		<< 1, ' ' << 1, 'N' << 1, '7' << 1, 'L' << 1, 'E' << 1, 'M'
		<< 1, ' ' << 1, 0x61};
#endif

void AX25_prepareIFrame(TX_FRAME *frame) {
	static uint8 NR = 0;
	static uint8 NS = 0;
	frame->flag = 0x7E;
	frame->pid = 0xF0;
	/******************************
	 *    config control field
	 ******************************/
	frame->control = (frame->control & 0x1F) | ((NR << 5) & 0xE0); 	/* insert N(R) into control field */
	frame->control = (frame->control & 0xF1) | ((NS << 1) & 0x0E);	/* insert N(S) into control field */
	frame->control &=~(1<<0);										/* put zero in BIT0 of control field as in page 16 I frame */
	frame->control &=~(1<<4);										/* clear P bit as it's not used as stated in section 6.2 */

	uint16 i;
	for (i = 0; i < ADDR_L; i++) {
		frame->address[i] = AX25_txAddressField[i];
	}
	NR++;
	NS++;
	if(NR > 7 || NS > 7)
	{
		NR = 0;
		NS = 0;
	}
}
void printTxFrame(TX_FRAME *tx_ptr)
{
	int i;
	printf("flag : %x \n",tx_ptr->flag);
	printf("Address: \n");
	for(i=0; i< ADDR_L;i++)
	{
		printf("\t %x \n",tx_ptr->address[i]);
	}
	printf("control: %x\n", tx_ptr->control);
	printf("PID : %x \n", tx_ptr->pid);
	printf("flag : %x \n",tx_ptr->flag);
}























