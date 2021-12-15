/*
 * ax25.h
 *
 *  Created on: Oct 7, 2021
 *      Author: Mohamed Ezzat
 */

#ifndef AX25_H_
#define AX25_H_

/***********************************************
 *                   includes                  *
 ***********************************************/
#include "std_types.h"

/***********************************************
 *                   definitions               *
 ***********************************************/
#define FLAG_LEN 1

#define AX25_FRAME_MAX_SIZE  256	// Fixed frame size

#define ADDR_LEN 14					// Address length in bytes
#define ADDR_OFFSET 1				// offset where address start (offset = 1, since flag is 1 byte)

#define CNTRL_OFFSET ADDR_LEN+ADDR_OFFSET
#define CNTRL_LEN 1

#define INFO_OFFSET CNTRL_LEN+CNTRL_OFFSET
#define INFO_LEN        236			// Max number of bytes for Info field. (256 is the default value).
#define INFO_MAX_LEN 	237

#define PADDING_LEN 5
#define PADDING_OFFSET INFO_LEN + ADDR_LEN + FLAG_LEN + CNTRL_LEN

#define FCS_OFFSET 253


#define FRAME_SIZE_WITHOUT_INFO_AND_PADDING 19

#define FULL 1U
#define EMPTY 0U

#define SIZE_SSP_to_Control_Buffer 236

/***********************************************
 *                   global variables          *
 ***********************************************/
typedef enum{
	I,S,U
}frameType;

typedef enum{
	RR, RNR, REJ, SREJ, SABME = 0x6C, DISC = 0x40, DM = 0x04, UA = 0x60, UI = 0, TEST = 0xE0
}frameSecondaryType;

typedef enum{
	CLEAR,SET
}flagMode;

typedef enum{
	idle, TX, RX, start_TX, start_RX
}controlLayerMode;


//extern uint8 flag_SSP_to_Control=EMPTY;
//extern uint8 flag_Control_to_Framing=EMPTY;
//extern uint8 flag_Control_to_SSP=EMPTY;
//extern uint8 flag_Deframing_to_Control=EMPTY;


/***********************************************
 *              Function Prototypes            *
 ***********************************************/
void AX25_buildFrame(uint8 *, uint8 *, uint16 * , uint8 *, uint8, uint8);
void AX25_deFrame(uint8*, uint16, uint8);
uint8 AX25_getControl(frameType frameType, frameSecondaryType secondaryType, uint8 NS, uint8 NR, uint8 pollFinal);
uint8 AX25_Manager();
void AX25_getInfo(uint8 * info);

#endif /* AX25_H_ */
