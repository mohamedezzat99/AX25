/*
 * main.c
 *
 *  Created on: Oct 8, 2021
 *      Author: Mohamed Ezzat
 */


#include <stdlib.h>
#include <stdio.h>

#include "ax25.h"


int main()
{
	uint8 infoReadyFlag = NOT_READY;
	uint8 control=0xC0;
	uint8 addr[ADDR_LEN] = { 'O', 'N', '4', 'U', 'L', 'G', 0x60, 'O', 'U',
			'F', 'T', 'I', '1', 0x61};
	uint8 infoData=0;
	uint8 buffer[AX25_FRAME_MAX_SIZE];
	uint16 frameSize=0;
	uint8 info[INFO_LEN];
	for(int i = 0; i<INFO_LEN;i++){
		info[i]=infoData++;
	}
	infoReadyFlag = 1;
	uint8 padding[PADDING_LEN]= {0xAA};
	if(infoReadyFlag == READY){
	AX25_buildFrame(buffer, info, &frameSize, addr, control, padding, &infoReadyFlag);
	}
	printf("\n info flag = %d \n", infoReadyFlag);
	for (int i = 0; i < frameSize; ++i) {
		printf("%x", buffer[i]);
	}
	printf("\n frame size in bytes is: %d" ,frameSize);
}
