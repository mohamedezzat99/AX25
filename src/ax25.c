/*
 ============================================================================
 Name        : AX25.c
 Author      : Mohamed, 7ares, Nasser.
 Version     :
 Copyright   : Your copyright notice
 Description : Form AX25 frame structure
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "ax25.h"
#include "AX25_CRC.h"

void AX25_buildFrame(uint8 *buffer, uint8 *info, uint16 *frameSize, uint8 *ADDR,
		uint8 control, uint8 *padding) {
	uint16 i;

	/* Put flags at the right place in the buffer. */
	buffer[0] = 0x7E;

	/* Add the address in the buffer. */
	for (i = 1; i < ADDR_LEN + ADDR_OFFSET; i++) {
		buffer[i] = ADDR[i - 1];
	}
	/* Add the control byte */
	for (; i < CNTRL_OFFSET + CNTRL_LEN; i++) {
		buffer[i] = control;
	}
	/* Add the info field in the buffer. */
	for (; i < (INFO_OFFSET + INFO_LEN); i++) {
		buffer[i] = *info;
		info++;
	}
	for (; i < PADDING_OFFSET + PADDING_LEN; i++) {
		buffer[i] = *padding;
		padding++;
	}

	/* Calculation and insertion of the FCS in the buffer. */
	AX25_putCRC(buffer, &i);
	buffer[i] = 0x7E;
	*frameSize = i + 1;
}

/* TODO:remind Eng. Ahmed to make bit-stuffing mask */

//AX25_deFrame(uint8 * buffer, uint16 size){ /* note that array must be new */
//copy the above layer buffer to this buffer, with size;
//newbuffer[size];
//for(){
//	newbuffer[i] = buffer[i];
//}
//
//}
void AX25_deFrame(uint8 *buffer, uint16 size) {
	uint8 newbuffer[size], recived_adrress[ADDR_LEN],
			control_recived[CNTRL_LEN], info_reciver[INFO_MAX_LEN],
			padding_recived[PADDING_LEN];
	uint16 crc;
	uint16 j;
	uint8 *ptrz;
	ptrz = (uint8*) &crc;
	uint16 i = 0;
	for (; i < AX25_FRAME_MAX_SIZE; i++) {
		newbuffer[i] = buffer[i];
	}

	if (newbuffer[0] == 0x7E) {
		for (i = 1, j = 0; i < ADDR_LEN + ADDR_OFFSET; i++, j++) {
			recived_adrress[j] = newbuffer[i];
		}
		for (j = 0; i < CNTRL_LEN + CNTRL_OFFSET; i++, j++) {
			control_recived[j] = newbuffer[i];
		}

		for (j = 0; i < INFO_MAX_LEN + INFO_OFFSET; i++, j++) {
			info_reciver[j] = newbuffer[i];
		}
		for (j = 0; i < PADDING_LEN + PADDING_OFFSET; i++, j++) {
			padding_recived[j] = newbuffer[i];
		}

		crc = computeCRC(newbuffer, &i);
		if (*ptrz == newbuffer[i]) {
			i++;
			ptrz++;
			if (*ptrz == newbuffer[i]) {
				i++;
				printf("**received frame**\n");
				if (newbuffer[i] == 0x7E) {
					printf("flag=: %x", newbuffer[i]);
					printf("\naddress:\n");
					for (i = 0; i < ADDR_LEN; i++) {
						//printf("address[%d]=%x\t", i, recived_adrress[i]);

						printf("%x", recived_adrress[i]);
					}
					printf("\ncontrol byte\n");
					for (i = 0; i < CNTRL_LEN; i++) {
						printf("control[%d]=%x\t", i, control_recived[i]);
					}
					printf("\nreceived information\n");
					printf("\ninfo\n");
					for (i = 0; i < INFO_MAX_LEN; i++) {
						//	printf("info[%d]=%x\t", i, info_reciver[i]);
						printf("%x", info_reciver[i]);
					}
					printf("\nvariable padding\n");
					printf("\npadding\n");
					for (i = 0; i < PADDING_LEN; i++) {
						//printf("padding[%d]=%x\t",i,padding_recived[i]);

						printf("%x", padding_recived[i]);
					}
					printf("\nFCS\n");
//					ptrz--;
//					printf("FCS[0]=%x\t", *ptrz);
//					ptrz++;
//					printf("FCS[1]=%x\t", *ptrz);
					printf("\nCRC = %x\n", crc);
					printf("\nflag = %x", newbuffer[++i]); /*TODO: fix here cause it prints bytes in reverse order*/

				}
			}
		}
	}
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
