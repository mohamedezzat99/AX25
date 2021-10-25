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
uint8 infoReadyFlag = NOT_READY;

uint8 g_NR=0;
uint8 g_NS=0;

uint8 g_VR=0;
uint8 g_VS=0;

uint8 g_pollFinal=0;

/*TODO: ask how to pass a copy of the array
 * ask for next steps
 */


/*
 * Description: computes control bit given the following parameters
 * parameters:
 * frameType: give frame type (I, S, U)
 * secondaryType: RR, RNR, REJ, SREJ, SABME, DISC, DM, UA , UI, TEST
 *
 */
uint8 AX25_getControl(frameType frameType, frameSecondaryType secondaryType){
	uint8 control=0;
	switch(frameType){
	case I:
		control = (control & 0x1F) | ((g_NR << 5) & 0xE0); 			/* insert N(R) into control field */
		control = (control & 0xF1) | ((g_NS << 1) & 0x0E); 			/* insert N(S) into control field */
		control = (control & 0xEF) | ((g_pollFinal<<4) & 0x10); 	/* insert P into control field */
		control &= ~(1 << 0); 										/* insert 0 in rightmost bit */
		break;
	case S:
		control = (control & 0x1F) | ((g_NR << 5) & 0xE0); 			/* insert N(R) into control field */
		control = (control & 0xEF) | ((g_pollFinal<<4) & 0x10); 	/* insert P/F into control field */
		control = (control & 0xFC) | 0x01;							/* insert 01 in the two rightmost bits */
		control = (control & 0xF3) | ((secondaryType << 2) & 0x0C);	/* insert S bits into their place */
		break;
	case U:
		control = (control & 0xEF) | ((g_pollFinal<<4) & 0x10); 	/* insert P/F into control field */
		control = (control & 0xFC) | 0x03;							/* insert 11 in the two rightmost bits */
		control = (control & 0x13) | (secondaryType & 0xEC);		/* insert M bits into their proper location */
		break;
	}
	return control;
}
/*
 * Description: function fills in info array
 * parameters:
 *  *info: pointer to the global info array
 */
void AX25_getInfo(uint8 * info){
	infoReadyFlag=NOT_READY;
	uint8 infoData=0;
	/*currently fill info this way */
	for(int i = 0; i<INFO_LEN;i++){
		info[i]=infoData++;
	}
	infoReadyFlag=READY;
}
int main()
{

	uint8 addr[ADDR_LEN] = { 'O', 'N', '4', 'U', 'L', 'G', 0x60, 'O', 'U',
			'F', 'T', 'I', '1', 0x61};

	uint16 frameSize=0;
	uint8 control;
	control = AX25_getControl(S,SREJ);
	uint8 padding[PADDING_LEN]= {0xAA};
	AX25_getInfo(info);
	if(infoReadyFlag == READY){
		AX25_buildFrame(buffer, info, &frameSize, addr, control, padding, &infoReadyFlag);
	}
	for (int i = 0; i < frameSize; ++i) {
		printf("%x", buffer[i]);
	}
	printf("\n frame size in bytes is: %d" ,frameSize);
}
