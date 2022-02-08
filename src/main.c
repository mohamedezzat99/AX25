/*
 * main.c
 *
 *  Created on: Oct 8, 2021
 *      Author: Mohamed Ezzat
 */

#include <stdlib.h>
#include <stdio.h>

#include "ax25.h"

uint8 SerialTXBuffer[AX25_FRAME_MAX_SIZE];
uint8 info[SSP_FRAME_MAX_SIZE];
uint8 addr[ADDR_LEN] = { 'O', 'N', '4', 'U', 'L', 'G', 0x60, 'O', 'U', 'F',
		'T', 'I', '1', 0x61 };



uint8 flag_SSP_to_Control = EMPTY;
uint8 flag_Control_to_Framing = EMPTY;
uint8 flag_Control_to_SSP = EMPTY;
uint8 flag_Deframing_to_Control = EMPTY;
uint8 flag_SerialTXBuffer = EMPTY;

uint8 g_infoSize = 236; //temp set as 236


extern uint8 flag_TX;
extern uint8 flag_RX;
extern uint8 flag_busy;


int main() {
	uint8 AddressReadyFlag = EMPTY;
	uint8 ControlReadyFlag = EMPTY;
//	uint8 infoReadyFlag = EMPTY;
	uint8 FCSReadyFlag = EMPTY;


	uint8 control;
	//	uint8 addr[ADDR_LEN] = { "ON4ULG", 0x60, 'O', 'U', 'F', 'T', 'I', '1', 0x61};

	uint16 frameSize = 0;

	//	uint8 NR=0;
	fflush(stdout);
while(1){
	if (flag_SSP_to_Control == EMPTY) {
		 AX25_getInfo(info);
		 flag_SSP_to_Control = FULL;
	}

	if ((flag_SSP_to_Control == FULL && flag_Control_to_Framing == EMPTY) || (flag_Control_to_SSP == EMPTY && flag_Deframing_to_Control == FULL) ) {
		 AX25_Manager(&control);
		 // check for ack first
		 //		flag_SSP_to_Control = EMPTY;


//		flag_Control_to_SSP = FULL;
//		flag_Deframing_to_Control = EMPTY;
	}

	if (flag_Control_to_Framing == FULL && flag_SerialTXBuffer == EMPTY) {
		AX25_buildFrame(SerialTXBuffer, info, &frameSize, addr, control, g_infoSize);
	}

	for (int i = 0; i < frameSize; ++i) {
		printf("%x", SerialTXBuffer[i]);
	}
	printf("\n frame size in bytes is: %d\n", frameSize);

//	if (flag_Deframing_to_Control == EMPTY) {
//		AX25_deFrame(SerialTXBuffer, frameSize, g_infoSize);
//	}

//AX25_buildFrame();

}
}
