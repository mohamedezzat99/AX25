/*--------------------------------------------------------------------------*
 * AX25_CRC.c
 * FCS operations of the AX.25 frame.
 *--------------------------------------------------------------------------*/

#include "AX25_CRC.h"


/*--------------------------------------------------------------------------*
 * AX.25 FCS calculation (bitwise method).
 * CRC-16-CCITT G(x) = x16 + x12 + x5 + 1.
 * Polynom = 0x1021.
 *
 * PARAMETERS:
 * *buffer       pointer of the frame buffer.
 * *OpArrSize    it stores the index of the last inserted element in the array to keep track of size
 *
 * RETURN:
 * the CRC with a final XORed operation.
 *--------------------------------------------------------------------------*/
uint16 computeCRC(uint8 *buffer, uint16 * OpArrSize) {
	unsigned int i, j;
	unsigned short shiftRegister, outBit;
	char byte;
	/* Initialization of the Shift Register to 0xFFFF */
	shiftRegister = 0xFFFF;

	for(i=1 ; i<*OpArrSize; i++) {  /* The first flag is not calculated so i=1. */
		byte = buffer[i];

		for(j=0; j<8; j++) {
			outBit = shiftRegister & 0x0001;
			shiftRegister >>= 0x01;  /* Shift the register to the right. */

			if(outBit != (byte & 0x01)) {
				shiftRegister ^= 0x8408;  /* Mirrored polynom. */
				byte >>= 0x01;
				continue;
			}
			byte >>= 0x01;
		}
	}
	return shiftRegister ^ 0xFFFF;  /* Final XOR, FCS calculation. */
}


/*--------------------------------------------------------------------------*
 * AX.25 FCS positioning.
 * Put the FCS in the right place in the frame. The FCS is sent MSB first
 * so we prepare the 15th bit of the CRC to be sent first.
 *
 * PARAMETERS:
 * *buffer        pointer of the frame buffer.
 * *OpArrSize     it stores the index of the last inserted element in the array to keep track of size
 *--------------------------------------------------------------------------*/
void AX25_putCRC(uint8 *buffer, uint16 *OpArrSize) {
  uint16 crc;

  /* FCS calculation. */
  crc = computeCRC(buffer, OpArrSize);

  /* Put the FCS in the right place with the 15th bit to be sent first. */
	buffer[*OpArrSize] = ((crc >> 8) & 0xff);
	*OpArrSize=*OpArrSize+1;
	buffer[*OpArrSize] = (crc & 0xff);
	*OpArrSize=*OpArrSize+1;
}


