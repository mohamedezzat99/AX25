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

#define INFO_LEN        20		// Max number of bytes for Info field. (256 is the default value).
#define INFO_MAX_LEN 256
#define AX25_FRAME_MAX_SIZE  276	// Max number of bytes for an I-frame. (1+14+1+1+256+2+1).

#define ADDR_LEN 14					// Address length in bytes
#define ADDR_OFFSET 1				// offest where address start (offset = 1, since flag is 1 byte)

#define CNTRL_OFFSET 15
#define CNTRL_LEN 1

#define INFO_OFFSET 16

#define PADDING_LEN 5
#define PADDING_OFFSET INFO_LEN + ADDR_LEN + FLAG_LEN + CNTRL_LEN


#define FRAME_SIZE_WITHOUT_INFO_AND_PADDING 19

#define READY 1
#define NOT_READY 0




/***********************************************
 *              Function Prototypes            *
 ***********************************************/
void AX25_buildFrame(uint8 *, uint8 *, uint16 * , uint8 *, uint8, uint8 *, uint8 *);

#endif /* AX25_H_ */
