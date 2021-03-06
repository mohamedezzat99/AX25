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

uint8 g_pollFinal=0;
uint8 g_Received_NR=0;

uint8 g_received_address[ADDR_LEN], g_control_recived[CNTRL_LEN], g_info_reciver[SSP_FRAME_MAX_SIZE], g_padding_recived[PADDING_LEN];

extern 	uint8 addr[ADDR_LEN];

uint8 flag_TX;
uint8 flag_RX;
uint8 flag_busy;
uint8 flag_RX_crc; /* used in the Manager function to check whether CRC is correct or not */
uint8 flag_info = EMPTY;

extern uint8 flag_SSP_to_Control;
extern uint8 flag_Control_to_Framing;
extern uint8 flag_Control_to_SSP;
extern uint8 flag_Deframing_to_Control;
extern uint8 info[SSP_FRAME_MAX_SIZE];

extern 	uint8 g_infoSize;
extern uint8 flag_SerialTXBuffer;


/* -------------------- TX Functions --------------------*/

/*
 * Description: computes control byte given the following parameters
 * parameters:
 * frameType: give frame type (I, S, U)
 * secondaryType: RR, RNR, REJ, SREJ, SABME, DISC, DM, UA , UI, TEST
 *
 */
uint8 AX25_getControl(frameType frameType, frameSecondaryType secondaryType, uint8 NS, uint8 NR, uint8 pollFinal){
	uint8 control=0;
	switch(frameType){
	case I:
		control = (control & 0x1F) | ((NR << 5) & 0xE0); 			/* insert N(R) into control field */
		control = (control & 0xF1) | ((NS << 1) & 0x0E); 			/* insert N(S) into control field */
		control = (control & 0xEF) | ((pollFinal<<4) & 0x10); 		/* insert P into control field */
		control &= ~(1 << 0); 										/* insert 0 in rightmost bit */
		break;
	case S:
		control = (control & 0x1F) | ((NR << 5) & 0xE0); 			/* insert N(R) into control field */
		control = (control & 0xEF) | ((pollFinal<<4) & 0x10); 		/* insert P/F into control field */
		control = (control & 0xFC) | 0x01;							/* insert 01 in the two rightmost bits */
		control = (control & 0xF3) | ((secondaryType << 2) & 0x0C);	/* insert S bits into their place */
		break;
	case U:
		control = (control & 0xEF) | ((pollFinal<<4) & 0x10); 		/* insert P/F into control field */
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
	uint8 infoData=0;
	/*currently fill info this way */
	for(int i = 0; i<236;i++){
		info[i]=infoData++;
	}
}

/*
 * TESTING function
 * Description: function fills in info array
 * parameters:
 *  *info: pointer to the global info array
 */
void fillBuffer(uint8 *buffer, uint8 size){
	uint8 Data=0;
	/*currently fill buffers this way */
	for(int i = 0; i<size;i++){
		buffer[i]=Data++;
	}
}

/*
 * Description: this function acts as Control Layer
 * parameters:
 *
 */

void AX25_Manager(uint8* a_control){

	uint8 control;
	uint8 prev_state;
	uint8 i;
	uint8 SSP_to_Control_Buffer_Copy[SIZE_SSP_to_Control_Buffer];
	uint8 SSP_to_Control_Buffer[SIZE_SSP_to_Control_Buffer];
	uint8 Deframing_To_Control_Buffer[256];
	uint8 Deframing_To_Control_Buffer_Copy[256];
	static uint8 state = idle;
	uint8 pollfinal=0;
	static uint8 VS=0;
	static uint8 VR=0;
	uint8 NS;
	uint8 NR;
	uint8 Received_NS;
	uint8 PollFinal;
	uint8 Received_PollFinal;
	uint8 Sbits;
	uint8 Received_Sbits;
	uint8 Mbits;
	uint8 Received_Mbits;
	uint8 received_control;
	uint8 Address_Copy[ADDR_LEN];
	uint8 myAddress[ADDR_LEN] = { 'O', 'N', '4', 'U', 'L', 'G', 0x60, 'O', 'U', 'F',
			'T', 'I', '1', 0x61 };
	uint8 notMyAddress = CLEAR;
	uint8 Control_To_SSP[236];
	uint8 flag_Status = ACCEPT;
	uint8 g_Recieved_NR_1;



	/*---------------------------------------- 3rd trial ----------------------------------------*/
	/* TESTING code */

	fillBuffer(SSP_to_Control_Buffer, SIZE_SSP_to_Control_Buffer); /* fills buffer of SSP_TO_CONTROL */

	/* ----------END OF TESTING------------------- */
	switch(state){
	case idle:
		/*----------------------- TX part -----------------------*/
		if((flag_SSP_to_Control == FULL && flag_Control_to_Framing == EMPTY)){
			for (i = 0; i < SIZE_SSP_to_Control_Buffer; i++) {
				info[i] = SSP_to_Control_Buffer[i];
			}
			flag_SSP_to_Control = EMPTY;
			flag_Control_to_Framing = FULL;
			NS=VS;
			NR=VR;
			*a_control = AX25_getControl(S, RR, NS, NR, pollfinal); /* check from dr if it should be I frame and not S frame. */
			flag_TX = SET;
			state = TX;
		}

		/*----------------------- RX part -----------------------*/

		if((flag_Control_to_SSP == EMPTY && flag_Deframing_to_Control == FULL)){

			/* nmla el fo2o b3adha n3ml flag clear */
			/* TODO: flag_Deframing_to_Control = EMPTY; */

			for(i=0; i<ADDR_LEN; i++){
				if(g_received_address[i] != myAddress[i])
				{
					notMyAddress = SET;
					flag_Deframing_to_Control = EMPTY; /* clears Buffer in case address is not ours */
					break;
				}
			}

			if (notMyAddress != SET) {

				/* copy array to upper layer */
				for (i = 0; i < SSP_FRAME_MAX_SIZE; i++) {
					Control_To_SSP[i] = g_info_reciver[i];
				}

				flag_Deframing_to_Control = EMPTY; /* clears Buffer after copying data in it */
				state = RX;
			}
		}
		break;

	case TX:

		if(flag_Deframing_to_Control == FULL){

			received_control = g_control_recived[0];

			/* check which type I or S or U */
			if((received_control && 0x01) == 0){

				/* type is I frame */
				g_Received_NR = (received_control & 0xE0)>> 5;
				Received_NS = (received_control & 0x0E) >> 1;
				Received_PollFinal = (received_control & 0x10) >> 4; /* TODO: check if it should be Poll or PollFinal */
			}
			else if((received_control && 0x03) == 1){

				/* type is S frame */

				g_Received_NR = (received_control & 0xE0)>> 5;
				Received_PollFinal = (received_control & 0x10) >> 4;
				Received_Sbits = (received_control & 0x0C) >> 2;

				/* in case the received NR is 0 this means that we ack frames up to 7 from previous window */
				g_Recieved_NR_1 = g_Received_NR -1;
				if(g_Recieved_NR_1 > 7){
					g_Recieved_NR_1 = 7;
				}


				if((g_Recieved_NR_1) == VS && (Received_Sbits == RR || Received_Sbits == RNR)){ /* check if frame was received properly or not by other side */

					flag_Status = ACCEPT; /* this means that the frame sent was accepted */

					/* make values of VS range from 0 --> 7 only */
					if (VS < 7) {
						VS++;
					} else {
						VS = 0;
					}

					state = idle;
				}
				else{
					flag_Status = REJECT;
					state = idle;
				}
			}
			else{

				/* type is U frame */
				Received_Mbits = (received_control & 0xEC)>> 2;
				Received_PollFinal = (received_control & 0x10) >> 4;
			}
		}
		state = idle;

		break;

	case RX:

		/* Generate Required Control Byte */

		/*TODO: check if this part should be here or after the if condition above */
		NS = VS;
		if (VS < 7) {
			VS++;
		} else {
			VS = 0;
		}

		/* check on CRC flag (in deframe function) if True make RR if False make REJ */

		if(flag_RX_crc == SET){
			*a_control = AX25_getControl(S, RR, NS, g_Received_NR, pollfinal);
		} else{

			*a_control = AX25_getControl(S, REJ, NS, g_Received_NR, pollfinal);
		}


		/*------------------------------------------*/

		g_infoSize = 0; /* set info field size to 0 in S frame */

		/* Fill address array */
		for (i = 0; i < ADDR_LEN; i++) {
			addr[i] = myAddress[i];
		}


		/* */
		flag_Control_to_Framing = FULL;

		state = idle;

		break;
	}

	/*---------------------------------------- 3rd trial ----------------------------------------*/

#if 0
	/* check cond */

	if((prev_state == idle) && (flag_SSP_to_Control == FULL && flag_Control_to_Framing == EMPTY)){

		/* start TX */

		/* clear buffer : TODO which buffer? */

		for (i = 0; i < SIZE_SSP_to_Control_Buffer; i++) {
			SSP_to_Control_Buffer_Copy[i]=SSP_to_Control_Buffer[i];
		}

		*a_control = AX25_getControl(S, RR, NS, g_Received_NR, pollfinal);

		flag_SSP_to_Control = CLEAR;

		prev_state = start_TX;

	}

	if((prev_state == idle) && (flag_Control_to_SSP == EMPTY && flag_Deframing_to_Control == FULL)){

		/* start RX */

		/* clear buffer : TODO which buffer? */

		for (i = 0; i < AX25_FRAME_MAX_SIZE; i++) {
			Deframing_To_Control_Buffer_Copy[i] = Deframing_To_Control_Buffer[i];
		}

		flag_Deframing_to_Control = EMPTY;



		prev_state = start_RX;

	}

	if((prev_state == start_TX) && (flag_Deframing_to_Control == FULL)){

		/* cont TX */

		/* TODO: check if S frame also check address */

		prev_state = idle;

	}

	if((prev_state == start_RX)){

		/* cont RX */

		prev_state = idle;

		*a_control = AX25_getControl(S, RR, NS, g_Received_NR, pollfinal); /* TODO: make S and RR variables */

		/* call function? AX25_buildFrame(buffer, info, frameSize, ADDR, control, infoSize); */

	}

	/*----------------------------------------------*/
	/* --------------------------- Beginning of TX part ---------------------------*/

	if(flag_TX == SET && flag_RX == CLEAR){

		NS=VS;

		/* make sure that max value for VS is 7 since it's only 3 bits */

		if(VS<7){
			VS++;
		}
		else{
			VS=0;
		}

		*a_control = AX25_getControl(S, RR, NS, g_Received_NR, pollfinal);
	}

	/* --------------------------- End of TX part ---------------------------*/

	/*TODO: after transmitting I frame how do i wait for receiving RR frame? */



	/* check which type I or S or U */
	control = g_control_recived[0];
	if ((control && 0x01) == 0) {

		/* type is I frame */

		g_Received_NR = (control & 0xE0) >> 5;
		NS = (control & 0x0E) >> 1;
		PollFinal = (control & 0x10) >> 4; /* TODO: check if it should be Poll or PollFinal */

		//		AX25_Manager_TX();

	} else if ((control && 0x03) == 1) {

		/* type is S frame */

		g_Received_NR = (control & 0xE0) >> 5;
		PollFinal = (control & 0x10) >> 4;
		Sbits = (control & 0x0C) >> 2;

		switch(Sbits){
		case RR:

			/*
			 * The RR frame is used to:
			 * 	Indicate that the sender of the RR is now able to receive more "I" frames.
			 * 	Acknowledge the proper reception of "I" frames up to "N(R)-1". TODO: check how to implement this line
			 * 	Clear a previously busy condition.
			 */

			flag_TX = CLEAR;
			flag_RX = SET;
			flag_busy = CLEAR;

			break;

		case RNR:

			/*
			 * The RNR frame is used to indicate that the station is temporarily busy and cannot accept more "I" frames.
			 * Frames up to "N(R)-1" are acknowledged. TODO: check how to implement this line
			 * Frames N(R) and above that may have been transmitted are discarded and must be retransmitted again.
			 */


			flag_TX = SET;
			flag_RX = CLEAR;
			flag_busy = SET;

			break;


		case REJ:

			/*
			 * The REJ frame is used to request the retransmission of "I" frames starting with
			 * N(R) and any frames sent with a sequence number "N(R)-1" are acknowledged.
			 */

			flag_TX = CLEAR;
			flag_RX = SET;

			break;

		case SREJ:

			/*
			 * The SREJ frame is used to request the retransmission of a single "I" frame numbered N(R).
			 */

			flag_TX = CLEAR;
			flag_RX = SET;

			break;

		}

	} else {

		/* type is U frame */

		Mbits = (control & 0xEC) >> 2;
		PollFinal = (control & 0x10) >> 4;

	}




#endif
}


void AX25_buildFrame(uint8 *buffer, uint8 *info, uint16 *frameSize, uint8 *ADDR,
		uint8 control, uint8 infoSize) {
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
	for (; i < infoSize + INFO_OFFSET; i++) {
		buffer[i] = *info;
		info++;
	}
	for (; i < FCS_OFFSET; i++) {
		buffer[i] = 0xaa;
	}

	/* Calculation and insertion of the FCS in the buffer. */
	AX25_putCRC(buffer, &i);
	buffer[i] = 0x7E;
	*frameSize = i + 1;

	flag_Control_to_Framing = EMPTY; /*  */
	flag_SerialTXBuffer = FULL;
}

/* TODO:remind Eng. Ahmed to make bit-stuffing mask */


/*
 * Description: takes whole frame and splits it into fields (address, control, info).
 * 				and also checks on the frame flags (namely 0x7E) and the CRC
 *
 * Parameters:
 * 				buffer: the buffer that contains the full frame which will be de-framed.
 * 				frameSize: the size of the frame.
 * 				infoSize: the size of the info field in that frame.
 *
 * Notes:
 * 				Sets the De-framing to Control flag
 *
 */

uint8 AX25_deFrame(uint8 *buffer, uint16 frameSize, uint8 infoSize) {
	uint8 newbuffer[AX25_FRAME_MAX_SIZE]; // this was set to frameSize, i changed it to AX25_FRAME_MAX_SIZE to test it.
	uint16 crc;
	uint8 *ptrz;
	uint16 i = 0;
	uint16 j;
	flag_RX_crc = CLEAR; /* initially assume CRC is wrong, if CRC is correct it will SET the flag. otherwise it will remain CLEAR */

	ptrz = (uint8*) &crc;
	for (; i < AX25_FRAME_MAX_SIZE; i++) {
		newbuffer[i] = buffer[i];
	}

	if (newbuffer[0] == 0x7E) {
		for (i = 1, j = 0; i < ADDR_LEN + ADDR_OFFSET; i++, j++) {
			g_received_address[j] = newbuffer[i];
		}
		for (j = 0; i < CNTRL_LEN + CNTRL_OFFSET; i++, j++) {
			g_control_recived[j] = newbuffer[i];
		}
		for (j = 0; i < infoSize + INFO_OFFSET; i++, j++) {
			g_info_reciver[j] = newbuffer[i];
		}
		for (j = 0; i < FCS_OFFSET; i++, j++) {
//			g_padding_recived[j] = newbuffer[i];
		}
		flag_Deframing_to_Control = FULL;
		crc = computeCRC(newbuffer, &i);
		ptrz++;
		if (*ptrz == newbuffer[i]) {
			i++;
			ptrz--;
			if (*ptrz == newbuffer[i]) {
				/*TODO: make flag RX_CRC (True or False) */
				flag_RX_crc = SET;
				i++;
				printf("\n**received frame**\n");
				if (newbuffer[i] == 0x7E) {
					printf("flag=: %x", newbuffer[i]);
					printf("\n address:\t");
					for (i = 0; i < ADDR_LEN; i++) {
						printf("%x", g_received_address[i]);
					}
					printf("\n control byte\t");
					for (i = 0; i < CNTRL_LEN; i++) {
						printf("control[%d]=%x\t", i, g_control_recived[i]);
					}

					printf("\n info \n");
					for (i = 0; i < infoSize; i++) {
						printf("%x", g_info_reciver[i]);
					}
					printf("\n padding: \t");
					for (i = 0; i < INFO_MAX_LEN-infoSize; i++) {
						printf("%x", g_padding_recived[i]);
					}
					printf("\nFCS\n");
					printf("\n CRC = %x\n", crc);
					printf("\n flag = %x \n", newbuffer[AX25_FRAME_MAX_SIZE-1]);
				}
			}
		}
	}
}

#if 1
void AX25_buildFrame_TEST(uint8 *buffer, uint8 *info, uint8 *ADDR,
		uint8 control, uint8 infoSize) {
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
	for (; i < infoSize + INFO_OFFSET; i++) {
		buffer[i] = *info;
		info++;
	}
	for (; i < FCS_OFFSET; i++) {
		buffer[i] = 0xaa;
	}

	/* Calculation and insertion of the FCS in the buffer. */
	AX25_putCRC(buffer, &i);
	buffer[i] = 0x7E;
}


#endif

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
