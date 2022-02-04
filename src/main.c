/*
 * main.c
 *
 *  Created on: Oct 8, 2021
 *      Author: Mohamed Ezzat
 */

#include <stdlib.h>
#include <stdio.h>

#include "ax25.h"

uint8 buffer[AX25_FRAME_MAX_SIZE];
uint8 info[INFO_LEN];
uint8 addr[ADDR_LEN] = { 'O', 'N', '4', 'U', 'L', 'G', 0x60, 'O', 'U', 'F',
		'T', 'I', '1', 0x61 };



uint8 flag_SSP_to_Control = EMPTY;
uint8 flag_Control_to_Framing = EMPTY;
uint8 flag_Control_to_SSP = EMPTY;
uint8 flag_Deframing_to_Control = EMPTY;

uint8 g_infoSize;


extern uint8 flag_TX;
extern uint8 flag_RX;
extern uint8 flag_busy;


int main() {
	uint8 AddressReadyFlag = EMPTY;
	uint8 ControlReadyFlag = EMPTY;
	uint8 infoReadyFlag = EMPTY;
	uint8 FCSReadyFlag = EMPTY;


	uint8 control;
	//	uint8 addr[ADDR_LEN] = { "ON4ULG", 0x60, 'O', 'U', 'F', 'T', 'I', '1', 0x61};

	uint16 frameSize = 0;

	//	uint8 NR=0;

	if (infoReadyFlag == EMPTY) {
		AX25_getInfo(info);
		infoReadyFlag = FULL;
	}

	if ((flag_SSP_to_Control == FULL && flag_Control_to_Framing == EMPTY) || (flag_Control_to_SSP == EMPTY && flag_Deframing_to_Control == FULL) ) {
		 AX25_Manager(&control); /* TX call */
		 // check for ack first
		 //		flag_SSP_to_Control = EMPTY;
		 //		flag_Control_to_Framing = FULL;


//		flag_Control_to_SSP = FULL;
//		flag_Deframing_to_Control = EMPTY;
	}

	if (flag_Control_to_Framing == FULL) {
		AX25_buildFrame(buffer, info, &frameSize, addr, control, g_infoSize);
		flag_TX = CLEAR;
		infoReadyFlag = EMPTY;
	}

	for (int i = 0; i < frameSize; ++i) {
		printf("%x", buffer[i]);
	}
	printf("\n frame size in bytes is: %d\n", frameSize);

	if (flag_Deframing_to_Control == EMPTY) {
		AX25_deFrame(buffer, frameSize, g_infoSize);
		flag_RX = CLEAR;
	}
}
