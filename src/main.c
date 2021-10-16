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
	uint8 control=0xC0;
	uint8 buffer[INFO_LEN + PADDING_LEN + FRAME_SIZE_WITHOUT_INFO_AND_PADDING];
	uint16 frameSize = INFO_LEN + PADDING_LEN + FRAME_SIZE_WITHOUT_INFO_AND_PADDING;
	uint8 info[INFO_LEN];
	uint8 infoData=0;
	for(int i = 0; i<INFO_LEN;i++){
		info[i]=infoData++;
	}
	uint8 addr[ADDR_LEN] = { 'O', 'N', '4', 'U', 'L', 'G', 0x60, 'O', 'U',
			'F', 'T', 'I', '1', 0x61};
	uint8 padding[PADDING_LEN]= {0xAA};
	AX25_prepareIFrame(buffer, info, frameSize, addr, control, padding);
	for (int i = 0; i < frameSize; ++i) {
		printf("%x", buffer[i]);
	}
}
