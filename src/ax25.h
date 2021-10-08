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
#define INFO_MAX_SIZE        256	// Max number of bytes for Info field. (256 is the default value).
#define AX25_FRAME_MAX_SIZE  276	// Max number of bytes for an AX25 UI-frame. (1+14+1+1+256+2+1).
#define ADDR_L 14					// could be 14 or 28
#define INFO_L 256					// default length

typedef struct{
	uint8 flag;
	uint8 address[ADDR_L];
	uint8 control;
	uint8 pid;
	uint8 info[INFO_L];
	uint16 fcs;
}TX_FRAME;


/***********************************************
 *              Function Prototypes            *
 ***********************************************/

void AX25_prepareUIFrame(uint8 *buffer, char *info, unsigned int length_info_field);
void AX25_prepareIFrame(TX_FRAME*);
void printTxFrame(TX_FRAME *tx_ptr);

#endif /* AX25_H_ */
